#ifndef H_VECTOR
#define H_VECTOR

#include <ostream>

/*
 * A tiny 3D vector structure with some simple operators.
 * It uses the same order the NBT files do (X, Z, Y).
 */
struct pvector {
  int x;
  int z;
  int y;

  pvector() : x(0), z(0), y(0) {};
  pvector(int x, int z) : x(x), z(z), y(0) {};
  pvector(int x, int z, int y) : x(x), z(z), y(y) {};

  /* Transpose vector. */
  pvector transpose_xz() const { return {z, x, y}; };
  pvector transpose_zy() const { return {x, y, z}; };
  pvector transpose_xy() const { return {y, z, x}; };

  /* Some operations. */
  pvector operator+(const pvector& v) const;
  pvector operator-(const pvector& v) const;
  pvector operator*(int i) const;

  /* Convert vector to position in an NBT array. If a boolean is
     given, count only 4 bits for each block, and set it to true if
     the upper four bits should be used. */
  int nbt() const;
  int nbt(bool& upper) const;
};

std::ostream& operator<<(std::ostream& o, const pvector& p);

#endif
