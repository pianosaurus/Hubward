#include "nbt.hpp"

#include <stdexcept>
using namespace NBT;

/* Open and read file. */
Parser::Parser(std::string filepath) {
  /* Open the file for reading. */
  gzFile file = gzopen(filepath.c_str(), "rb");
  if (file == Z_NULL) {
    throw std::runtime_error(string("Couldn't open file ") + filepath);
  }

  /* Read the file, tag by tag. */
  try {
    int type = gzgetc(file);
    if (type != 10) {
      if (type == -1) {
        int err;
        throw std::runtime_error(gzerror(file, &err));
      }
      throw std::runtime_error(string("Root tag is not compound in")
                               + filepath);
    }
    root.get(file, true);
  } catch (std::runtime_error& e) {
    gzclose(file);
    throw std::runtime_error(string(e.what()) + " in " + filepath);
  } catch (std::exception& e) {
    gzclose(file);
    throw;
  }

  /* Close the file. */
  gzclose(file);
}

/* Print structure of named tags. */
void Parser::print_structure(std::ostream& out) {
  root.print_structure(out, 0);
  out << std::flush;
}

/* Fetch a tag by path (eg. Level.Blocks). */
const TAG* Parser::fetch(std::string path) {
  return root.fetch(path);
}
