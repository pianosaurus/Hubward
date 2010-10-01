#include "renderer.hpp"
#include "chunk.hpp"
#include "image.hpp"
#include "intstring.hpp"

#include <stdexcept>
#include <png.h>
#include <list>

/* Pure renderers cannot be constructed. */
Renderer::Renderer(bool oblique, direction up, unsigned int skylight)
  : up(up), oblique(oblique), skylight(skylight), image(0) {
  /* Make a local copy of the default colours. */
  for (int i = 0; i < 256; i++) {
    colours[i] = default_colours[i];
  }
}

/* Initialise a renderer based on an option string. Options will be parsed
   up to the first "-- ", and parsed options will be removed. */
Renderer::Renderer(std::string& options) : up(N), oblique(false),
                                           skylight(127), image(0) {
  /* Make a local copy of the default colours. */
  for (int i = 0; i < 256; i++) {
    colours[i] = default_colours[i];
  }

  /* Parse options. */
  std::list<char> opts; // Used short options.

  size_t at = options.find('-');
  bool canlong = true;
  bool didlong = false;

  while (at != string::npos) {
    at++;
    if (at >= options.length())
      break;

    /* Get short option equivalent. */
    char shortopt = 0;
    if (canlong && options[at] == '-') {
      /* Long option. */
      size_t stop = options.find_first_of("= ", at);
      string longopt;
      if (stop == string::npos)
        longopt = options.substr(at + 1);
      else
        longopt = options.substr(at + 1, stop - at - 1);

      if (longopt.empty()) {
        /* Found --, done parsing. */
        at++;
        break;
      }
      at = stop;
      for (int i = 0;
           i < (sizeof(Renderer::options) / sizeof(Renderer::option)); i++) {
        if (longopt == Renderer::options[i].longopt) {
          shortopt = Renderer::options[i].shortopt;
          break;
        }
      }
      if (shortopt == 0) {
        throw std::logic_error(string("Unknown option --") + longopt);
      }
      didlong = true;
    } else {
      /* Short option. */
      canlong = false;
      shortopt = options[at];
    }

    /* Make sure the option is not specified twice. */
    for (std::list<char>::const_iterator it = opts.begin();
         it != opts.end(); ++it) {
      if (*it == shortopt) {
        throw std::logic_error(string("Option -") + shortopt +
                               " specified twice.");
      }
    }

    /* Parse option and handle any arguments. */
    bool ok = false;
    std::string argument;
    size_t stop = at;
    if (at != string::npos) {
      at = options.find_first_not_of(' ', at);
      stop = options.find(' ', at);
      argument = options.substr(at + 1, stop - at - 1);
    }
    for (int i = 0;
         i < (sizeof(Renderer::options) / sizeof(Renderer::option)); i++) {
      if (Renderer::options[i].shortopt == shortopt) {
        if (Renderer::options[i].argname.length() > 0) {
          parseoption(shortopt, &argument);
          at = stop;
        } else {
          parseoption(shortopt);
        }
        ok = true;
        break;
      }
    }
    if (!ok) {
      throw std::logic_error(string("Unknown option -") + shortopt);
    }

    opts.push_back(shortopt);

    if (at == string::npos) {
      /* Done. */
      break;
    } else if (options[at] == ' ' || didlong) {
      at = options.find('-', at);
      canlong = true;
    }
  }

  if (at == string::npos) {
    options = "";
  } else {
    options = options.substr(at);
  }

  /* Make sure a filename was set. */
  if (filename.empty()) {
    throw std::logic_error("No output filename set.");
  }
}


/* Release memory. */
Renderer::~Renderer() {
  delete image;
}

/* Let the renderer know the coordinates of the map corners. */
void Renderer::set_surface(const Level::position& top_right_chunk,
                           const Level::position& bottom_left_chunk) {
  top_right = { top_right_chunk.second, top_right_chunk.first, 0 };
  bottom_left = { bottom_left_chunk.second, bottom_left_chunk.first, 0 };

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
void Renderer::render(const Chunk& chunk,
                      const Chunk& north, const Chunk& east,
                      const Chunk& south, const Chunk& west) {
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
            /* Adjust lighting slightly depending on height in flat mode. */
            if (!oblique) {
              under.light(y + 128);
            }
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
void Renderer::save() {
  /* TODO: Finalise. */
  if (image) {
    int angle;
    switch (up) {
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
    Image rotate(*image, angle);
    rotate.output(filename, angle % 2); // Trim on diagonals.
  } else {
    throw std::logic_error("Attempted to save unrendered image.");
  }
}

/* Parse an option. */
void Renderer::parseoption(char shortopt, std::string* argument) {
  switch (shortopt) {
  case 'l': {
    /* Daylight percentage. */
    if (*argument == "day" || *argument == "Day" || *argument == "DAY") {
      skylight = 255;
    } else if (*argument == "night" || *argument == "Night" ||
               *argument == "NIGHT") {
      skylight = 51;
    } else if (*argument == "twilight" || *argument == "Twilight"
               || *argument == "TWILIGHT") {
      skylight = 127;
    } else {
      try {
        int light = stringtoint(*argument);
        if (light < 0 || light > 100) {
          throw std::runtime_error("Argument to -l must be "
                                   "between 0 and 100.");
        }

        skylight = (255 * light) / 100;
      } catch (std::runtime_error& e) {
        throw std::runtime_error("Invalid argument to -l.");
      }
    }
  } break;

  case 'o': {
    /* Oblique mode. */
    oblique = true;
  } break;

  case 'O': {
    /* Output file name. */
    filename = *argument;
  } break;

  case 'r': {
    /* Rotate. */
    int upint = -1;
    try {
      upint = stringtoint(*argument);
    } catch (std::runtime_error& e) {}

    if (*argument == "N" || *argument == "n" || upint == 0) {
      up = N;
    } else if (*argument == "NE" || *argument == "ne" || upint == 7) {
      up = NE;
    } else if (*argument == "E" || *argument == "e" || upint == 6) {
      up = E;
    } else if (*argument == "SE" || *argument == "se" || upint == 5) {
      up = SE;
    } else if (*argument == "S" || *argument == "s" || upint == 4) {
      up = S;
    } else if (*argument == "SW" || *argument == "sw" || upint == 3) {
      up = SW;
    } else if (*argument == "W" || *argument == "w" || upint == 2) {
      up = W;
    } else if (*argument == "NW" || *argument == "nw" || upint == 1) {
      up = NW;
    } else {
      throw std::runtime_error("Invalid argument to -r.");
    }
  } break;

  default:
    throw std::runtime_error(string("Unable to handle option -") + shortopt);
  }
}

/* Get colour value of a block. */
Pixel Renderer::getblock(const Chunk& chunk, const pvector& pos,
                         direction dir) {
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
unsigned char Renderer::getlight(const Chunk& chunk, const pvector& pos,
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

/* Define renderer arguments. */
const Renderer::option Renderer::options[] = {
  {'l', "light", "n",
   "The percentage of daylight to render. 20 gives a good\n"
   "night view. May also be one of day, night, twilight.", "50"},
  {'r', "rotate", "n",
   "Rotate the map n*45 degrees in a clockwise direction.\n"
   "You may also specify one of N, NE, E, SE, S, SW, W, NW.", "N"},
  {'o', "oblique", "", "Not yet implemented."},
  {'O', "output", "filename",
   "Output filename, typically specified with a .png suffix."}
};
