#include "pvector.hpp"

#include <stdexcept>

/* Vector addition. */
pvector pvector::operator+(const pvector& v) const {
  pvector result = *this;
  result.x += v.x;
  result.y += v.y;
  result.z += v.z;

  return result;
}

/* Vector subtraction. */
pvector pvector::operator-(const pvector& v) const {
  pvector result = *this;
  result.x -= v.x;
  result.y -= v.y;
  result.z -= v.z;

  return result;
}

/* Integer multiplication. */
pvector pvector::operator*(int i) const {
  return pvector(x*i, z*i, y*i);
}

/* Convert vector to position in an NBT array. */
int pvector::nbt() const {
  if (x < 0 || x > 16)
    throw std::out_of_range("X out of bounds for NBT section.");
  if (z < 0 || z > 16)
    throw std::out_of_range("Z out of bounds for NBT section.");
  if (y < 0 || y > 127)
    throw std::out_of_range("Y out of bounds for NBT section.");

  return y + (z * 128) + (x * 128 * 16);
}

/* Convert vector to position in an NBT array. Count only 4 bits for
   each block, and set upper to true if the upper four bits should be
   used. */
int pvector::nbt(bool& upper) const {
  int pos = nbt();

  upper = pos & 1;
  pos /= 2;

  return pos;
}

/* Stream output. */
std::ostream& operator<<(std::ostream& o, const pvector& p) {
  o << p.x << "." << p.z << "." << p.y;
  return o;
}
