#include "intstring.hpp"

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
