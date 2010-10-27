#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <stdexcept>

#include "../config.h"

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
      try {
        worldpath = savepath() + "World" + opt->second;
      } catch (std::runtime_error& e) {
        cerr << e.what() << std::endl;
        return 1;
      }

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
      cerr << PACKAGE_STRING << "\n";
      return 0;

    } else if (opt->first == "verbose") {
      set_verbose(true);

    } else if (opt->first == "debug") {
      set_debug(true);
      set_verbose(true); // Debug implies verbose.

    } else if (opt->first == "chunks") {
      /* Fill chunk intersection list. */
      try {
        chunks = Level::chunk_list(opt->second);
      } catch (std::exception& e) {
        cerr << e.what() << std::endl;
        return 1;
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
    Renderer::RenderList tmp_list = Renderer::make_renderers(*str);
    renderers.splice(renderers.begin(), tmp_list);
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
  try {
    level->render(renderers);
  } catch (std::exception& e) {
    cerr << "Rendering failed: " << e.what() << std::endl;
    return 1;
  }

  /* Output the result. */
  bool all_ok = true;
  verbose << "Finished reading. Writing images to disk." << std::endl;
  Renderer::RenderList::iterator renderer = renderers.begin();
  while (!renderers.empty()) {
    try {
      (*renderer)->save();
    } catch (std::exception& e) {
      std::cerr << "Failed to save rendering: " << e.what() << std::endl;
      all_ok = false;
    }

    /* Delete the renderer, just to be nice. */
    delete *renderer;
    renderer = renderers.erase(renderer);
  }

  /* Finished. */
  delete level;
  std::cerr << "Done." << std::endl;
  if (all_ok)
    return 0;
  else
    return 1;
}
