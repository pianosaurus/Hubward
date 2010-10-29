#include "image.hpp"
#include "chunk.hpp"
#include "render_contour.hpp"

/* Simple renderer with no file output. */
Render_Contour::Render_Contour(const std::string& filename,
                               const recipe& options)
  : Renderer(filename, options) {
  this->options.dimdepth = boolopt(false, "");

  /* Make most blocks invisible. We don't want buildings to show up. */
  for (int i = 0; i < 256; i++) {
    if (i != 0x01 && // Stone
        i != 0x02 && // Grass
        i != 0x03 && // Dirt
        i != 0x07 && // Bedrock
        i != 0x0c && // Sand
        i != 0x0d && // Gravel
        i != 0x0e && // Gold ore
        i != 0x0f && // Iron ore
        i != 0x10 && // Coal ore
        i != 0x38)   // Diamond ore
      colours[i].top.A = colours[i].side.A = 0x00;
  }
}

/* The top of blocks divisible by 5 are black. The rest is white. Some
   blocks are invisible. */
Pixel Render_Contour::getblock(const Chunk& chunk, pvector pos,
                               direction dir) {
  /* Fetch block from target. */
  unsigned char type = chunk.blocks(pos);
  bool top = dir & TOP;
  Pixel original = (dir & TOP) ? colours[type].top : colours[type].side;

  /* Transparent and partly transparent blocks are invisible. */
  if (original.A < 1-0.005f) {
    return Pixel(0.0f, 0.0f, 0.0f, 0.0f);
  }

  /* Remember height value of blocks in the green channel. */
  return Pixel(0.0f, pos.y - (top ? 0 : 1), 0.0f, 1.0f);
}

/* Everything is fully lit in contour mode. */
double Render_Contour::getlight(const Chunk& chunk, pvector pos,
                                direction dir) {
  return 1.0f;
}

/* Draw contours from heightmap. */
void Render_Contour::finalise() {
  if (image) {
    /* Create contour lines from data image. */
    const Image::ivector size = image->dimensions();
    Image* target = new Image(size);

    for (int x = 0; x < size.x; x++) {
      for (int y = 0; y < size.y; y++) {
        const unsigned char height = (*image)(x, y).G;

        /* Lines every 5 meters, offset for shoreline. */
        if (((*image)(x, y).A > 0) && (height % 5 == 3)) {
          /* Pixel only visible if a nearby pixel is lower. */
          bool edge = false;
          if ((x > 0) && (*image)(x - 1, y).G < height) edge = true;
          if ((y > 0) && (*image)(x, y - 1).G < height) edge = true;
          if ((x < (size.x - 1)) &&
              (*image)(x + 1, y).G < height) edge = true;
          if ((y < (size.y - 1)) &&
              (*image)(x, y + 1).G < height) edge = true;

          if (edge) {
            (*target)(x, y) = Pixel(0.0f, 0.0f, 0.0f, 1.0f);
          }
        }
      }
    }

    delete image;
    image = target;
  }

  Renderer::finalise();
}
