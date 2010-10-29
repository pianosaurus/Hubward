#ifndef H_IMAGE
#define H_IMAGE

#include <string>

class Pixel;

/*
 * Stores and handles pixel arrays.
 */
class Image {
public:
  struct ivector {
    int x;
    int y;

    ivector() : x(0), y(0) {};
    ivector(int x, int y) : x(x), y(y) {};
    ivector transpose() { return ivector(y, x); };
  };

  /* Allocate and initialise memory. */
  Image(ivector dimensions, const Pixel* fill = 0);

  /* Copy another image. Integer rotate is 0 to 7 clockwise. */
  Image(const Image& source, int rotate = 0);
  Image& operator=(const Image& source);

  /* Clean up. */
  virtual ~Image();

  /* Alpha blend another image on top of this one. */
  void overlay(const Image& source);

  /* Write results to file. */
  void output(std::string filename, bool trim = true) const;

  /* Get image dimensions. */
  ivector dimensions() const { return size; };

  /* Colour replace. */
  void colour_replace(const Pixel& from, const Pixel& to);

  /* Return a pixel. */
  Pixel& operator()(int x, int y);
  Pixel  operator()(int x, int y) const;

private:
  /* Image dimensions. */
  ivector size;

  /* Raw image data. */
  Pixel* data;
};

#endif
