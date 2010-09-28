#include "renderer.hpp"
#include "chunk.hpp"
#include "image.hpp"

#include <stdexcept>
#include <png.h>

/* Pure renderers cannot be constructed. */
Renderer::Renderer(bool oblique, direction up, unsigned int skylight)
  : up(up), oblique(oblique), skylight(skylight), image(0) {
  /* Make a local copy of the default colours. */
  for (int i = 0; i < 256; i++) {
    colours[i] = default_colours[i];
  }
}

/* Release memory. */
Renderer::~Renderer() {
  delete image;
}

/* Let the renderer know the coordinates of the map corners. */
void Renderer::set_surface(const pvector& top_right_chunk,
                           const pvector& bottom_left_chunk) {
  top_right = top_right_chunk;
  bottom_left = bottom_left_chunk;

  delete image; // Just in case we are rendering more than once.

  if (!oblique) {
    /* Allocate memory for a flat unrotated map. We may rotate
       it when rendering is finished. */
    pvector adjust = {1, 1};
    pvector size = ((bottom_left - top_right) + adjust) * 16;
    image = new Image({size.z, size.x});
  } else {
    /* TODO: Calculate size of oblique map. */
  }
}

/* Pass a chunk to the renderer and let it do its thing. */
void Renderer::render(Chunk& chunk) {
  if (!oblique) {
    /* Flat map. Render it unrotated. We may rotate it when
       all chunks are rendered. */
    for (int x = 0; x < 16; x++) {
      for (int z = 0; z < 16; z++) {
        Pixel dot;
        for (int y = 127; y >= 0; y--) {
          Pixel under = getblock(chunk, {x, z, y}, TOP);
          if (under.A > 0) {
            under.light(getlight(chunk, {x, z, y}, TOP));
            if (under.A > 0) {
              dot.blend_under(under);
              if (dot.A > 0xf0) {
                /* If pixel is almost opaque, we're close enough. */
                dot.A = 0xff;
                break;
              }
            }
          }
        }

        /* Paint new dot to map. */
        int img_x = (bottom_left.z - chunk.get_position().z) * 16 + (15 - z);
        int img_y = (chunk.get_position().x - top_right.x) * 16 + x;
        (*image)(img_x, img_y) = dot;
      }
    }
  } else {
    /* Oblique map. */
    /* TODO */
  }
}

/* Finalise and save image. */
void Renderer::save(std::string filename) {
  /* TODO: Finalise. */
  if (image) {
    int angle;
    switch (up) {
    case N: angle = 0; break;
    case NE: angle = 1; break;
    case E: angle = 2; break;
    case SE: angle = 3; break;
    case S: angle = 4; break;
    case SW: angle = 5; break;
    case W: angle = 6; break;
    case NW: angle = 7; break;
    default:
      throw std::logic_error("Invalid rotation of image.");
    }
    Image rotate(*image, angle);
    rotate.output(filename, angle % 2); // Trim on diagonals.
  } else {
    throw std::logic_error("Attempted to save unrendered image.");
  }
}

/* Get colour value of a block. */
Pixel Renderer::getblock(Chunk& chunk, const pvector& pos, direction dir) {
  unsigned char type = chunk.blocks(pos);
  Pixel result = (dir & TOP) ? colours[type].top : colours[type].side;

  if ((type == 0x08 || type == 0x09) && (dir & TOP)) {
    /* Block is water. Set top alpha based on depth. */
    try {
      unsigned char invdepth = chunk.data(pos);
      if (invdepth > 0) {
        result.A = 0xff - invdepth * 0x0f;
      }
    } catch (std::runtime_error& e) { /* Data was not loaded. */ }
  }

  return result;
}

/* Get lighting level of a block. */
unsigned char Renderer::getlight(Chunk& chunk, const pvector& pos,
                                 direction dir) {
  pvector source = pos;
  if (dir & TOP) {
    source.y++;
  } else if (dir & N) {
    source.x--;
  } else if (dir & E) {
    source.z--;
  } else if (dir & S) {
    source.x++;
  } else if (dir & W) {
    source.z++;
  }

  unsigned char l_sky = 0;
  unsigned char l_block = 0;

  if (source.y > 127 || source.y < 0) {
    /* There is no lighting data above or below the map. */
    l_sky = 255;
    l_block = 0;
  } else if (source.x < 0 || source.x > 15 || source.z < 0 || source.z > 15) {
    /* Lighting data is in another chunk. */
    /* TODO */
  } else {
    try {
      l_sky = chunk.skylight(source) * 17;
    } catch (std::runtime_error& e) {
      /* Skylight data not loaded. */
      l_sky = 0;
    }
    try {
      l_block = chunk.blocklight(source) * 17;
    } catch (std::runtime_error& e) {
      /* Blocklight data not loaded. */
      l_block = 0;
    }
  }

  /* Balance lighting. */
  return ((l_sky * skylight) / 255) + ((l_block * (255 - skylight)) / 255);
}

/* Set the alpha channel of certain block types. */
void Renderer::set_default_alpha(std::vector<unsigned char>& types,
                                 unsigned char alpha,
                                 bool invertlist, direction tex) {
  if (!invertlist) {
    /* Set the alpha of each block type in the list. */
    for (int i = 0; i < types.size(); i++) {
      if (tex & TOP) {
        default_colours[types[i]].top.A = alpha;
      }
      if (tex & SIDES) {
        default_colours[types[i]].side.A = alpha;
      }
    }
  } else {
    /* Only set the alpha if a type is NOT in the list. */
    for (int i = 0; i < 256; i++) {
      bool found = false;
      for (int j = 0; j < types.size(); j++) {
        if (types[j] == i) {
          found = true;
          break;
        }
      }
      if (!found) {
        if (tex & TOP) {
          default_colours[i].top.A = alpha;
        }
        if (tex & SIDES) {
          default_colours[i].side.A = alpha;
        }
      }
    }
  }
}

/* Set the alpha channel of certain block types. */
void Renderer::set_alpha(std::vector<unsigned char>& types,
                         unsigned char alpha,
                         bool invertlist, direction tex) {
  if (!invertlist) {
    /* Set the alpha of each block type in the list. */
    for (int i = 0; i < types.size(); i++) {
      if (tex & TOP) {
        colours[types[i]].top.A = alpha;
      }
      if (tex & SIDES) {
        colours[types[i]].side.A = alpha;
      }
    }
  } else {
    /* Only set the alpha if a type is NOT in the list. */
    for (int i = 0; i < 256; i++) {
      bool found = false;
      for (int j = 0; j < types.size(); j++) {
        if (types[j] == i) {
          found = true;
          break;
        }
      }
      if (!found) {
        if (tex & TOP) {
          colours[i].top.A = alpha;
        }
        if (tex & SIDES) {
          colours[i].side.A = alpha;
        }
      }
    }
  }
}
