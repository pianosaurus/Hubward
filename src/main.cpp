#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>

#include <stdexcept>

#include "level.hpp"
#include "renderer.hpp"
#include "image.hpp"

using std::cout;
using std::cerr;
using std::string;

/*
 * Output usage help.
 */
void usage(const char* binary) {
  cerr << "Usage:\t" << binary
       << " [options] <world path | world number>\n";
  /*TODO
       << "Options:\n"

       << "  -r <n> | --rotate=<n>\n"
       << "\tRotate the map n*45 degrees in a clockwise direction.\n";

       << "  -o | --oblique\n"
       << "\tRender the map using an oblique projection.\n";
  */
}

/*
 * Convert from string to another data type.
 */
int stringtoint(const string& s)
{
  int i = 0;
  std::istringstream stream(s);
  if ((stream >> i).fail() || !stream.eof())
    throw std::runtime_error("Not a number.");

  return i;
}

/*
 * Main function. Interpret arguments and get started.
 */
int main(int argc, char** argv) {
  /* Initialise a variable to hold the path to the world. */
  string worldpath;

  /* Interpret arguments. */
  for (int i = 1; i < argc; i++) {
    /* Choose what to do depending on first character. */
    switch (argv[i][0]) {
    case 0:   // Empty argument.
      continue;

    case '-': // An option.
      //TODO
      break;

    default: // World path or number.
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

  /* Load the level. */
  std::list<Level::chunkhead> tmp;
  for (int i = -200; i < 200; i++) {
    tmp.push_back(Level::chunkhead());
    std::list<Level::chunkhead>::reverse_iterator it = tmp.rbegin();
    (*it).pos.x = i / 20;
    (*it).pos.z = i % 20;
  }

  //Level level(worldpath, tmp);
  Level level(worldpath);

  /* Render the level to memory. */
  std::list<Renderer*> renderers;
  renderers.push_back(new Renderer());
  renderers.push_back(new Renderer(false, Renderer::NE, 45));
  renderers.push_back(new Renderer(false, Renderer::SE, 127));
  renderers.push_back(new Renderer(false, Renderer::SW, 255));
  renderers.push_back(new Renderer(false, Renderer::NW, 190));
  level.render(renderers);

  /* Output the result. */
  char i[] = {0x2f, 0};
  for (std::list<Renderer*>::iterator it = renderers.begin();
       it != renderers.end(); ++it) {
    i[0]++;
    (*it)->save(std::string("output") + i + ".png");
  }

  /* Finished. */
  return 0;
}
