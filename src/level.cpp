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
Level::Level(const string& path) {
  std::stack<string> directories;

  directories.push(path);
  /* Loop recursively through all found directories. */
  while (!directories.empty()) {
    string current = directories.top();
    directories.pop();

    DIR* dir = opendir(current.c_str());
    if (!dir) {
      std::cerr << "Warning: Couldn't open directory: " << current << "\n";
      continue;
    }

    /* Loop through all entries in the current directory. */
    dirent* ent;
    while (ent = readdir(dir)) {
      string entryname = ent->d_name;
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
          chunkhead newchunk;
          newchunk.path = entryname;
          string x, z;

          /* Parse file name. */
          std::istringstream stream(ent->d_name);
          string token;
          if (!std::getline(stream, token, '.').good() || token != "c" ||
              !std::getline(stream, x, '.').good() ||
              !std::getline(stream, z, '.').good() ||
              !std::getline(stream, token).eof() || token != "dat") {
            std::cerr << "Warning: Unknown file " << ent->d_name << "\n";
          } else {
            newchunk.pos.x = base36toint(x);
            newchunk.pos.z = base36toint(z);
          }

          /* Chunk file name found. Add to list. */
          update_bounds(newchunk.pos);
          chunks.push_back(newchunk);
        }
      }
    }

    /* Done with this directory. Continue with next. */
    closedir(dir);
  }

  /* Make sure the chunks are sorted. */
  chunks.sort(Level::comparehead);
  chunks.reverse();
}

/* Find requested chunk files. */
Level::Level(const string& path, const list<chunkhead>& intersect) {
  /* Loop through intersect and see if the corresponding files exist. */
  for (list<chunkhead>::const_iterator it = intersect.begin();
       it != intersect.end(); ++it) {
    string file = path + "/" + inttobase36((*it).pos.x % 64, true)
                       + "/" + inttobase36((*it).pos.z % 64, true)
                       + "/c." + inttobase36((*it).pos.x) + "."
                       + inttobase36((*it).pos.z) + ".dat";
    struct stat state;
    if (stat(file.c_str(), &state) || !S_ISREG(state.st_mode)) {
      std::cerr << "Warning: Couldn't stat file " << file << "\n";
    } else {
      /* Got a file. Add path name to chunk list. */
      chunkhead newchunk = {{(*it).pos.x, (*it).pos.z}, file};
      update_bounds(newchunk.pos);
      chunks.push_back(newchunk);
    }
  }

  /* Make sure the chunks are sorted. */
  chunks.sort(Level::comparehead);
  chunks.reverse();
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

  /* Create a stack of loaded chunks. */
  std::stack<chunkhead*> data;
  bool done_loading = false;

  /* Load and render chunks in parallel. */
  /* TODO: This parallelisation seems to just drown in overhead.
     Apparently, rendering is really fast. Try with more renderers. */
#pragma omp parallel sections
  {
#pragma omp section
    {
      /* Load files into memory. */
      for (list<chunkhead>::iterator it = chunks.begin();
           it != chunks.end(); ++it) {
        (*it).data = new Chunk((*it).path, (*it).pos);

#pragma omp critical(chunk_list)
        data.push(&(*it));
      }

#pragma omp critical(chunk_list)
      done_loading = true;
    }

#pragma omp section
    {
      /* Render loaded chunks. */
      bool domore;
#pragma omp critical(chunk_list)
      domore = !done_loading || !data.empty();

      while (domore) {
        chunkhead* loaded = 0;

#pragma omp critical(chunk_list)
        if (!data.empty()) {
          loaded = data.top();
          data.pop();
        }

        if (loaded) {
          /* We have a chunk. Render it. */
          for (list<Renderer*>::iterator renderer = renderers.begin();
               renderer != renderers.end(); ++renderer) {
            (*renderer)->render(*((*loaded).data));
          }

          delete (*loaded).data;
        }

#pragma omp critical(chunk_list)
        domore = !done_loading || !data.empty();
      }
    }
  }

  std::cout << "Loaded " << chunks.size() << " chunks." << std::endl;
}

/* Update bounding box to include pos. */
void Level::update_bounds(const pvector& pos) {
  if (chunks.size() == 0) {
    /* This is the first chunk. */
    top_right = bottom_left = pos;
  } else {
    top_right.x = (pos.x < top_right.x) ? pos.x : top_right.x;
    top_right.z = (pos.z < top_right.z) ? pos.z : top_right.z;
    bottom_left.x = (pos.x > bottom_left.x) ? pos.x : bottom_left.x;
    bottom_left.z = (pos.z > bottom_left.z) ? pos.z : bottom_left.z;
  }
}

/* Convert from base36. */
int Level::base36toint(const string& s) {
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
string Level::inttobase36(int i, bool mod64) {
  string result;
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

/* Compare chunkheads according to position. */
bool Level::comparehead(const chunkhead& a, const chunkhead& b) {
  if (a.pos.x < b.pos.x)
    return true;
  else if (a.pos.x > b.pos.x)
    return false;
  else if (a.pos.z < b.pos.z)
    return true;
  else
    return false;
}
