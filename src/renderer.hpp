#ifndef H_RENDERER
#define H_RENDERER

#include "pixel.hpp"
#include "pvector.hpp"
#include "level.hpp"

#include <list>
#include <string>
#include <vector>

class Chunk;
class Image;

/*
 * This class does the default rendering and outputs to filesystem.
 */
class Renderer {
  bool test;
public:
  struct chunkbox {
    Chunk* center; // Chunk being rendered.
    Chunk* north;  // North neighbour.
    Chunk* east;   // East neighbour.
    Chunk* south;  // South neighbour.
    Chunk* west;   // West neighbour.
  };

  /* A direction enum. */
  enum direction {
    TOP = 1,
    N = 2,
    E = 4,
    S = 8,
    W = 16,
    NE = 32,
    SE = 64,
    SW = 128,
    NW = 256,
    BOTTOM = 512,
    CARDINAL = N + E + S + W,
    ORDINAL = NE + SE + SW + NW,
    ALL = CARDINAL + ORDINAL + TOP + BOTTOM
  };

  /* Possible overlays. */
  enum overlay_type {
    DEFAULT,
    CONTOUR
  };

  /* Option values, and the input strings used to set them. */
  typedef std::pair<direction, std::string> directionopt;
  typedef std::pair<unsigned char, std::string> ucharopt;
  typedef std::pair<bool, std::string> boolopt;

  /* Holds the needed info to create a renderer. */
  struct recipe {
    directionopt dir;
    ucharopt lightlevel;
    boolopt dimdepth;
    boolopt oblique;
  };

  /* Generate a list of renderers based on an option string. It is the
     callers responsibility to delete these renderers. If source is
     given, only one renderer will be created and no filename is
     parsed.  All members of opts may be overridden except rotation
     and obliqueness. */
  typedef std::list<Renderer*> RenderList;
  static RenderList make_renderers(const std::string& options,
                                   const recipe* source = 0);

  /* Construct renderer. */
  Renderer(const std::string& filename, const recipe& options);

  /* Clean up*/
  virtual ~Renderer();

  /* Let the renderer know the coordinates of the map corners. */
  void set_surface(const Level::position& top_right_chunk,
                   const Level::position& bottom_left_chunk);

  /* Pass a chunk to the renderer and let it do its thing. */
  void render(const chunkbox& chunks);

  /* Make any last minute adjustments. */
  virtual void finalise();

  /* Save image. */
  void save();

  /* Return a reference to the image. Can only be done after it has been
     finalised. The reference is valid until the renderer is deleted. */
  const Image& get_image() const;

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

  /* Holds the options used to create the renderer.*/
  recipe options;

  /* File path and name this render will be written to. */
  std::string filename;

  /* Colours of blocks. */
  static colourmap default_colours[256];
  colourmap colours[256];

  /* World position of corner chunks. */
  pvector top_right;
  pvector bottom_left;

  /* Rendered image. */
  Image* image;

  /* Overlays. */
  RenderList overlays;

  /* Is the renderer finalised? */
  bool finalised;

  /* Get unlit colour value of a block. */
  virtual Pixel getblock(const chunkbox& chunks, pvector pos,
                         direction dir);

  /* Get lighting value of a block. */
  virtual unsigned char getlight(const chunkbox& chunks, pvector pos,
                                 direction dir);

  /* Get a block, light it and blend behind a pixel. */
  virtual void blendblock(const chunkbox& chunks, pvector pos,
                          direction dir, Pixel& top);

  /* Negate a cardinal or ordinal direction. */
  static direction negate_direction(direction direction);

  /* Convert a chunkbox-pvector combo to a chunk-pvector combo. The pvector
     passed in may point outside the center chunk. */
  static Chunk* fixpvector(const chunkbox& chunks, pvector& pos);

  /* Add another renderer as an overlay to this one. This will take
     ownership, and the overlays will be deleted along with self. */
  void overlay(Renderer* renderer) { overlays.push_back(renderer); };

private:
  /* Don't allow copy construction or assignment. */
  Renderer(const Renderer& source);
  Renderer& operator=(const Renderer& source);

  /* Wildcard replacement for file names. */
  static std::string wildcard(const std::string& filename,
                              const std::string& wildcard,
                              const std::string& replacement);

};

#endif
