#ifndef H_LEVEL
#define H_LEVEL

#include <string>
#include <list>

#include "pvector.hpp"

using std::string;
using std::list;

class Renderer;
class Chunk;

/*
 * This class loads a level from drive, either in it's entirety or
 * only the requested chunks.
 */
class Level {
public:
  /* A struct for the loaded chunk and it's position.
     0,0 is spawn by default. */
  struct chunkhead {
    pvector pos;
    string path;
    Chunk* data;
  };

  /* Constructors. */
  /* Find all chunk files. */
  Level(const string& path);
  /* Find requested chunk files. */
  Level(const string& path, const list<chunkhead>& intersect);

  /* Load files while rendering, clear data from memory continuously. */
  void render(Renderer& renderer);
  void render(list<Renderer*>& renderers);

private:
  /* Levels cannot be copied or assigned. */
  Level(const Level&);
  Level& operator=(const Level&);

  /* List of chunk positions and file paths. */
  list<chunkhead> chunks;

  /* Bounding box. */
  pvector top_right;
  pvector bottom_left;
  void update_bounds(const pvector& pos);

  /* Convert to/from base36. */
  int base36toint(const string& s);
  string inttobase36(int i, bool mod64 = false);

  /* Compare chunkheads according to position. */
  static bool comparehead(const chunkhead& a, const chunkhead& b);
};

#endif
