#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <stdexcept>

#include "level.hpp"
#include "renderer.hpp"
#include "image.hpp"
#include "intstring.hpp"

using std::cout;
using std::cerr;
using std::string;

/*
 * Output usage help.
 */
void usage(const char* binary) {
  cerr << "Usage:\t" << binary << " <world> [options]\n\n"
       << "Options:\n";

  for (int i = 0; i < (sizeof(Renderer::options) / sizeof(Renderer::option));
       i++) {
    const Renderer::option o = Renderer::options[i];
    const string arg = (o.argname.length()) ? "<" + o.argname + ">" : "";
    cerr << " -" << o.shortopt << arg << " | --" << o.longopt;
    if (arg.length())
      cerr << "=" << arg;
    if (o.defaultval.length())
      cerr << "  (default " << o.defaultval << ")";
    cerr << "\n" << o.description << "\n\n";
  }

  cerr << "Multiple outputs:\n"
       << " By separating groups of options using --, you may render several\n"
       << "maps in one run. You may only render one world per run.\n";
}

/*
 * Main function. Interpret arguments and get started.
 */
int main(int argc, char** argv) {
  /* Initialise a variable to hold the path to the world. */
  string worldpath;

  /* A string to hold options for the renderers. */
  string options;

  /* Interpret arguments. */
  for (int i = 1; i < argc; i++) {
    /* Choose what to do depending on first character. */
    switch (argv[i][0]) {
    case 0:   // Empty argument.
      continue;

    case '-': // An option.
      options += string(" ") + argv[i];
      break;

    default:  // World path or number.
      if (!worldpath.empty()) {
        /* World path is already set. */
        cerr << "Error: Can't render two worlds at once. "
                "Please check your syntax.\n";
        return 1;
      }

      /* Convert number to path if necessary. */
      try {
        int worldno = stringtoint(argv[i]);

        /* Argument is a number. */
#ifdef __unix__
        worldpath = getenv("HOME");
        worldpath += "/";
#else
#warning "TODO:Specifying world number only works on *NIX platforms (for now)."
#endif
        worldpath += ".minecraft/saves/World";
        worldpath += argv[i];
      } catch (std::runtime_error& e) {
        /* Argument is a path. */
        worldpath = argv[i];
      }
    }
  }


  /* Make sure a world was chosen. */
  if (worldpath.empty()) {
    if (argc > 1) cerr << "Error: No world specified.\n";

    usage(argv[0]);
    if (argc > 1) return 1;;

    return 0;
  }

  /* Prepare the level (create chunk map). */
  Level level(worldpath);

  /* Render the level to memory. */
  std::list<Renderer*> renderers;
  while (!options.empty()) {
    renderers.push_back(new Renderer(options));
  }
  level.render(renderers);

  /* Output the result. */
  for (std::list<Renderer*>::iterator it = renderers.begin();
       it != renderers.end(); ++it) {
    (*it)->save();
  }

  /* Finished. */
  return 0;
}
