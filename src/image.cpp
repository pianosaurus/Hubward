#include "image.hpp"
#include "pixel.hpp"

#include <algorithm>
#include <stdexcept>
#include <png.h>

/* Allocate and initialise memory. */
Image::Image(ivector dimensions, const Pixel* fill) :
  size(dimensions) {

  data = new Pixel[size.x * size.y];

  /* Fill the allocated memory with something reasonable. */
  bool gotfill = true;
  if (!fill) {
    gotfill = false;
    fill = new Pixel(0xff, 0xff, 0xff, 0x00);
  }
  for (int i = 0; i < size.x * size.y; i++) {
    data[i] = *fill;
  }
  if (!gotfill)
    delete fill;
}

/* Copy another image. Integer rotate is 0 to 7 clockwise. */
Image::Image(const Image& source, int rotate) {
  rotate %= 8;
  int corner;
  if (rotate == 0 || rotate == 4) {
    /* Straight or upside down. */
    size = source.dimensions();
  } else if (rotate == 2 || rotate == 6) {
    /* Rotated sideways. */
    size = source.dimensions().transpose();
  } else {
    /* Rotated on a diagonal. */
    size.x = size.y = (source.dimensions().x + source.dimensions().y + 1) / 2;
    corner = source.dimensions().y / 2;
  }

  data = new Pixel[size.x * size.y];
  for (int y = 0; y < size.y; y++) {
    for (int x = 0; x < size.x; x++) {
      switch (rotate) {
      case 0:
        /* Straight copy. */
        (*this)(x,y) = source(x,y);
        break;

      case 4:
        /* Upside down. */
        (*this)(x,y) = source(size.x - x - 1, size.y - y - 1);
        break;

      case 2:
        /* Rotated 90 degrees clockwise. */
        (*this)(x,y) = source(y, size.x - x - 1);
        break;

      case 6:
        /* Rotated 90 degrees counterclockwise. */
        (*this)(x,y) = source(size.y - y - 1, x);
        break;

      default: {
        /* Diagonal rotation. */
        int s_x = 0; // Source x coordinate.
        int s_y = 0; // Source y coordinate.

        if (rotate == 1) {
          /* 45 degrees from north. ?? */
          s_x = y - corner + x;
          s_y = source.dimensions().y - (x + corner - y) - 1;
        } else if (rotate == 3) {
          /* 135 degrees from north. */
          s_x = source.dimensions().x - (x + corner - y) - 1;
          s_y = source.dimensions().y - (y - corner + x) - 1;
        } else if (rotate == 5) {
          /* 225 degrees from north. ?? */
          s_x = source.dimensions().x - (y - corner + x) - 1;
          s_y = x + corner - y;
        } else if (rotate == 7) {
          /* 315 degrees from north. */
          s_x = x + corner - y;
          s_y = y - corner + x;
        } else {
          /* This shouldn't happen. Sheesh. */
          throw std::runtime_error("Bad rotation! Go stand in the corner!");
        }

        if (s_x >= 0 && s_y >= 0 &&
            s_x < source.dimensions().x && s_y < source.dimensions().y) {

          /* Blend in surrounding pixels. */
          Pixel dot;
          if (s_x > 0) {
            dot = source(s_x - 1, s_y);
          }
          if (s_y > 0) {
            dot.mix(source(s_x, s_y - 1));
          } else {
            dot.mix({0, 0, 0, 0});
          }
          if (s_x < source.dimensions().x - 2) {
            dot.mix(source(s_x + 1, s_y));
          } else {
            dot.mix({0, 0, 0, 0});
          }
          if (s_y < source.dimensions().y - 2) {
            dot.mix(source(s_x, s_y + 1));
          } else {
            dot.mix({0, 0, 0, 0});
          }

          /* Write result. */
          dot.mix(source(s_x, s_y));
          (*this)(x,y) = dot;
        }
      } break;
      }
    }
  }
}

/* Duplicate image data on assignment. */
Image& Image::operator=(const Image& source) {
  size = source.dimensions();
  if (this != &source) {
    Pixel* data2 = new Pixel[size.x * size.y];
    std::swap(data2, data);
    delete [] data2;

    for (int y = 0; y < size.y; y++) {
      for (int x = 0; x < size.x; x++) {
        (*this)(x,y) = source(x,y);
      }
    }
  }
  return *this;
}

/* Clean up. */
Image::~Image() {
  delete [] data;
}

/* Alpha blend another image on top of this one. */
void Image::overlay(const Image& source) {
  int xtop = (size.x < source.dimensions().x) ? size.x : source.dimensions().x;
  int ytop = (size.y < source.dimensions().y) ? size.y : source.dimensions().y;
  for (int x = 0; x < xtop; x++) {
    for (int y = 0; y < ytop; y++) {
      data[y*size.x + x].blend_over(source(x,y));
    }
  }
}

/* Write results to file. */
void Image::output(std::string filename, bool trim) const {
  int top = 0;
  int left = 0;
  int width = size.x;
  int height = size.y;
  if (trim) {
    left = size.x - 1;
    int right = 0;
    /* Find top of image. */
    for (int i = 0; i < (size.x * size.y); i++) {
      if (data[i].A > 0) {
        top = i / size.x;
        break;
      }
    }

    /* Find left and right of image. */
    for (int y = 0; y < size.y; y++) {
      for (int x = 0; x < size.x; x++) {
        if (data[y * size.x + x].A > 0) {
          if (left > x) left = x;
          break;
        }
      }
      for (int x = size.x - 1; x >= 0; x--) {
        if (data[y * size.x + x].A > 0) {
          if (right < x) right = x;
          break;
        }
      }
    }
    width = right - left;

    /* Find bottom of image. */
    for (int i = (size.x * size.y) - 1; i >= 0; i--) {
      if (data[i].A > 0) {
        height = (i / size.x) - top + 1;
        break;
      }
    }
  }

  /* Create file. */
  FILE *fp = fopen(filename.c_str(), "wb");
  if (!fp){
    throw std::runtime_error(std::string("Couldn't open ") + filename
                             + " for writing.");
  }

  /* Initialize png writer. */
  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                                NULL, NULL, NULL);
  if (!png_ptr){
    throw std::runtime_error(filename + ": png_create_write_struct failed.");
  }

  png_infop info_ptr = png_create_info_struct(png_ptr);
  if (!info_ptr){
    throw std::runtime_error(filename + ": png_create_info_struct failed.");
  }

  if (setjmp(png_jmpbuf(png_ptr))){
    throw std::runtime_error(filename + ": init_io failed.");
  }
  png_init_io(png_ptr, fp);

  /* write header */
  if (setjmp(png_jmpbuf(png_ptr))){
    throw std::runtime_error(filename + ": writing header failed.");
  }

  png_set_IHDR(png_ptr, info_ptr, width, height,
               8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
               PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
  png_write_info(png_ptr, info_ptr);


  /* write bytes */
  if (setjmp(png_jmpbuf(png_ptr))){
    throw std::runtime_error(filename + ": writing failed.");
  }
  for (int i = top; i < (top + height); i++){
    png_write_row(png_ptr, (png_byte*)(data + i*size.x + left));
  }


  /* end write */
  if (setjmp(png_jmpbuf(png_ptr))){
    throw std::runtime_error(filename + ": writing trailer failed.");
  }
  png_write_end(png_ptr, NULL);

  fclose(fp);
}

/* Colour replace. */
void Image::colour_replace(const Pixel& from, const Pixel& to) {
  for (int i = 0; i < size.x * size.y; i++) {
    if (data[i] == from)
      data[i] = to;
  }
}

/* Return a pixel. */
Pixel& Image::operator()(int x, int y) {
  if (x < 0 || x >= size.x) {
    throw std::out_of_range("Image X position out of range.");
  }
  if (y < 0 || y >= size.y) {
    throw std::out_of_range("Image Y position out of range.");
  }
  return data[y*size.x + x];
}
Pixel Image::operator()(int x, int y) const {
  if (x < 0 || x >= size.x) {
    throw std::out_of_range("Image X position out of range.");
  }
  if (y < 0 || y >= size.y) {
    throw std::out_of_range("Image Y position out of range.");
  }
  return data[y*size.x + x];
}
