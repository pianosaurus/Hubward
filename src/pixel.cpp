#include "pixel.hpp"

/* Alpha blend another pixel underneath self. */
void Pixel::blend_under(const Pixel& source) {
  unsigned char nA = A + ((255-A)*source.A)/255;
  A = (nA == 0) ? 0 : A * 255 / nA;
  unsigned char sA = 255 - A;
  R = (R * A + source.R * sA) / 255;
  G = (G * A + source.G * sA) / 255;
  B = (B * A + source.B * sA) / 255;
  A = nA;
}

/* Alpha blend another pixel over self. */
void Pixel::blend_over(const Pixel& source) {
  unsigned char nA = source.A + ((255-source.A)*A)/255;
  A = (nA == 0) ? 0 : source.A * 255 / nA;
  unsigned char tA = 255 - A;
  R = (R * tA + source.R * A) / 255;
  G = (G * tA + source.G * A) / 255;
  B = (B * tA + source.B * A) / 255;
  A = nA;
}

/* Shade pixel. 0 = no change, +/- 127 = white/black.. */
void Pixel::shade(signed char value) {
  if (value >= 0) {
    R += (127 - R) * value / 127;
    G += (127 - G) * value / 127;
    B += (127 - B) * value / 127;
  } else {
    R += R * value / 127;
    G += G * value / 127;
    B += B * value / 127;
  }
}

/* Light pixel. */
void Pixel::light(unsigned char value) {
  R = (R * value) / 255;
  G = (G * value) / 255;
  B = (B * value) / 255;
}

/* Mix in another pixel 50/50.*/
void Pixel::mix(const Pixel& source) {
  R = (R / 2) + (source.R / 2);
  G = (G / 2) + (source.G / 2);
  B = (B / 2) + (source.B / 2);
  A = (A / 2) + (source.A / 2);
}
