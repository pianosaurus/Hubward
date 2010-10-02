#include "image.hpp"
#include "chunk.hpp"
#include "render_contour.hpp"

/* Simple renderer with no file output. */
Render_Contour::Render_Contour(const std::string& filename,
                               const recipe& options)
  : Renderer(filename, options) {
  this->options.dimdepth = boolopt(false, "litdepth");

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
Pixel Render_Contour::getblock(const chunkbox& chunks, pvector pos,
                               direction dir) {
  /* Get a proper target. */
  Chunk* target = fixpvector(chunks, pos);

  /* Fetch block from target. */
  unsigned char type = target->blocks(pos);
  Pixel result = (dir & TOP) ? colours[type].top : colours[type].side;

  /* Transparent and partly transparent blocks are invisible. */
  if (result.A < 0xff) {
    return {0, 0, 0, 0};
  }

  /* Tops are black if divisible by 5. */
  if ((dir & TOP) && (pos.y % 5 == 0)) { // TODO: Offset for shoreline.
    /* But only if there is a neighbouring transparent block. */
    bool air = false;
    try {
      air = air ||
        (Renderer::getblock(chunks, pos + pvector(1, 0, 0), TOP).A < 0xff);
    } catch (std::exception& e) {}
    try {
      air = air ||
        (Renderer::getblock(chunks, pos + pvector(0, 1, 0), TOP).A < 0xff);
    } catch (std::exception& e) {}
    try {
      air = air ||
        (Renderer::getblock(chunks, pos - pvector(1, 0, 0), TOP).A < 0xff);
    } catch (std::exception& e) {}
    try {
      air = air ||
        (Renderer::getblock(chunks, pos - pvector(0, 1, 0), TOP).A < 0xff);
    } catch (std::exception& e) {}

    if (air)
      return {0, 0, 0, 0xff};
  }

  return {0xff, 0xff, 0xff, 0xff};
}

/* Everything is fully lit in contour mode. */
unsigned char Render_Contour::getlight(const chunkbox& chunks,
                                       pvector pos,
                                       direction dir) {
  return 255;
}

/* Finalise and save. */
void Render_Contour::finalise() {
  if (image) {
    /* Make white transparent. */
    image->colour_replace({0xff, 0xff, 0xff, 0xff}, {0, 0, 0, 0});
  }
  Renderer::finalise();
}
