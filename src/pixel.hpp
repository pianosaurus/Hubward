#ifndef H_PIXEL
#define H_PIXEL

#include <ostream>

/*
 * A simple structure for colour data, with blending methods.
 */
class Pixel {
public:
  double R;
  double G;
  double B;
  double A;

  struct charval {
    unsigned char R;
    unsigned char G;
    unsigned char B;
    unsigned char A;
  };

  /* Initialise to transparent. */
  Pixel() : R(0xff), G(0xff), B(0xff), A(0x00) {};
  /* Or some other value. */
  Pixel(double R, double G,
        double B, double A) : R(R), G(G), B(B), A(A) {};
  Pixel(unsigned char R, unsigned char G,
        unsigned char B, unsigned char A) : R(R / 255.0f), G(G / 255.0f),
                                            B(B / 255.0f), A(A / 255.0f) {};

  /* Compare with another pixel. */
  bool operator==(const Pixel& o);

  /* Blend another pixel underneath self. */
  void blend_under(const Pixel& source);

  /* Blend another pixel over self. */
  void blend_over(const Pixel& source);

  /* Light pixel. */
  void light(double value);

  /* Mix in another pixel 50/50.*/
  void mix(const Pixel& source);

  /* Convert to low precision chars. */
  charval chars();
};

std::ostream& operator<<(std::ostream& o, const Pixel& p);

#endif
