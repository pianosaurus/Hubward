#ifndef H_RENDERER
#define H_RENDERER

#include "pixel.hpp"
#include "pvector.hpp"
#include "level.hpp"

#include <string>
#include <vector>

class Chunk;
class Image;

/*
 * This class does the rendering and outputs to filesystem.
 * It is not meant to be used directly, but to be subclassed.
 */
class Renderer {
public:
  struct option {
    char shortopt;
    std::string longopt;
    std::string argname;
    std::string description;
    std::string defaultval;
  };
  static const option options[4];

  /* A direction enum. */
  enum direction {
    TOP = 1,
    N = 2,
    NE = 6,
    E = 4,
    SE = 12,
    S = 8,
    SW = 24,
    W = 16,
    NW = 18,
    BOTTOM = 32,
    SIDES = 30,
    ALL = 63
  };

  /* Initialise a default renderer with no file output. */
  Renderer(bool oblique = false, direction up = N,
           unsigned int skylight = 127);
  /* Initialise a renderer based on an option string. Options will be parsed
     up to the first "-- ", and parsed options will be removed. */
  Renderer(std::string& options);

  /* Clean up*/
  virtual ~Renderer();

  /* Let the renderer know the coordinates of the map corners. */
  void set_surface(const Level::position& top_right_chunk,
                   const Level::position& bottom_left_chunk);

  /* Pass a chunk to the renderer and let it do its thing. */
  virtual void render(const Chunk& chunk,
                      const Chunk& north, const Chunk& east,
                      const Chunk& south, const Chunk& west);

  /* Finalise and save image. */
  void save();

  /* Set the alpha channel of certain block types. */
  static void set_default_alpha(std::vector<unsigned char>& types,
                                unsigned char alpha,
                                bool invertlist = false, direction tex = ALL);
  void set_alpha(std::vector<unsigned char>& types,
                 unsigned char alpha,
                 bool invertlist = false, direction tex = ALL);

protected:
  struct colourmap {
    Pixel top;
    Pixel side;
  };

  std::string filename;

  /* Colours of blocks. */
  static colourmap default_colours[256];
  colourmap colours[256];

  /* Map up direction. */
  direction up;

  /* Whether the view is angled instead of top-down. */
  bool oblique;

  /* Lighting type. */
  unsigned char skylight;

  /* World position of corner chunks. */
  pvector top_right;
  pvector bottom_left;

  /* Raw image data. */
  Image* image;

  /* Parse an option. */
  virtual void parseoption(char shortopt, std::string* argument = 0);

  /* Get colour value of a block. */
  virtual Pixel getblock(const Chunk& chunk, const pvector& pos,
                         direction dir);

  /* Get lighting value of a block. */
  virtual unsigned char getlight(const Chunk& chunk, const pvector& pos,
                                 direction dir);


private:
  /* Don't allow copy construction or assignment. */
  Renderer(const Renderer& source);
  Renderer& operator=(const Renderer& source);

};

#endif
