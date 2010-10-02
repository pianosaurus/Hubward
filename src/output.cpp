#include "output.hpp"

std::ostream dummy(0);
std::ostream& debug = *(new std::ostream(0));
std::ostream& verbose = *(new std::ostream(0));

void set_debug(bool enable) {
  if (enable) {
    debug.rdbuf(std::cerr.rdbuf());
  } else {
    debug.rdbuf(dummy.rdbuf());
  }
}

void set_verbose(bool enable) {
  if (enable) {
    verbose.rdbuf(std::cerr.rdbuf());
  } else {
    verbose.rdbuf(dummy.rdbuf());
  }
}
