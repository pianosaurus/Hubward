#include "../config.h"
#include "intstring.hpp"

#ifdef HAVE_WINDOWS_H
  #include <windows.h>
#elif HAVE_CSTDLIB
  #include <cstdlib>
#endif

#include <stdexcept>
#include <sstream>

/*
 * Convert from string to integer.
 */
int stringtoint(const std::string& s) {
  int sign = 0;
  if (s[0] == '+') {
    sign = 1;
  } else if (s[0] == '-') {
    sign = -1;
  }

  int i = 0;
  std::stringstream stream;
  if (sign != 0) {
    stream << s.substr(1);
  } else {
    stream << s;
  }
  if ((stream >> i).fail() || !stream.eof())
    throw std::runtime_error("Not a number.");

  return (sign != 0) ? i * sign : i;
}

/* Fetch the Minecraft save directory with a trailing "/". */
std::string savepath() {
  std::string result;

#ifdef HAVE_WINDOWS_H
  /* Windows environment. */
  /* TODO: Does this work on non-ansi paths? It would be nice if
     someone with special characters in their user name could test it. */
  char path[MAX_PATH];
  DWORD environment = GetEnvironmentVariableA("APPDATA", path, MAX_PATH);

  if (!environment) {
    throw std::runtime_error("Couldn't fetch %appdata% from environment. "
                             "Try specifying path to save instead.");
  }
  result += path;



#elif HAVE_CSTDLIB
  /* *NIX environment.  */
  char* environment = getenv("HOME");
  if (!environment) {
    throw std::runtime_error("Couldn't fetch $HOME from environment. "
                             "Try specifying path to save instead.");
  }
  result = environment;


#else
  /* Unknown environment. */
  #warning "Cannot fetch save directory. The --number (-n) option will fail."
#endif


  /* Append minecraft-specific string. */
  result += "/.minecraft/saves/";
  return result;
}
