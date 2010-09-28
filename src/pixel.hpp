#ifndef H_PIXEL
#define H_PIXEL

/*
 * A simple structure for colour data, with blending methods.
 */
class Pixel {
public:
  unsigned char R;
  unsigned char G;
  unsigned char B;
  unsigned char A;

  /* Initialise to transparent. */
  Pixel() : R(0xff), G(0xff), B(0xff), A(0x00) {};
  /* Or some other value. */
  Pixel(unsigned char sR, unsigned char sG,
        unsigned char sB, unsigned char sA) : R(sR), G(sG), B(sB), A(sA) {};

  /* Blend another pixel underneath self. */
  void blend_under(const Pixel& source);

  /* Shade pixel. 0 = no change, +/- 127 = white/black.. */
  void shade(signed char value);

  /* Light pixel. */
  void light(unsigned char value);

  /* Mix in another pixel 50/50.*/
  void mix(const Pixel& source);
};

#endif
