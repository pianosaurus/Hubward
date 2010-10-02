#include "chunk.hpp"

#include <stdexcept>

/* Open and read file. */
Chunk::Chunk(std::string filepath, const Level::position& pos)
  : Parser(filepath),
    p_blocks(dynamic_cast<const NBT::TAG_Byte_Array*>(fetch("Level.Blocks"))),
    p_skylight(dynamic_cast<const NBT::TAG_Byte_Array*>(fetch("Level.SkyLight"))),
    p_blocklight(dynamic_cast<const NBT::TAG_Byte_Array*>(fetch("Level.BlockLight"))),
    p_data(dynamic_cast<const NBT::TAG_Byte_Array*>(fetch("Level.Data"))) {
  position = {pos.second, pos.first, 0};
}

/* Construct an empty dummy chunk. */
Chunk::Chunk(const Level::position& pos) : Parser(),
                                           p_blocks(0), p_skylight(0),
                                           p_blocklight(0), p_data(0) {
  position = {pos.second, pos.first, 0};
}

/* Get block type at position. */
unsigned char Chunk::blocks(const pvector& pos) const {
  if (!p_blocks)
    throw std::runtime_error("Chunk has no Blocks section.");

  return p_blocks->payload[pos.nbt()];
}

/* Get skylight at position. */
unsigned char Chunk::skylight(const pvector& pos) const {
  if (!p_skylight)
    throw std::runtime_error("Chunk has no SkyLight section.");

  bool upper;
  unsigned char result = p_skylight->payload[pos.nbt(upper)];
  if (upper)
    result >>= 4;
  else
    result &= 0xf;

  return result;
}

/* Get blocklight at position. */
unsigned char Chunk::blocklight(const pvector& pos) const {
  if (!p_blocklight)
    throw std::runtime_error("Chunk has no BlockLight section.");

  bool upper;
  unsigned char result = p_blocklight->payload[pos.nbt(upper)];
  if (upper)
    result >>= 4;
  else
    result &= 0xf;

  return result;
}

/* Get data at position. */
unsigned char Chunk::data(const pvector& pos) const {
  if (!p_data)
    throw std::runtime_error("Chunk has no Data section.");

  bool upper;
  unsigned char result = p_data->payload[pos.nbt(upper)];
  if (upper)
    result >>= 4;
  else
    result &= 0xf;

  return result;
}

/* Compare chunk positions. */
bool Chunk::operator<(const Chunk& chunk) const {
  if (position.x < chunk.get_position().x)
    return true;
  else if (position.x > chunk.get_position().x)
    return false;
  else if (position.z < chunk.get_position().z)
    return true;
  else
    return false;
}
