#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <cstdlib>

#include <stdexcept>

#include "version.h"

#include "level.hpp"
#include "renderer.hpp"
#include "render_contour.hpp"
#include "image.hpp"
#include "intstring.hpp"
#include "options.hpp"
#include "output.hpp"

using std::cerr;
using std::string;
using std::list;

/*
 * Main function. Interpret arguments and get started.
 */
int main(int argc, char** argv) {
  /* Print usage on empty argument list. */
  if (argc == 1) {
    usage(argv[0]);
    return 0;
  }

  /* A variable to hold the path to the world. */
  string worldpath;

  /* Strings to hold options for the renderers. */
  list<string> renderstrs;

  /* Global options. */
  list<longopt> options;

  /* Chunk intersect list. */
  list<Level::position> chunks;

  /* Get options and their arguments. */
  try {
    parse_options(argc, argv, renderstrs, options);
  } catch (std::exception& e) {
    cerr << e.what() << "\n";
    return 1;
  }

  /* Handle the options. */
  for (list<longopt>::const_iterator opt = options.begin();
       opt != options.end(); ++opt) {
    if (opt->first == "number"){
      /* Set the world path by world number. */

      /* Make sure it's not already set. */
      if (!worldpath.empty()) {
        cerr << "Cannot render two maps at once. Check your arguments.\n";
        return 1;
      }

      /* Make sure argument is valid. */
      try {
        int worldno = stringtoint(opt->second);
      } catch (std::runtime_error& e) {
        cerr << "Invalid world number: " << opt->second << "\n";
        return 1;
      }

      /* Get path. */
#ifdef __unix__
      worldpath = getenv("HOME");
      worldpath += "/";
#else
#warning "TODO:Specifying world number only works on *NIX platforms (for now)."
#endif
      worldpath += ".minecraft/saves/World";
      worldpath += opt->second;

    } else if (opt->first == "path") {
      /* Set the world path directly. */

      /* Make sure it's not already set. */
      if (!worldpath.empty()) {
        cerr << "Cannot render two maps at once. Check your arguments.\n";
        return 1;
      }

      worldpath = opt->second;

    } else if (opt->first == "version") {
      /* Print version information and exit. */
      cerr << APPNAME << " " << APPVERSION << "\n";
      return 0;

    } else if (opt->first == "verbose") {
      set_verbose(true);

    } else if (opt->first == "debug") {
      set_debug(true);
      set_verbose(true); // Debug implies verbose.

    } else if (opt->first == "chunks") {
      string error = "Invalid dimensions. Use WxH or WxH+Z+X, where\n"
                     "  W is east-west count\n  H is north-south count\n"
                     "  Z is west offset\n  X is south offset\n"
                     "You may also use - instead of + for negative Z and X. If"
                     " you leave out the\noffset, center will be at 0,0.";
      int width, height, west, south;

      size_t dX = opt->second.find_first_of("xX");
      if (dX == string::npos) {
        cerr << error << std::endl;
        return 1;
      }
      try {
        width = stringtoint(opt->second.substr(0, dX));
      } catch (std::runtime_error& e) {
        cerr << "Invalid width: " << e.what() << std::endl;
        return 1;
      }

      size_t do1 = opt->second.find_first_of("+-");
      if (do1 == string::npos) {
        try {
          height = stringtoint(opt->second.substr(dX + 1));
        } catch (std::runtime_error& e) {
          cerr << "Invalid height: " << e.what() << std::endl;
          return 1;
        }
        west = -width/2;
        south = -height/2;
      } else {
        try {
          height = stringtoint(opt->second.substr(dX + 1, do1 - dX - 1));
        } catch (std::runtime_error& e) {
          cerr << "Invalid height: " << e.what() << std::endl;
          return 1;
        }
        size_t do2 = opt->second.find_first_of("+-", do1 + 1);
        try {
          if (do2 == string::npos) {
            west = stringtoint(opt->second.substr(do1));
            south = -height/2;
          } else {
            west = stringtoint(opt->second.substr(do1, do2 - do1));
            south = stringtoint(opt->second.substr(do2));
          }
        } catch (std::runtime_error& e) {
          cerr << "Invalid offset: " << e.what() << std::endl;
          return 1;
        }
      }

      /* Add to intersect list. */
      for (int xx = 0; xx < height; xx++) {
        for (int zz = 0; zz < width; zz++) {
          chunks.push_back({xx + south, zz + west});
        }
      }
    }
  }


  /* Make sure a world was chosen. */
  if (worldpath.empty()) {
    if (argc > 1) cerr << "Error: No world specified.\n";

    usage(argv[0]);
    return 1;;
  }

  /* Generate the requested renderers. */
  Renderer::RenderList renderers;
  for (list<string>::iterator str = renderstrs.begin();
       str != renderstrs.end(); ++ str) {
    renderers.splice(renderers.begin(), Renderer::make_renderers(*str));
  }

  /* Make sure there is at least one renderer.*/
  if (renderers.size() == 0) {
    cerr << "No outputs specified.\n";
    return 1;
  }

  /* Prepare the level (create chunk map). */
  verbose << "Finding files in " << worldpath << std::endl;
  Level* level;
  if (chunks.empty()) {
    level = new Level(worldpath);
  } else {
    level = new Level(worldpath, chunks);
  }

  /* Render to memory. */
  verbose << "Rendering..." << std::endl;
  level->render(renderers);

  /* Output the result. */
  verbose << "Finished reading. Writing images to disk." << std::endl;
  Renderer::RenderList::iterator renderer = renderers.begin();
  while (!renderers.empty()) {
    try {
      (*renderer)->save();
    } catch (std::exception& e) {
      std::cerr << "Failed to save rendering: " << e.what() << std::endl;
    }

    /* Delete the renderer, just to be nice. */
    delete *renderer;
    renderer = renderers.erase(renderer);
  }

  /* Finished. */
  delete level;
  std::cerr << "Done." << std::endl;
  return 0;
}
