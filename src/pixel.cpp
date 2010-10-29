#include "pixel.hpp"

/* Compare with another pixel. */
bool Pixel::operator==(const Pixel& o) {
  static const double prec = 0.004;
  return (R > o.R - prec) && (R < o.R + prec) &&
         (G > o.G - prec) && (G < o.G + prec) &&
         (B > o.B - prec) && (B < o.B + prec) &&
         (A > o.A - prec) && (A < o.A + prec);
}

/* Alpha blend another pixel underneath self. */
void Pixel::blend_under(const Pixel& source) {
  double nA = A + ((1.0f - A)*source.A);
  A = (nA == 0) ? 0 : A / nA;
  double sA = 1.0f - A;
  R = (R * A + source.R * sA);
  G = (G * A + source.G * sA);
  B = (B * A + source.B * sA);
  A = nA;
}

/* Alpha blend another pixel over self. */
void Pixel::blend_over(const Pixel& source) {
  double nA = source.A + ((1.0f - source.A)*A);
  A = (nA == 0) ? 0 : source.A / nA;
  double tA = 1.0f - A;
  R = (R * tA + source.R * A);
  G = (G * tA + source.G * A);
  B = (B * tA + source.B * A);
  A = nA;
}

/* Light pixel. */
void Pixel::light(double value) {
  R = (R * value);
  G = (G * value);
  B = (B * value);
}

/* Mix in another pixel 50/50.*/
void Pixel::mix(const Pixel& source) {
  R = (R / 2) + (source.R / 2);
  G = (G / 2) + (source.G / 2);
  B = (B / 2) + (source.B / 2);
  A = (A / 2) + (source.A / 2);
}

/* Convert to low precision chars. */
Pixel::charval Pixel::chars() {
  Pixel::charval result;
  result.R = int(R*255);
  result.G = int(G*255);
  result.B = int(B*255);
  result.A = int(A*255);
  return result;
}

/* Stream output. */
std::ostream& operator<<(std::ostream& o, const Pixel& p) {
  o << int(p.R * 255) << "." << int(p.G * 255) << "."
    << int(p.B * 255) << "." << int(p.A * 255);
  return o;
}
