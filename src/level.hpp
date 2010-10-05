#ifndef H_LEVEL
#define H_LEVEL

#include <string>
#include <list>
#include <utility>
#include <map>
#include <utility>

#include "pvector.hpp"

class Renderer;
class Chunk;

/*
 * This class loads a level from drive, either in it's entirety or
 * only the requested chunks.
 */
class Level {
public:
  /* Chunk positions given as x,z. */
  typedef std::pair<int, int> position;

  /* Generate chunk list from geometry string. */
  static std::list<position> chunk_list(const std::string& geometry);

  /* Constructors. */
  /* Find all chunk files. */
  Level(const std::string& path);
  /* Find requested chunk files. */
  Level(const std::string& path,
        const std::list<position>& intersect);
  /* Clear chunk map. */
  ~Level();

  /* Load files while rendering, clear data from memory continuously. */
  void render(Renderer& renderer);
  void render(std::list<Renderer*>& renderers);

private:
  /* Levels cannot be copied or assigned. */
  Level(const Level&);
  Level& operator=(const Level&);

  /* Map of chunk positions, chunks and file paths. */
  typedef std::pair<std::string, Chunk*> datasource; // Chunk file and data.
  typedef std::map<position, datasource> chunkmap; // A map of chunks in level.
  chunkmap chunks;

  /* Bounding box. */
  position top_right;
  position bottom_left;
  void update_bounds(const position& pos);

  /* Convert to/from base36. */
  static int base36toint(const std::string& s);
  static std::string inttobase36(int i, bool mod64 = false);
};

#endif
