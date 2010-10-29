#ifndef H_RENDER_CONTOUR
#define H_RENDER_CONTOUR

#include "renderer.hpp"

#include <string>

/*
 * For rendering contour lines on a transparent background.
 */
class Render_Contour : public Renderer {
public:
  /* Initialise a contour renderer. */
  Render_Contour(const std::string& filename, const recipe& options);

protected:
  /* Get colour value of a block. */
  virtual Pixel getblock(const Chunk& chunk, pvector pos, direction dir);

  /* Get lighting value of a block. */
  virtual double getlight(const Chunk& chunk, pvector pos, direction dir);

  /* Draw contours from heightmap. */
  virtual void finalise();
};

#endif
