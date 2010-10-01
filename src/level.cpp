#include "level.hpp"
#include "chunk.hpp"
#include "renderer.hpp"
#include "image.hpp"

#include <sstream>
#include <iostream>
#include <stack>

#include <dirent.h>
#include <sys/stat.h>

#include <stdexcept>

/* Find all chunk files. */
Level::Level(const std::string& path) {
  std::stack<std::string> directories;

  directories.push(path);
  /* Loop recursively through all found directories. */
  while (!directories.empty()) {
    std::string current = directories.top();
    directories.pop();

    DIR* dir = opendir(current.c_str());
    if (!dir) {
      std::cerr << "Warning: Couldn't open directory: " << current << "\n";
      continue;
    }

    /* Loop through all entries in the current directory. */
    dirent* ent;
    while (ent = readdir(dir)) {
      std::string entryname = ent->d_name;
      if (entryname != "." && entryname != ".." &&
          entryname != "level.dat" && entryname != "level.dat_old" &&
          entryname != "session.lock") {
        entryname = current + "/" + entryname;

        /* Find out if entry is a directory or a file. */
        struct stat state;
        if (stat(entryname.c_str(), &state)) {
          std::cerr << "Warning: Couldn't stat file " << ent->d_name << "\n";
        } else if (S_ISDIR(state.st_mode)) {
          directories.push(entryname);
        } else if (S_ISREG(state.st_mode)) {
          std::string x, z;

          /* Parse file name. */
          std::istringstream stream(ent->d_name);
          std::string token;
          if (!std::getline(stream, token, '.').good() || token != "c" ||
              !std::getline(stream, x, '.').good() ||
              !std::getline(stream, z, '.').good() ||
              !std::getline(stream, token).eof() || token != "dat") {
            std::cerr << "Warning: Unknown file " << ent->d_name << "\n";
          } else {
            /* Chunk file name found. Add to map. */
            position pos(base36toint(z), base36toint(x));
            chunks.insert(std::pair<position, datasource>
                          (pos, datasource(entryname, (Chunk*)0)));
            update_bounds(pos);
          }
        }
      }
    }

    /* Done with this directory. Continue with next. */
    closedir(dir);
  }
}

/* Find requested chunk files. */
Level::Level(const std::string& path,
             const std::list<position>& intersect) {
  /* Loop through intersect and see if the corresponding files exist. */
  for (std::list<position>::const_iterator it =
         intersect.begin();
       it != intersect.end(); ++it) {
    std::string file = path + "/" + inttobase36((*it).second % 64, true)
                            + "/" + inttobase36((*it).first % 64, true)
                            + "/c." + inttobase36((*it).second) + "."
                            + inttobase36((*it).first) + ".dat";
    struct stat state;
    if (stat(file.c_str(), &state) || !S_ISREG(state.st_mode)) {
      std::cerr << "Warning: Couldn't stat file " << file << "\n";
    } else {
      /* Chunk file name found. Add to map. */
      position pos((*it).first, (*it).second);
      chunks.insert(std::pair<position, datasource>
                    (pos, datasource(file, (Chunk*)0)));
      update_bounds(pos);
    }
  }
}

/* Clear chunk map on delete. */
Level::~Level() {
  for (chunkmap::reverse_iterator it = chunks.rbegin();
       it != chunks.rend(); ++it) {
    delete it->second.second;
  }
}

/* Load files while rendering, clear data from memory continuously. */
void Level::render(Renderer& renderer) {
  list<Renderer*> renderers;
  renderers.push_back(&renderer);
  render(renderers);
}
void Level::render(list<Renderer*>& renderers) {
  /* Initialise renderers with map size. */
  for (list<Renderer*>::iterator renderer = renderers.begin();
       renderer != renderers.end(); ++renderer) {
    (*renderer)->set_surface(top_right, bottom_left);
  }

  /* Load and render chunks in parallel. */
  /* TODO: This parallelisation seems to just drown in overhead.
     Apparently, rendering is really fast. Try with more renderers. */
#pragma omp parallel sections
  {
#pragma omp section
    {
      /* Load files into memory. */
      for (chunkmap::reverse_iterator it = chunks.rbegin();
           it != chunks.rend(); ++it) {
        Chunk* loading = new Chunk(it->second.first, it->first);
#pragma omp critical(chunks)
        it->second.second = loading;
      }
    }

#pragma omp section
    {
      /* Render loaded chunks. */
      for (chunkmap::reverse_iterator it = chunks.rbegin();
      it != chunks.rend(); ++it) {
        /* Wait for the chunk's requirements to load. */
        //#pragma omp critical(chunks)
        Chunk* chunk = it->second.second;
        //#pragma omp critical(chunks)
        while (chunk == 0) {
          /* TODO: Give up a timeslice. */
          chunk = it->second.second;
        }

        /* We have a chunk. Render it. */
        for (list<Renderer*>::iterator renderer = renderers.begin();
             renderer != renderers.end(); ++renderer) {
          (*renderer)->render(*(it->second.second));
        }

        /* Delete chunks that will no longer be needed. */
        delete it->second.second;
        it->second.second = 0;
      }
    }
  }

  std::cout << "Loaded " << chunks.size() << " chunks." << std::endl;
}

/* Update bounding box to include pos. */
void Level::update_bounds(const position& pos) {
  if (chunks.size() == 0) {
    /* This is the first chunk. */
    top_right = bottom_left = pos;
  } else {
    if (pos.first < top_right.first) top_right.first = pos.first;
    if (pos.second < top_right.second) top_right.second = pos.second;

    if (pos.first > bottom_left.first) bottom_left.first = pos.first;
    if (pos.second > bottom_left.second) bottom_left.second = pos.second;
  }
}

/* Convert from base36. */
int Level::base36toint(const std::string& s) {
  /* TODO: Overflows on large input. Not relevant to MineDraft, but
     it should be fixed anyway. */
  int result = 0;
  const char* cstr = s.c_str();
  bool negative = (cstr[0] == '-');
  if (negative)
    cstr = &cstr[1];

  while (cstr[0] != 0) {
    result = result * 36;
    if (cstr[0] >= 0x30 && cstr[0] <= 0x39) {        // 0-9
      result += (cstr[0] - 0x30);
    } else if (cstr[0] >= 0x61 && cstr[0] <= 0x7a) { // a-z
      result += (cstr[0] - 0x61 + 10);
    } else if (cstr[0] >= 0x41 && cstr[0] <= 0x5a) { // A-Z
      result += (cstr[0] - 0x41 + 10);
    } else {
      throw std::runtime_error("Failed to convert from base36.");
    }
    cstr = &cstr[1];
  }

  if (negative) result = -result;

  return result;
}

/* Convert to base36. */
std::string Level::inttobase36(int i, bool mod64) {
  std::string result;
  char prepend[] = {0,0};

  if (mod64) {
    i = i % 64;
    if (i < 0)
      i += 64;
  }

  bool negative = (i < 0);
  if (negative)
    i = -i;

  while (i) {
    int remainder = i % 36;
    if (remainder < 10)
      prepend[0] = 0x30 + remainder;
    else
      prepend[0] = 0x61 - 10 + remainder;

    result = prepend + result;
    i = (i - remainder) / 36;
  }

  if (negative)
    result = "-" + result;
  if (result.empty())
    result = "0";
  return result;
}
