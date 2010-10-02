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

using std::cout;
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

  /* Verbosity flag. */
  bool verbose = false;

  /* Strings to hold options for the renderers. */
  list<string> renderstrs;

  /* Global options. */
  list<longopt> options;

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
      verbose = true;
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
  Level level(worldpath);

  /* Render to memory. */
  level.render(renderers);

  /* Output the result. */
  for (std::list<Renderer*>::iterator it = renderers.begin();
       it != renderers.end(); ++it) {
    (*it)->save();
  }

  /* Finished. */
  return 0;
}
