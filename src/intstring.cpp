#include "intstring.hpp"

#include <stdexcept>
#include <sstream>

/*
 * Convert from string to integer.
 */
int stringtoint(const std::string& s) {
  int i = 0;
  std::istringstream stream(s);
  if ((stream >> i).fail() || !stream.eof())
    throw std::runtime_error("Not a number.");

  return i;
}
