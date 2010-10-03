#include "options.hpp"

#include <iostream>
#include <stdexcept>

using std::cerr;
using std::string;
using std::list;

/* Short options and their long equivalents. */
const struct option {
  char shortopt;
  string longopt;
  bool argument;
  string argname;
  string description;
} valid_options[] = {
  { 0, "debug", false, "", "Enable debugging output."},
  { 'c', "chunks", true, "dimensions", "Only render the chunks specified by "
                                       "dimensions (WxH or WxH+Z+X)." },
  { 'n', "number", true, "n", "The world number to render. This or -p must "
                              "be specified."},
  { 'p', "path", true, "path", "The path of the world to render. This or -n "
                               "must be specified."},
  { 'v', "verbose", false, "", "Print more status information." },
  { 0, "version", false, "", "Print the version of this release and exit." }
};

/*
 * Output usage help.
 */
void usage(const char* binary) {
  cerr << "Usage:\t" << binary << " <options> <renderspec1> [renderspec2 [...]]\n\n"
       << "Options:\n";

  /* Print all valid options. */
  for (int v = 0; v < sizeof(valid_options)/sizeof(option); v++) {
    /* Short version. */
    if (valid_options[v].shortopt) {
      cerr << "  -" << valid_options[v].shortopt;
      if (valid_options[v].argument)
        cerr << "<" << valid_options[v].argname << ">";
      cerr << "\n";
    }

    /* Long version. */
    cerr << "  --" << valid_options[v].longopt;
    if (valid_options[v].argument)
      cerr << "=<" << valid_options[v].argname << ">";

    /* Description. */
    cerr << "\n\t" << valid_options[v].description << "\n";
  }

  /* Describe the renderspec. */
  cerr << "\nRenderspecs:\n"
       << "\t<filename>[:keywords[:overlay1[:overlay2[...]]]\n\n";
  cerr << "  Rotation keywords [%r]: (defaults to <north>)\n"
       << "\t<n, nw, w, sw, s, se, e, ne> or the equivalent <north,\n"
       << "\tnorthwest, west, southwest, south, southeast, east, northeast>.\n"
       << "\tYou may also use <cardinal, ordinal>; see Multiples below.\n";
  cerr << "  Lighting keywords [%l]: (defaults to <twilight>)\n"
       << "\t<day, night, twilight, lightNNN>, where NNN is a number\n"
       << "\tbetween 0 and 255 inclusive.\n";
  cerr << "  Depth dimming keywords [%d]: (defaults to <dimdepth>)\n"
       << "\t<dimdepth, litdepth>. Use dimdepth to adjust brightness of\n"
       << "\tthe surface depending on height. This will enable you to see\n"
       << "\theight features on top-down maps without contour lines.\n";
  cerr << "  Special keywords: (defaults to nothing)\n"
       << "\t<contour> draws contour lines on a transparent background.\n";
  cerr << "\n  Overlays can be added with all the same keywords except\n"
       << "  rotation. The specials make good overlays, as they have\n"
       << "  transparent backgrounds.\n";

  /* Describe multiples. */
  cerr << "\nMultiples:\n"
       << "  You may specify multiple keywords in most of the keyword groups\n"
       << "  above to render several maps at once. You must also add the\n"
       << "  %-marker in the square brackets of that group to the file name.\n"
       << "  This marker will be replaced with the keyword being rendered.\n";

  /* This is complex, so give some examples. */
  cerr << "\nExamples:\n";
  cerr << "  Render a default map:\n"
       << "\t" << binary << " -n2 map.png\n";
  cerr << "  Render three maps with various lighting conditions:\n"
       << "\t" << binary << " -n2 map-%l.png:day,night,twilight\n";
  cerr << "  Render a map facing east with contour lines:\n"
       << "\t" << binary << " -n2 map.png:east:contour\n";
}

/*
 * Fill the renderstrs and options lists with strings pased on
 * parsing the incoming arguments (except the first).
 */
void parse_options(int argc, char** argv,
                   list<string>& renderstrs, list<longopt>& options) {
  /* Interpret arguments. */
  for (int i = 1; i < argc; i++) {
    /* Choose what to do depending on first character. */
    switch (argv[i][0]) {
    case 0:   // Empty argument.
      continue;

    case '-': {// An option.
      string newopt = argv[i];
      if (newopt.length() > 1 && newopt[1] != '-') {
        /* Short options. */
        for (int opt = 1; opt < newopt.length(); opt++) {
          bool done = false;
          bool found = false;
          /* Find option in list of valid ones. */
          for (int v = 0; v < sizeof(valid_options)/sizeof(option); v++) {
            if (valid_options[v].shortopt == newopt[opt]) {
              /* Get argument if required. */
              string argument;
              if (valid_options[v].argument) {
                if (opt < newopt.length() - 1) {
                  /* The rest of this argv is the argument. */
                  argument = newopt.substr(opt + 1);
                } else {
                  /* The next argv is the argument. */
                  if (i < argc - 1) {
                    argument = argv[i + 1];
                    i++;
                  } else {
                    /* Missing argument. */
                    throw std::runtime_error(string("Option -") + newopt[opt]
                                             + " requires an argument.");
                  }
                }

                /* If we got an argument, we are done with this argv. */
                done = true;
              }

              options.push_back(longopt(valid_options[v].longopt, argument));
              found = true;
              break;
            }
          }
          if (!found) {
            throw std::runtime_error(string("Unknown option -") + newopt[opt]);
          }
          if (done) {
            break;
          }
        }

      } else if (newopt.length() > 1 && newopt[1] == '-') {
        /* Long option. */
        string argument;
        size_t argstart = newopt.find('=');
        if (argstart != string::npos) {
          if (argstart != string::npos && argstart < newopt.length() - 1) {
            argument = newopt.substr(argstart + 1);
          }
          newopt = newopt.substr(2, argstart - 2);
        } else {
          newopt = newopt.substr(2);
        }
        bool found = false;
        for (int v = 0; v < sizeof(valid_options)/sizeof(option); v++) {
          if (valid_options[v].longopt == newopt) {
            /* Get the argument if required. */
            if (valid_options[v].argument && argument.empty()) {
              throw std::runtime_error(string("Option --") + newopt
                                       + " requires an argument.");
            } else if (!valid_options[v].argument && !argument.empty()) {
              throw std::runtime_error(string("Option --") + newopt
                                       + " does not take an argument.");
            }

            options.push_back(longopt(newopt, argument));

            found = true;
            break;
          }
        }
        if (!found) {
          throw std::runtime_error(string("Unknown option --") + newopt);
        }
      }
    } break;

    default:  // Renderer options.
      renderstrs.push_back(argv[i]);
    }
  }
}
