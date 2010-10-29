#include "output.hpp"
#include "renderer.hpp"
#include "chunk.hpp"
#include "image.hpp"
#include "intstring.hpp"

#include "render_contour.hpp"

#include <stdexcept>
#include <png.h>
#include <list>
#include <set>

/* Generate a list of renderers based on an option string. It is the
   callers responsibility to delete these renderers. If source is
   given, only one renderer will be created and no filename is parsed.
   All members of opts may be overridden except rotation and
   obliqueness. */
Renderer::RenderList Renderer::make_renderers(const std::string& options,
                                              const recipe* source) {
  RenderList result;

  /* Set up lists of requested options. */
  std::list<directionopt> rotations;
  std::list<ucharopt> lightlevels;
  std::list<boolopt> dimdepths;
  std::list<boolopt> angles;

  /* Type of renderer and list of requested overlays. */
  overlay_type type = DEFAULT;
  std::list<std::string> overlays;

  /* Separate filename and options. */
  size_t filestop = 0;
  std::string filename;
  if (!source) {
    filestop = options.find(':');
    filename = options.substr(0, filestop);
    if (filestop != std::string::npos)
      filestop++;
  }
  std::string opts;
  if (filestop != std::string::npos) {
    size_t optstop = options.find(':', filestop);
    if (optstop != std::string::npos) {
      opts = options.substr(filestop, optstop - filestop);

      /* Make a list of overlays */
      size_t olstop;
      size_t olstart = optstop + 1;
      do {
        olstop = options.find(':', olstart);

        overlays.push_back(options.substr(olstart, olstop - olstart));

        olstart = olstop + 1;
      } while (olstop != std::string::npos);
    } else {
      opts = options.substr(filestop);
    }
  }

  /* Read options. */
  size_t optstop;
  size_t optstart = 0;
  do {
    optstop = opts.find(',', optstart);
    std::string opt = opts.substr(optstart, optstop - optstart);
    if (!opt.empty()) {
      if (opt == "contour") {
        if (type != DEFAULT && type != CONTOUR) {
          throw std::logic_error("Multiple overlays must be separated using "
                                 "the colon character.");
        }
        type = CONTOUR;
      } else if (opt == "n" || opt == "north") {
        rotations.push_back(directionopt(N, "north"));
      } else if (opt == "ne" || opt == "northeast") {
        rotations.push_back(directionopt(NE, "northeast"));
      } else if (opt == "e" || opt == "east") {
        rotations.push_back(directionopt(E, "east"));
      } else if (opt == "se" || opt == "southeast") {
        rotations.push_back(directionopt(SE, "southeast"));
      } else if (opt == "s" || opt == "south") {
        rotations.push_back(directionopt(S, "south"));
      } else if (opt == "sw" || opt == "southwest") {
        rotations.push_back(directionopt(SW, "southwest"));
      } else if (opt == "w" || opt == "west") {
        rotations.push_back(directionopt(W, "west"));
      } else if (opt == "nw" || opt == "northwest") {
        rotations.push_back(directionopt(NW, "northwest"));
      } else if (opt == "cardinal") {
        rotations.push_back(directionopt(N, "north"));
        rotations.push_back(directionopt(E, "east"));
        rotations.push_back(directionopt(S, "south"));
        rotations.push_back(directionopt(W, "west"));
      } else if (opt == "ordinal") {
        rotations.push_back(directionopt(NE, "northeast"));
        rotations.push_back(directionopt(SE, "southeast"));
        rotations.push_back(directionopt(SW, "southwest"));
        rotations.push_back(directionopt(NW, "northwest"));
      } else if (opt == "dimdepth") {
        dimdepths.push_back(boolopt(true, opt));
      } else if (opt == "litdepth") {
        dimdepths.push_back(boolopt(false, opt));
      } else if (opt == "oblique") {
        angles.push_back(boolopt(true, opt));
      } else if (opt == "topdown") {
        angles.push_back(boolopt(false, opt));
      } else if (opt == "day") {
        lightlevels.push_back(ucharopt(255, opt));
      } else if (opt == "night") {
        lightlevels.push_back(ucharopt(50, opt));
      } else if (opt == "twilight") {
        lightlevels.push_back(ucharopt(127, opt));
      } else if (opt.substr(0, 5) == "light") {
        try {
          int light = stringtoint(opt.substr(5));
          if (light < 0 || light > 255) {
            throw std::logic_error(string("Light level out of range: ")
                                   + opt);
          }
          lightlevels.push_back(ucharopt(light, opt));
        } catch (std::runtime_error& e) {
          throw std::logic_error(string("Invalid light level specified: ")
                                 + opt);
        }
      } else {
        throw std::logic_error(string("Invalid renderer option: ") + opt);
      }
    }

    optstart = optstop + 1;
  } while (optstop != std::string::npos);

  /* Merge options into opts, if given. */
  if (source) {
    /* No rotations or obliqueness may be given. */
    if (!rotations.empty()) {
      throw std::logic_error("You cannot specify rotation for overlays.");
    }
    if (!angles.empty()) {
      throw std::logic_error("You cannot specify angles for overlays.");
    }
    rotations.push_back(source->dir);
    angles.push_back(source->oblique);

    /* No multiples allowed. */
    if (lightlevels.size() > 1 || dimdepths.size() > 1) {
      throw std::logic_error("You cannot specify multiples for overlays.");
    }

    /* Use recipe options if none were specified. */
    if (lightlevels.empty()) {
      lightlevels.push_back(source->lightlevel);
    }
    if (dimdepths.empty()) {
      dimdepths.push_back(source->dimdepth);
    }
  } else {
    /* If any lists are empty, add defaults. */
    if (rotations.empty())
      rotations.push_back(directionopt(N, "north"));    // North-facing map.
    if (lightlevels.empty())
      lightlevels.push_back(ucharopt(127, "twilight")); // Twilight.
    if (dimdepths.empty())
      dimdepths.push_back(boolopt(true, "dimdepth"));   // Dim deep areas.
    if (angles.empty())
      angles.push_back(boolopt(false, "topdown"));      // Top down map.

    /* If any lists have more than 1 entries, we need
       wildcards in the filename. */
    if ((rotations.size() > 1) &&
        (filename.find("%r") == std::string::npos)) {
      throw std::logic_error("Rotation multiple needs %r in filename.");
    }
    if ((lightlevels.size() > 1) &&
        (filename.find("%l") == std::string::npos)) {
      throw std::logic_error("Light level multiple needs %l in filename.");
    }
    if ((dimdepths.size() > 1) &&
        (filename.find("%d") == std::string::npos)) {
      throw std::logic_error("Depth dimming multiple needs %d in filename.");
    }
    if ((angles.size() > 1) &&
        (filename.find("%a") == std::string::npos)) {
      throw std::logic_error("Angle multiple needs %a in filename.");
    }
  }

  /* Set of outputs so far. Used to avoid duplicates. */
  std::set<std::string> filenames;

  /* Generate renderers. */
  for (std::list<directionopt>::const_iterator rotation = rotations.begin();
       rotation != rotations.end(); ++rotation) {
    std::string rotfile = wildcard(filename, "%r", rotation->second);

    for (std::list<ucharopt>::const_iterator lightlevel = lightlevels.begin();
         lightlevel != lightlevels.end(); ++lightlevel) {
      std::string lightfile = wildcard(rotfile, "%l", lightlevel->second);

      for (std::list<boolopt>::const_iterator dimdepth = dimdepths.begin();
           dimdepth != dimdepths.end(); ++dimdepth) {
        std::string dimfile = wildcard(lightfile, "%d", dimdepth->second);

        for (std::list<boolopt>::const_iterator angle = angles.begin();
             angle != angles.end(); ++angle) {
          std::string anglefile = wildcard(dimfile, "%a", angle->second);

          /* Make sure we have a unique filename for this renderer. */
          std::pair<std::set<std::string>::iterator, bool> unique =
            filenames.insert(anglefile);
          if (!unique.second) {
            debug << "Duplicate file name. Skipping " << anglefile
                  << std::endl;
            continue;
          }

          /* Make recipe struct. */
          const recipe target = {*rotation, *lightlevel, *dimdepth, *angle};

          /* Create renderer depending on type. */
          Renderer* add;

          switch (type) {
          case DEFAULT:
            add = new Renderer(anglefile, target);
            break;
          case CONTOUR:
            add = new Render_Contour(anglefile, target);
            break;
          }

          /* Add overlays. */
          for (std::list<std::string>::const_iterator
                 overlay = overlays.begin(); overlay != overlays.end();
               ++overlay) {
            RenderList r = make_renderers(*overlay, &target);
            add->overlay(*(r.begin()));
          }

          /* Add new renderer to result list. */
          result.push_back(add);
        }
      }
    }
  }

  return result;
}

/* Construct renderer. */
Renderer::Renderer(const std::string& filename, const recipe& options)
  : options(options), filename(filename),
    image(0), lightbuffer(0), dpos(0), finalised(false) {
  /* Make a local copy of the default colours. */
  for (int i = 0; i < 256; i++) {
    colours[i] = default_colours[i];
  }
}

/* Release memory. */
Renderer::~Renderer() {
  RenderList::iterator overlay = overlays.begin();
  while (!overlays.empty()) {
    delete *overlay;
    overlay = overlays.erase(overlay);
  }

  delete image;
  delete [] lightbuffer;
  delete [] dpos;
}

/* Let the renderer know the coordinates of the map corners. */
void Renderer::set_surface(const Level::position& top_right_chunk,
                           const Level::position& bottom_left_chunk) {
  top_right.x = top_right_chunk.first;
  top_right.z = top_right_chunk.second;
  top_right.y = 0;
  bottom_left.x = bottom_left_chunk.first;
  bottom_left.z = bottom_left_chunk.second;
  bottom_left.y = 0;
  pvector adjust = pvector(1, 1);
  pvector size = ((bottom_left - top_right) + adjust) * 16;

  /* Make sure we only do this once. */
  if (image)
    throw std::runtime_error("Attempted to allocate image memory twice.");

  if (!options.oblique.first) {
    /* Allocate memory for a flat unrotated map. We may rotate
       it when rendering is finished. */
    image = new Image(Image::ivector(size.z, size.x));
  } else {
    /* Calculate size of oblique map. */
    if (options.dir.first & CARDINAL) {
      if (options.dir.first & (N | S)) {
        /* North or south facing map. */
        image = new Image(Image::ivector(size.z, size.x + 128));
      } else {
        /* East or west facing map. */
        image = new Image(Image::ivector(size.x, size.z + 128));
      }
    } else if (options.dir.first & ORDINAL) {
      /* TODO: Calculate size of ordinal oblique map. */
      throw std::logic_error("This alpha release cannot render "
                             "ordinal oblique maps yet.");
    } else {
      /* Oblique TOP or BOTTOM is not possible. */
      throw std::runtime_error("Invalid oblique direction for allocation.");
    }

    /* Allocate space for lighting buffer. */
    lightbuffer = new Pixel[image->dimensions().x * 128];
    dpos = new int[image->dimensions().x / 16];
  }

  /* Allocate for all overlays too. */
  for (RenderList::iterator overlay = overlays.begin();
       overlay != overlays.end(); ++overlay) {
    (*overlay)->set_surface(top_right_chunk, bottom_left_chunk);
  }
}

/* Pass a chunk to the renderer and let it do its thing. */
void Renderer::render(const Chunk& chunk) {
  if (!options.oblique.first) {
    /* Flat map. Render it unrotated. We may rotate it when
       all chunks are rendered. */
    for (int x = 0; x < 16; x++) {
#pragma omp parallel for
      for (int z = 0; z < 16; z++) {
        Pixel dot;
        for (int y = 127; y >= 0; y--) {
          blendblock(chunk, pvector(x, z, y), TOP, dot);
          if (dot.A >= 1.0f - 0.004f) {
            /* Done with this pixel. */
            dot.A = 1.0f;
            break;
          }
        }

        /* Paint new dot to map. */
        int img_x = (bottom_left.z - chunk.get_position().z) * 16
          + (15 - z);
        int img_y = (chunk.get_position().x - top_right.x) * 16 + x;
        (*image)(img_x, img_y) = dot;
      }
    }
  } else {
    /* Oblique map. */
    if (options.dir.first & CARDINAL) {
      /* Facing a cardinal direction. */
      bool front_to_back = options.dir.first & (N | E);

      /* Calculate image coordinate offset of chunk. */
      int off_x, off_y;
      switch (options.dir.first) {
      case N:
        /* Facing north. */
        off_x = (bottom_left.z - chunk.get_position().z) * 16;
        off_y = (chunk.get_position().x - top_right.x) * 16
          + 127 + 16;
        break;

      case E:
        /* Facing east. */
        off_x = (chunk.get_position().x - top_right.x) * 16;
        off_y = (chunk.get_position().z - top_right.z) * 16
          + 127 + 16;
        break;

      case S:
        /* Facing south. */
        off_x = (chunk.get_position().z - top_right.z) * 16;
        off_y = (bottom_left.x - chunk.get_position().x) * 16
          + 127 + 16;
        break;

      case W:
        /* Facing west. */
        off_x = (bottom_left.x - chunk.get_position().x) * 16;
        off_y = (bottom_left.z - chunk.get_position().z) * 16
          + 127 + 16;
        break;
      }

      for (int w = 0; w < 16; w++) {
#pragma omp parallel for
        for (int y = 16 + 127; y >= 0; y--) {
          /* Calculate image coordinates. */
          int img_y = off_y - y;
          int img_x = off_x + w;

          /* Get initial pixel colour. */
          Pixel dot;
          if (front_to_back) {
            dot = (*image)(img_x, img_y);
            if (dot.A >= 1.0f - 0.004f) {
              /* Pixel is already finished. */
              dot.A = 1.0f;
              continue;
            }
          }

          /* Start of raycast. */
          int depth = 0;
          int ystep = y;
          direction step = negate_direction(options.dir.first);

          /* If y is more than 127, we are looking at the top of the chunk. */
          if (y > 127) {
            step = TOP;
            ystep = 127;
            depth = y - 128;
          }

          /* Raycast back and down. */
          while (ystep >= 0 && depth < 16) {
            /* Convert depth and height to position inside chunk. */
            pvector pos(0, 0, ystep);
            switch (options.dir.first) {
            case N:
              /* Facing north. */
              pos.x = 15 - depth;
              pos.z = 15 - w;
              break;

            case E:
              /* Facing east. */
              pos.x = w;
              pos.z = 15 - depth;
              break;

            case S:
              /* Facing south. */
              pos.x = depth;
              pos.z = w;
              break;

            case W:
              /* Facing west. */
              pos.x = 15 - w;
              pos.z = depth;
              break;
            }

            /* Blend the current block onto the pixel. */
            int lbufferpos = img_x * 128 + pos.y;
            if ((depth == 0) && (step & CARDINAL)) {
              /* Front of chunk. */
              if (front_to_back) {
                /* Chunk in front saved lighting data. */
                double light = lightbuffer[lbufferpos].A;
                if (dpos[off_x / 16] != off_y + 16) {
                  light = 0; // Chunk in front doesn't exist.
                }
                blendpixel(getblock(chunk, pos, step), pos.y, light, dot);
              } else {
                /* Don't render. Save colour value for chunk in front. */
                lightbuffer[lbufferpos] = getblock(chunk, pos, step);
              }

            } else if ((depth == 15) && (step & TOP)) {
              /* Back of chunk. */
              if (front_to_back) {
                /* Render and save lighting data for chunk behind. */
                blendblock(chunk, pos, step, dot);
                lightbuffer[lbufferpos].A = getlight(chunk, pos, ALL);
              } else {
                /* Render self and saved colour value from chunk in front. */
                blendblock(chunk, pos, step, dot);
                if (dpos[off_x / 16] == off_y - 16) {
                  /* Chunk behind exists. */
                  blendpixel(lightbuffer[lbufferpos], pos.y,
                             getlight(chunk, pos, ALL), dot);
                }
              }

            } else {
              /* Inside chunk. */
              blendblock(chunk, pos, step, dot);
            }

            /* We are done with this pixel if it is opaque. */
            if (dot.A >= 1.0f - 0.004f) {
              dot.A = 1.0f;
              break;
            }

            /* Step to the block behind this one (in staircase steps). */
            if (step & CARDINAL) {
              /* We just got the side of a block. The pixel behind is
                 the top of the block below. */
              step = TOP;
              ystep--;
            } else {
              /* We just got the top of a block. The pixel behind is
                 the side of the block behind. */
              step = negate_direction(options.dir.first);
              depth++;
            }
          }

          /* Paint new dot to map. */
          if (front_to_back) {
            (*image)(img_x, img_y) = dot;
          } else {
            (*image)(img_x, img_y).blend_over(dot);
          }
        }
      }

      /* Remember depth of this chunk. */
      dpos[off_x / 16] = off_y;

    } else if (options.dir.first & ORDINAL) {
      /* TODO: Render oblique images. */
    } else {
      throw std::runtime_error("Invalid render direction.");
    }
  }

  /* Render all overlays too. */
  for (RenderList::iterator overlay = overlays.begin();
       overlay != overlays.end(); ++overlay) {
    (*overlay)->render(chunk);
  }
}

/* Make any last minute adjustments. */
void Renderer::finalise() {
  if (!image) {
    throw std::logic_error("Finalising failed: No image.");
  }

  if (!options.oblique.first) {
    /* Top-down images must be rotated. */
    int angle;
    switch (options.dir.first) {
    case N: angle = 0; break;
    case NE: angle = 7; break;
    case E: angle = 6; break;
    case SE: angle = 5; break;
    case S: angle = 4; break;
    case SW: angle = 3; break;
    case W: angle = 2; break;
    case NW: angle = 1; break;
    default:
      throw std::logic_error("Invalid rotation of image.");
    }
    Image* rotate = new Image(*image, angle);
    delete image;
    image = rotate;
  }

  /* Blend overlays. */
  for (RenderList::iterator it = overlays.begin();
       it != overlays.end(); ++it) {
    (*it)->finalise();

    image->overlay((*it)->get_image());
  }

  finalised = true;
}

/* Finalise and save image. */
void Renderer::save() {
  if (!finalised)
    finalise();

  /* Trim on ordinal rotations and all oblique angles. */
  bool trim = options.oblique.first || (options.dir.first & ORDINAL);

  image->output(filename, trim);
}

/* Get colour value of a block. */
Pixel Renderer::getblock(const Chunk& chunk, pvector pos,
                         direction dir) {
  /* Fetch block from target. */
  unsigned char type = chunk.blocks(pos);
  Pixel result = (dir & TOP) ? colours[type].top : colours[type].side;

  if (type == 0x08 || type == 0x09) {
    /* Block is water. Set alpha based on depth. */
    try {
      unsigned char invdepth = chunk.data(pos);
      if (invdepth > 0) {
        result.A = 1.0f - (double(invdepth) / 15.0f);
      }
    } catch (std::runtime_error& e) { /* Data was not loaded. */ }
  }

  return result;
}

/* Get lighting level of a block. */
double Renderer::getlight(const Chunk& chunk, pvector pos, direction dir) {
  if (dir == TOP) {
    pos.y++;
  } else if (dir == N) {
    pos.x--;
  } else if (dir == E) {
    pos.z--;
  } else if (dir == S) {
    pos.x++;
  } else if (dir == W) {
    pos.z++;
  } else if (dir == BOTTOM) {
    pos.y--;
  } else if (dir != ALL) {
    throw std::runtime_error("Cannot get lighting of diagonal block.");
  }

  double l_sky = 0;
  double l_block = 0;

  if (pos.y > 127 || pos.y < 0) {
    /* There is no lighting data above or below the map. */
    l_sky = 1; // Fully lit by sky.
    l_block = 0; // Not lit by other sources.

  } else {
    try {
      l_sky = double(chunk.skylight(pos)) / 15.0f;
    } catch (std::runtime_error& e) {
      /* Skylight data not loaded. */
    }
    try {
      l_block = double(chunk.blocklight(pos)) / 15.0f;
    } catch (std::runtime_error& e) {
      /* Blocklight data not loaded. */
    }
  }

  /* Balance lighting. */
  return ((l_sky * options.lightlevel.first) / 255) +
    ((l_block * (255 - options.lightlevel.first)) / 255);
}

/* Get a block, light it and blend behind a pixel. */
void Renderer::blendblock(const Chunk& chunk, pvector pos,
                          direction dir, Pixel& top) {
  blendpixel(getblock(chunk, pos, dir), pos.y,
             getlight(chunk, pos, dir), top);
}

/* Blend a pixel underneath another based on light value and depth. */
void Renderer::blendpixel(const Pixel& source, unsigned char depth,
                          double light, Pixel& target) {
  if (source.A > 0) {
    Pixel mod = source;
    mod.light(light);
    if (options.dimdepth.first)
      mod.light(0.5f + double(depth) / 256.0f);

    target.blend_under(mod);
  }
}

/* Negate a cardinal or ordinal direction. */
Renderer::direction Renderer::negate_direction(direction direction) {
  switch (direction) {
  case N:  return S;
  case NE: return SW;
  case E:  return W;
  case SE: return NW;
  case S:  return N;
  case SW: return NE;
  case W:  return E;
  case NW: return SE;
  case TOP:    return BOTTOM;
  case BOTTOM: return TOP;
  default:
    throw std::logic_error("Cannot negate compound direction.");
  }
}

/* Return a reference to the image. Can only be done after it has been
   finalised. The reference is valid until the renderer is deleted. */
const Image& Renderer::get_image() const {
  if (finalised)
    return *image;

  throw std::runtime_error("Trying to fetch unfinished image.");
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
      if (tex & CARDINAL) {
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
        if (tex & CARDINAL) {
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
      if (tex & CARDINAL) {
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
        if (tex & CARDINAL) {
          colours[i].side.A = alpha;
        }
      }
    }
  }
}

/* Wildcard replacement for file names. */
std::string Renderer::wildcard(const std::string& filename,
                               const std::string& wildcard,
                               const std::string& replacement) {
    std::string result = filename;
    size_t cardpos;

    while ((cardpos = result.find(wildcard)) != std::string::npos) {
      result.replace(cardpos, 2, replacement, 0, replacement.length());
    };

    return result;
}
