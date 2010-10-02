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
  std::list<boolopt> obliques;

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
      } else if (opt == "dimdepth") {
        dimdepths.push_back(boolopt(true, opt));
      } else if (opt == "litdepth") {
        dimdepths.push_back(boolopt(false, opt));
      } else if (opt == "oblique") {
        obliques.push_back(boolopt(true, opt));
      } else if (opt == "topdown") {
        obliques.push_back(boolopt(false, opt));
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
    if (!rotations.empty()) {
      throw std::logic_error("You cannot specify obliqueness for overlays.");
    }
    rotations.push_back(source->dir);
    obliques.push_back(source->oblique);

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
    if (obliques.empty())
      obliques.push_back(boolopt(false, "topdown"));    // Top down map.

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
    if ((obliques.size() > 1) &&
        (filename.find("%o") == std::string::npos)) {
      throw std::logic_error("Obliqueness multiple needs %o in filename.");
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

        for (std::list<boolopt>::const_iterator oblique = obliques.begin();
             oblique != obliques.end(); ++oblique) {
          std::string obliquefile = wildcard(dimfile, "%o", oblique->second);

          /* Make sure we have a unique filename for this renderer. */
          std::pair<std::set<std::string>::iterator, bool> unique =
            filenames.insert(obliquefile);
          if (!unique.second) {
            continue;
          }

          /* Make recipe struct. */
          const recipe target = {*rotation, *lightlevel, *dimdepth, *oblique};

          /* Create renderer depending on type. */
          Renderer* add;

          switch (type) {
          case DEFAULT:
            add = new Renderer(obliquefile, target);
            break;
          case CONTOUR:
            add = new Render_Contour("", target);
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
  : options(options), filename(filename), image(0), finalised(false) {
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
}

/* Let the renderer know the coordinates of the map corners. */
void Renderer::set_surface(const Level::position& top_right_chunk,
                           const Level::position& bottom_left_chunk) {
  top_right = { top_right_chunk.second, top_right_chunk.first, 0 };
  bottom_left = { bottom_left_chunk.second, bottom_left_chunk.first, 0 };

  /* Make sure we only do this once. */
  if (image)
    throw std::runtime_error("Attempted to allocate image memory twice.");

  if (!options.oblique.first) {
    /* Allocate memory for a flat unrotated map. We may rotate
       it when rendering is finished. */
    pvector adjust = {1, 1};
    pvector size = ((bottom_left - top_right) + adjust) * 16;
    image = new Image({size.z, size.x});
  } else {
    /* TODO: Calculate size of oblique map. */
  }

  /* Allocate for all overlays too. */
  for (RenderList::iterator overlay = overlays.begin();
       overlay != overlays.end(); ++overlay) {
    (*overlay)->set_surface(top_right_chunk, bottom_left_chunk);
  }
}

/* Pass a chunk to the renderer and let it do its thing. */
void Renderer::render(const chunkbox& chunks) {
  if (!options.oblique.first) {
    /* Flat map. Render it unrotated. We may rotate it when
       all chunks are rendered. */
    for (int x = 0; x < 16; x++) {
      for (int z = 0; z < 16; z++) {
        Pixel dot;
        for (int y = 127; y >= 0; y--) {
          Pixel under = getblock(chunks, {x, z, y}, TOP);
          if (under.A > 0) {
            under.light(getlight(chunks, {x, z, y}, TOP));
            /* Adjust lighting slightly depending on height. */
            if (options.dimdepth.first) {
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
        int img_x = (bottom_left.z - chunks.center->get_position().z) * 16
                    + (15 - z);
        int img_y = (chunks.center->get_position().x - top_right.x) * 16 + x;
        (*image)(img_x, img_y) = dot;
      }
    }
  } else {
    /* Oblique map. */
    /* TODO */
  }

  /* Render all overlays too. */
  for (RenderList::iterator overlay = overlays.begin();
       overlay != overlays.end(); ++overlay) {
    (*overlay)->render(chunks);
  }
}

/* Make any last minute adjustments. */
void Renderer::finalise() {
  if (options.oblique.first) {
    /* TODO: Finalise oblique images. */
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
  finalise();
  if (image) {
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
    Image rotate(*image, angle);
    rotate.output(filename, angle % 2); // Trim on diagonals.
  } else {
    throw std::logic_error("Attempted to save unrendered image.");
  }
}

/* Get colour value of a block. */
Pixel Renderer::getblock(const chunkbox& chunks, pvector pos,
                         direction dir) {
  /* Get a proper target. */
  Chunk* target = fixpvector(chunks, pos);

  /* Fetch block from target. */
  unsigned char type = target->blocks(pos);
  Pixel result = (dir & TOP) ? colours[type].top : colours[type].side;

  if ((type == 0x08 || type == 0x09) && (dir & TOP)) {
    /* Block is water. Set top alpha based on depth. */
    try {
      unsigned char invdepth = target->data(pos);
      if (invdepth > 0) {
        result.A = 0xff - invdepth * 0x0f;
      }
    } catch (std::runtime_error& e) { /* Data was not loaded. */ }
  }

  return result;
}

/* Get lighting level of a block. */
unsigned char Renderer::getlight(const chunkbox& chunks, pvector pos,
                                 direction dir) {
  if (dir & TOP) {
    pos.y++;
  } else if (dir & N) {
    pos.x--;
  } else if (dir & E) {
    pos.z--;
  } else if (dir & S) {
    pos.x++;
  } else if (dir & W) {
    pos.z++;
  }

  unsigned char l_sky = 0;
  unsigned char l_block = 0;

  if (pos.y > 127 || pos.y < 0) {
    /* There is no lighting data above or below the map. */
    l_sky = 255; // Fully lit by sky.
    l_block = 0; // Not lit by other sources.

  } else {
    /* Get a proper target. */
    Chunk* target = fixpvector(chunks, pos);

    try {
      l_sky = target->skylight(pos) * 17;
    } catch (std::runtime_error& e) {
      /* Skylight data not loaded. */
      l_sky = 0;
    }
    try {
      l_block = target->blocklight(pos) * 17;
    } catch (std::runtime_error& e) {
      /* Blocklight data not loaded. */
      l_block = 0;
    }
  }

  /* Balance lighting. */
  return ((l_sky * options.lightlevel.first) / 255) +
    ((l_block * (255 - options.lightlevel.first)) / 255);
}

/* Convert a chunkbox-pvector combo to a chunk-pvector combo. The pvector
   passed in may point outside the center chunk. */
Chunk* Renderer::fixpvector(const chunkbox& chunks, pvector& pos) {
  Chunk* target = chunks.center;

  /* Make sure height is valid. */
  if (pos.y < 0 || pos.y > 127) {
    throw std::logic_error("Trying to read data outside of valid height.");
  }

  /* Check if block is in a neighbouring chunk. */
  if (pos.z > 15) {
    target = chunks.west;
    pos.z -= 16;
  } else if (pos.z < 0) {
    target = chunks.east;
    pos.z += 16;
  } else if (pos.x > 15) {
    target = chunks.south;
    pos.x -= 16;
  } else if (pos.x < 0) {
    target = chunks.north;
    pos.x += 16;
  }

  /* If target is unset or position is invalid,
     target is not in the chunkbox. */
  if (!target || pos.x > 15 || pos.x < 0 || pos.z > 15 || pos.z < 0) {
    throw std::range_error("Attempting to read data from nonexisting chunk.");
  }

  return target;
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
