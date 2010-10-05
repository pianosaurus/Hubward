#ifndef H_NBT
#define H_NBT

#include <zlib.h>
#include <string>
#include <ostream>

#include "nbttags.hpp"

namespace NBT {

  /*
   * This class simply reads an NBT file into memory.
   */
  class Parser {
  private:
    TAG_Compound root;

  public:
    /* Read filepath into memory. */
    Parser(std::string filepath);
    /* Create an empty dummy NBT. */
    Parser() {};

    /* Print structure of named tags. */
    void print_structure(std::ostream& out);

    /* Fetch a tag by path (eg. Level.Blocks). */
    const TAG* fetch(std::string path);
  };

}

#endif
