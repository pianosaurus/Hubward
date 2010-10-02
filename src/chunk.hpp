#ifndef H_CHUNK
#define H_CHUNK

#include "nbt.hpp"
#include "level.hpp"
#include "pvector.hpp"

/*
 * This class simplifies reading data from chunk NBTs.
 */
class Chunk : public NBT::Parser {
private:
  pvector position;

  /* Cached pointers to important data. */
  const NBT::TAG_Byte_Array* p_blocks;
  const NBT::TAG_Byte_Array* p_skylight;
  const NBT::TAG_Byte_Array* p_blocklight;
  const NBT::TAG_Byte_Array* p_data;

public:
  /* Read filepath into memory. */
  Chunk(std::string filepath, const Level::position& pos);
  /* TODO: Add a no-position constructor, read position from chunk or
     throw an exception. */
  /* Construct an empty dummy chunk. */
  Chunk(const Level::position& pos);

  /* Get positional data. */
  unsigned char blocks(const pvector& pos) const;
  unsigned char skylight(const pvector& pos) const;
  unsigned char blocklight(const pvector& pos) const;
  unsigned char data(const pvector& pos) const;

  /* Get chunk position. */
  pvector get_position() const { return position; };

  /* Compare chunk positions. */
  bool operator<(const Chunk& chunk) const;
  bool operator>(const Chunk& chunk) const { return !operator<(chunk); };
};

#endif
