#include "renderer.hpp"

/* Default colours of blocks (top,side). */
Renderer::colourmap Renderer::default_colours[] = {
  { // 0x00 : Air
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x01 : Stone
    Pixel((unsigned char)0x7f, (unsigned char)0x7f,
          (unsigned char)0x7f, (unsigned char)0xff),
    Pixel((unsigned char)0x7f, (unsigned char)0x7f,
          (unsigned char)0x7f, (unsigned char)0xff)
  },
  { // 0x02 : Grass
    Pixel((unsigned char)0x7f, (unsigned char)0xbf,
          (unsigned char)0x4f, (unsigned char)0xff),
    Pixel((unsigned char)0x8f, (unsigned char)0x5f,
          (unsigned char)0x3f, (unsigned char)0xff)
  },
  { // 0x03 : Dirt
    Pixel((unsigned char)0x8f, (unsigned char)0x5f,
          (unsigned char)0x3f, (unsigned char)0xff),
    Pixel((unsigned char)0x8f, (unsigned char)0x5f,
          (unsigned char)0x3f, (unsigned char)0xff)
  },
  { // 0x04 : Cobblestone
    Pixel((unsigned char)0x6f, (unsigned char)0x6f,
          (unsigned char)0x6f, (unsigned char)0xff),
    Pixel((unsigned char)0x6f, (unsigned char)0x6f,
          (unsigned char)0x6f, (unsigned char)0xff)
  },
  { // 0x05 : Wood
    Pixel((unsigned char)0x6f, (unsigned char)0x4f,
          (unsigned char)0x1f, (unsigned char)0xff),
    Pixel((unsigned char)0x6f, (unsigned char)0x4f,
          (unsigned char)0x1f, (unsigned char)0xff)
  },
  { // 0x06 : Sapling
    Pixel((unsigned char)0x3f, (unsigned char)0x8f,
          (unsigned char)0x3f, (unsigned char)0x1f),
    Pixel((unsigned char)0x3f, (unsigned char)0x8f,
          (unsigned char)0x3f, (unsigned char)0x1f)
  },
  { // 0x07 : Bedrock
    Pixel((unsigned char)0x00, (unsigned char)0x00,
          (unsigned char)0x00, (unsigned char)0xff),
    Pixel((unsigned char)0x00, (unsigned char)0x00,
          (unsigned char)0x00, (unsigned char)0xff)
  },
  { // 0x08 : Water
    Pixel((unsigned char)0x1f, (unsigned char)0x4f,
          (unsigned char)0xff, (unsigned char)0x7f),
    Pixel((unsigned char)0x1f, (unsigned char)0x4f,
          (unsigned char)0xff, (unsigned char)0x7f)
  },
  { // 0x09 : Stationary water
    Pixel((unsigned char)0x1f, (unsigned char)0x4f,
          (unsigned char)0xff, (unsigned char)0x7f),
    Pixel((unsigned char)0x1f, (unsigned char)0x4f,
          (unsigned char)0xff, (unsigned char)0x7f)
  },
  { // 0x0a : Lava
    Pixel((unsigned char)0xff, (unsigned char)0x5a,
          (unsigned char)0x00, (unsigned char)0xff),
    Pixel((unsigned char)0xff, (unsigned char)0x5a,
          (unsigned char)0x00, (unsigned char)0xff)
  },
  { // 0x0b : Stationary lava
    Pixel((unsigned char)0xff, (unsigned char)0x5a,
          (unsigned char)0x00, (unsigned char)0xff),
    Pixel((unsigned char)0xff, (unsigned char)0x5a,
          (unsigned char)0x00, (unsigned char)0xff)
  },
  { // 0x0c : Sand
    Pixel((unsigned char)0xff, (unsigned char)0xef,
          (unsigned char)0x7f, (unsigned char)0xff),
    Pixel((unsigned char)0xff, (unsigned char)0xef,
          (unsigned char)0x7f, (unsigned char)0xff)
  },
  { // 0x0d : Gravel
    Pixel((unsigned char)0x8f, (unsigned char)0x7f,
          (unsigned char)0x7f, (unsigned char)0xff),
    Pixel((unsigned char)0x8f, (unsigned char)0x7f,
          (unsigned char)0x7f, (unsigned char)0xff)
  },
  { // 0x0e : Gold ore
    Pixel((unsigned char)0x8f, (unsigned char)0x8c,
          (unsigned char)0x7d, (unsigned char)0xff),
    Pixel((unsigned char)0x8f, (unsigned char)0x8c,
          (unsigned char)0x7d, (unsigned char)0xff)
  },
  { // 0x0f : Iron ore
    Pixel((unsigned char)0x88, (unsigned char)0x82,
          (unsigned char)0x7f, (unsigned char)0xff),
    Pixel((unsigned char)0x88, (unsigned char)0x82,
          (unsigned char)0x7f, (unsigned char)0xff)
  },
  { // 0x10 : Coal ore
    Pixel((unsigned char)0x73, (unsigned char)0x73,
          (unsigned char)0x73, (unsigned char)0xff),
    Pixel((unsigned char)0x73, (unsigned char)0x73,
          (unsigned char)0x73, (unsigned char)0xff)
  },
  { // 0x11 : Log
    Pixel((unsigned char)0x66, (unsigned char)0x51,
          (unsigned char)0x33, (unsigned char)0xff),
    Pixel((unsigned char)0x66, (unsigned char)0x51,
          (unsigned char)0x33, (unsigned char)0xff)
  },
  { // 0x12 : Leaves
    Pixel((unsigned char)0x3c, (unsigned char)0xc0,
          (unsigned char)0x29, (unsigned char)0x7f),
    Pixel((unsigned char)0x3c, (unsigned char)0xc0,
          (unsigned char)0x29, (unsigned char)0x7f)
  },
  { // 0x13 :   Sponge
    Pixel((unsigned char)0xee, (unsigned char)0xff,
          (unsigned char)0x00, (unsigned char)0xff),
    Pixel((unsigned char)0xee, (unsigned char)0xff,
          (unsigned char)0x00, (unsigned char)0xff)
  },
  { // 0x14 : Glass
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x1f),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x1f)
  },
  { // 0x15 : Red cloth
    Pixel((unsigned char)0x00, (unsigned char)0x00,
          (unsigned char)0xaa, (unsigned char)0xff),
    Pixel((unsigned char)0x00, (unsigned char)0x00,
          (unsigned char)0xaa, (unsigned char)0xff)
  },
  { // 0x16 :   Orange cloth
    Pixel((unsigned char)0xde, (unsigned char)0x88,
          (unsigned char)0x32, (unsigned char)0xff),
    Pixel((unsigned char)0xde, (unsigned char)0x88,
          (unsigned char)0x32, (unsigned char)0xff)
  },
  { // 0x17 :   Yellow cloth
    Pixel((unsigned char)0xde, (unsigned char)0xde,
          (unsigned char)0x32, (unsigned char)0xff),
    Pixel((unsigned char)0xde, (unsigned char)0xde,
          (unsigned char)0x32, (unsigned char)0xff)
  },
  { // 0x18 :   Lime cloth
    Pixel((unsigned char)0x88, (unsigned char)0xde,
          (unsigned char)0x32, (unsigned char)0xff),
    Pixel((unsigned char)0x88, (unsigned char)0xde,
          (unsigned char)0x32, (unsigned char)0xff)
  },
  { // 0x19 :   Green cloth
    Pixel((unsigned char)0x32, (unsigned char)0xde,
          (unsigned char)0x32, (unsigned char)0xff),
    Pixel((unsigned char)0x32, (unsigned char)0xde,
          (unsigned char)0x32, (unsigned char)0xff)
  },
  { // 0x1a :   Aqua green cloth
    Pixel((unsigned char)0x32, (unsigned char)0xde,
          (unsigned char)0x88, (unsigned char)0xff),
    Pixel((unsigned char)0x32, (unsigned char)0xde,
          (unsigned char)0x88, (unsigned char)0xff)
  },
  { // 0x1b :   Cyan cloth
    Pixel((unsigned char)0x32, (unsigned char)0xde,
          (unsigned char)0xde, (unsigned char)0xff),
    Pixel((unsigned char)0x32, (unsigned char)0xde,
          (unsigned char)0xde, (unsigned char)0xff)
  },
  { // 0x1c :   Blue cloth
    Pixel((unsigned char)0x68, (unsigned char)0xa3,
          (unsigned char)0xde, (unsigned char)0xff),
    Pixel((unsigned char)0x68, (unsigned char)0xa3,
          (unsigned char)0xde, (unsigned char)0xff)
  },
  { // 0x1d :   Purple cloth
    Pixel((unsigned char)0x78, (unsigned char)0x78,
          (unsigned char)0xde, (unsigned char)0xff),
    Pixel((unsigned char)0x78, (unsigned char)0x78,
          (unsigned char)0xde, (unsigned char)0xff)
  },
  { // 0x1e :   Indigo cloth
    Pixel((unsigned char)0x88, (unsigned char)0x32,
          (unsigned char)0xde, (unsigned char)0xff),
    Pixel((unsigned char)0x88, (unsigned char)0x32,
          (unsigned char)0xde, (unsigned char)0xff)
  },
  { // 0x1f :   Violet cloth
    Pixel((unsigned char)0xae, (unsigned char)0x4a,
          (unsigned char)0xde, (unsigned char)0xff),
    Pixel((unsigned char)0xae, (unsigned char)0x4a,
          (unsigned char)0xde, (unsigned char)0xff)
  },
  { // 0x20 :   Magenta cloth
    Pixel((unsigned char)0xde, (unsigned char)0x32,
          (unsigned char)0xde, (unsigned char)0xff),
    Pixel((unsigned char)0xde, (unsigned char)0x32,
          (unsigned char)0xde, (unsigned char)0xff)
  },
  { // 0x21 :   Pink cloth
    Pixel((unsigned char)0xde, (unsigned char)0x32,
          (unsigned char)0x88, (unsigned char)0xff),
    Pixel((unsigned char)0xde, (unsigned char)0x32,
          (unsigned char)0x88, (unsigned char)0xff)
  },
  { // 0x22 :   Black cloth
    Pixel((unsigned char)0x4d, (unsigned char)0x4d,
          (unsigned char)0x4d, (unsigned char)0xff),
    Pixel((unsigned char)0x4d, (unsigned char)0x4d,
          (unsigned char)0x4d, (unsigned char)0xff)
  },
  { // 0x23 : Gray cloth
    Pixel((unsigned char)0xde, (unsigned char)0xde,
          (unsigned char)0xde, (unsigned char)0xff),
    Pixel((unsigned char)0xde, (unsigned char)0xde,
          (unsigned char)0xde, (unsigned char)0xff)
  },
  { // 0x24 :   White cloth
    Pixel((unsigned char)0xde, (unsigned char)0xde,
          (unsigned char)0xde, (unsigned char)0xff),
    Pixel((unsigned char)0xde, (unsigned char)0xde,
          (unsigned char)0xde, (unsigned char)0xff)
  },
  { // 0x25 : Yellow flower
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0x00, (unsigned char)0x1f),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0x00, (unsigned char)0x1f)
  },
  { // 0x26 : Red rose
    Pixel((unsigned char)0xff, (unsigned char)0x1f,
          (unsigned char)0x10, (unsigned char)0x1f),
    Pixel((unsigned char)0xff, (unsigned char)0x1f,
          (unsigned char)0x10, (unsigned char)0x1f)
  },
  { // 0x27 : Brown mushroom
    Pixel((unsigned char)0xff, (unsigned char)0x9f,
          (unsigned char)0x2f, (unsigned char)0x1f),
    Pixel((unsigned char)0xff, (unsigned char)0x9f,
          (unsigned char)0x2f, (unsigned char)0x1f)
  },
  { // 0x28 : Red mushroom
    Pixel((unsigned char)0xff, (unsigned char)0x3f,
          (unsigned char)0x3f, (unsigned char)0x1f),
    Pixel((unsigned char)0xff, (unsigned char)0x3f,
          (unsigned char)0x3f, (unsigned char)0x1f)
  },
  { // 0x29 : Gold Block
    Pixel((unsigned char)0xef, (unsigned char)0xff,
          (unsigned char)0x2f, (unsigned char)0xff),
    Pixel((unsigned char)0xef, (unsigned char)0xff,
          (unsigned char)0x2f, (unsigned char)0xff)
  },
  { // 0x2a : Iron Block
    Pixel((unsigned char)0xbf, (unsigned char)0xbf,
          (unsigned char)0xbf, (unsigned char)0xff),
    Pixel((unsigned char)0xbf, (unsigned char)0xbf,
          (unsigned char)0xbf, (unsigned char)0xff)
  },
  { // 0x2b : Double step
    Pixel((unsigned char)0xc8, (unsigned char)0xc8,
          (unsigned char)0xc8, (unsigned char)0xff),
    Pixel((unsigned char)0xc8, (unsigned char)0xc8,
          (unsigned char)0xc8, (unsigned char)0xff)
  },
  { // 0x2c : Step
    Pixel((unsigned char)0xc8, (unsigned char)0xc8,
          (unsigned char)0xc8, (unsigned char)0xff),
    Pixel((unsigned char)0xc8, (unsigned char)0xc8,
          (unsigned char)0xc8, (unsigned char)0xff)
  },
  { // 0x2d : Brick
    Pixel((unsigned char)0xaa, (unsigned char)0x56,
          (unsigned char)0x3e, (unsigned char)0xff),
    Pixel((unsigned char)0xaa, (unsigned char)0x56,
          (unsigned char)0x3e, (unsigned char)0xff)
  },
  { // 0x2e : TNT
    Pixel((unsigned char)0xa0, (unsigned char)0x53,
          (unsigned char)0x41, (unsigned char)0xff),
    Pixel((unsigned char)0xa0, (unsigned char)0x53,
          (unsigned char)0x41, (unsigned char)0xff)
  },
  { // 0x2f : Bookcase
    Pixel((unsigned char)0x9f, (unsigned char)0x5f,
          (unsigned char)0x1f, (unsigned char)0xff),
    Pixel((unsigned char)0x9f, (unsigned char)0x5f,
          (unsigned char)0x1f, (unsigned char)0xff)
  },
  { // 0x30 : Mossy cobblestone
    Pixel((unsigned char)0x6f, (unsigned char)0x7f,
          (unsigned char)0x6f, (unsigned char)0xff),
    Pixel((unsigned char)0x6f, (unsigned char)0x7f,
          (unsigned char)0x6f, (unsigned char)0xff)
  },
  { // 0x31 : Obsidian
    Pixel((unsigned char)0x1a, (unsigned char)0x0b,
          (unsigned char)0x2b, (unsigned char)0xff),
    Pixel((unsigned char)0x1a, (unsigned char)0x0b,
          (unsigned char)0x2b, (unsigned char)0xff)
  },
  { // 0x32 : Torch
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0x3f, (unsigned char)0x3f),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0x3f, (unsigned char)0x3f)
  },
  { // 0x33 : Fire
    Pixel((unsigned char)0xff, (unsigned char)0x2f,
          (unsigned char)0x2f, (unsigned char)0xcf),
    Pixel((unsigned char)0xff, (unsigned char)0x2f,
          (unsigned char)0x2f, (unsigned char)0xcf)
  },
  { // 0x34 : Mob spawner
    Pixel((unsigned char)0xdf, (unsigned char)0x4f,
          (unsigned char)0x2f, (unsigned char)0xff),
    Pixel((unsigned char)0xdf, (unsigned char)0x4f,
          (unsigned char)0x2f, (unsigned char)0xff)
  },
  { // 0x35 : Wooden stairs
    Pixel((unsigned char)0x6f, (unsigned char)0x4f,
          (unsigned char)0x2f, (unsigned char)0xff),
    Pixel((unsigned char)0x6f, (unsigned char)0x4f,
          (unsigned char)0x2f, (unsigned char)0xff)
  },
  { // 0x36 : Chest
    Pixel((unsigned char)0x7d, (unsigned char)0x5b,
          (unsigned char)0x26, (unsigned char)0xff),
    Pixel((unsigned char)0x7d, (unsigned char)0x5b,
          (unsigned char)0x26, (unsigned char)0xff)
  },
  { // 0x37 : Redstone wire
    Pixel((unsigned char)0x9f, (unsigned char)0x1f,
          (unsigned char)0x1f, (unsigned char)0x2f),
    Pixel((unsigned char)0x9f, (unsigned char)0x1f,
          (unsigned char)0x1f, (unsigned char)0x2f)
  },
  { // 0x38 : Diamond ore
    Pixel((unsigned char)0x81, (unsigned char)0x8c,
          (unsigned char)0x8f, (unsigned char)0xff),
    Pixel((unsigned char)0x81, (unsigned char)0x8c,
          (unsigned char)0x8f, (unsigned char)0xff)
  },
  { // 0x39 : Diamond block
    Pixel((unsigned char)0x2d, (unsigned char)0xa6,
          (unsigned char)0x98, (unsigned char)0xff),
    Pixel((unsigned char)0x2d, (unsigned char)0xa6,
          (unsigned char)0x98, (unsigned char)0xff)
  },
  { // 0x3a : Workbench
    Pixel((unsigned char)0x72, (unsigned char)0x58,
          (unsigned char)0x38, (unsigned char)0xff),
    Pixel((unsigned char)0x72, (unsigned char)0x58,
          (unsigned char)0x38, (unsigned char)0xff)
  },
  { // 0x3b : Crops
    Pixel((unsigned char)0x92, (unsigned char)0xc0,
          (unsigned char)0x00, (unsigned char)0xff),
    Pixel((unsigned char)0x92, (unsigned char)0xc0,
          (unsigned char)0x00, (unsigned char)0xff)
  },
  { // 0x3c : Soil
    Pixel((unsigned char)0x5f, (unsigned char)0x3a,
          (unsigned char)0x1e, (unsigned char)0xff),
    Pixel((unsigned char)0x5f, (unsigned char)0x3a,
          (unsigned char)0x1e, (unsigned char)0xff)
  },
  { // 0x3d : Furnace
    Pixel((unsigned char)0x60, (unsigned char)0x60,
          (unsigned char)0x60, (unsigned char)0xff),
    Pixel((unsigned char)0x60, (unsigned char)0x60,
          (unsigned char)0x60, (unsigned char)0xff)
  },
  { // 0x3e : Burning furnace
    Pixel((unsigned char)0x60, (unsigned char)0x60,
          (unsigned char)0x60, (unsigned char)0xff),
    Pixel((unsigned char)0x60, (unsigned char)0x60,
          (unsigned char)0x60, (unsigned char)0xff)
  },
  { // 0x3f : Sign post
    Pixel((unsigned char)0x6f, (unsigned char)0x5b,
          (unsigned char)0x36, (unsigned char)0xff),
    Pixel((unsigned char)0x6f, (unsigned char)0x5b,
          (unsigned char)0x36, (unsigned char)0xff)
  },
  { // 0x40 : Wooden door
    Pixel((unsigned char)0x5f, (unsigned char)0x2f,
          (unsigned char)0x1f, (unsigned char)0xff),
    Pixel((unsigned char)0x5f, (unsigned char)0x2f,
          (unsigned char)0x1f, (unsigned char)0xff)
  },
  { // 0x41 : Ladder
    Pixel((unsigned char)0x6f, (unsigned char)0x4f,
          (unsigned char)0x1f, (unsigned char)0x2f),
    Pixel((unsigned char)0x6f, (unsigned char)0x4f,
          (unsigned char)0x1f, (unsigned char)0x2f)
  },
  { // 0x42 : Minecart tracks
    Pixel((unsigned char)0x7f, (unsigned char)0x7f,
          (unsigned char)0x6f, (unsigned char)0x2f),
    Pixel((unsigned char)0x7f, (unsigned char)0x7f,
          (unsigned char)0x6f, (unsigned char)0x2f)
  },
  { // 0x43 : Cobblestone stairs
    Pixel((unsigned char)0x73, (unsigned char)0x73,
          (unsigned char)0x73, (unsigned char)0xff),
    Pixel((unsigned char)0x73, (unsigned char)0x73,
          (unsigned char)0x73, (unsigned char)0xff)
  },
  { // 0x44 : Wall sign
    Pixel((unsigned char)0x6f, (unsigned char)0x5b,
          (unsigned char)0x36, (unsigned char)0xff),
    Pixel((unsigned char)0x6f, (unsigned char)0x5b,
          (unsigned char)0x36, (unsigned char)0xff)
  },
  { // 0x45 : Lever
    Pixel((unsigned char)0x8f, (unsigned char)0x8f,
          (unsigned char)0x7f, (unsigned char)0x0f),
    Pixel((unsigned char)0x8f, (unsigned char)0x8f,
          (unsigned char)0x7f, (unsigned char)0x0f)
  },
  { // 0x46 : Stone pressure plate
    Pixel((unsigned char)0x7f, (unsigned char)0x7f,
          (unsigned char)0x7f, (unsigned char)0x7f),
    Pixel((unsigned char)0x7f, (unsigned char)0x7f,
          (unsigned char)0x7f, (unsigned char)0x4f)
  },
  { // 0x47 : Iron door
    Pixel((unsigned char)0xbf, (unsigned char)0xbf,
          (unsigned char)0xbf, (unsigned char)0xff),
    Pixel((unsigned char)0xbf, (unsigned char)0xbf,
          (unsigned char)0xbf, (unsigned char)0xff)
  },
  { // 0x48 : Wooden pressure plate
    Pixel((unsigned char)0x6f, (unsigned char)0x4f,
          (unsigned char)0x1f, (unsigned char)0x7f),
    Pixel((unsigned char)0x6f, (unsigned char)0x4f,
          (unsigned char)0x1f, (unsigned char)0x4f)
  },
  { // 0x49 : Redstone ore
    Pixel((unsigned char)0x83, (unsigned char)0x6b,
          (unsigned char)0x6b, (unsigned char)0xff),
    Pixel((unsigned char)0x83, (unsigned char)0x6b,
          (unsigned char)0x6b, (unsigned char)0xff)
  },
  { // 0x4a : Glowing redstone ore
    Pixel((unsigned char)0x83, (unsigned char)0x6b,
          (unsigned char)0x6b, (unsigned char)0xff),
    Pixel((unsigned char)0x83, (unsigned char)0x6b,
          (unsigned char)0x6b, (unsigned char)0xff)
  },
  { // 0x4b : Redstone torch, off
    Pixel((unsigned char)0x9f, (unsigned char)0x1f,
          (unsigned char)0x1f, (unsigned char)0x1f),
    Pixel((unsigned char)0x9f, (unsigned char)0x1f,
          (unsigned char)0x1f, (unsigned char)0x1f)
  },
  { // 0x4c : Redstone torch, on
    Pixel((unsigned char)0xff, (unsigned char)0x00,
          (unsigned char)0x00, (unsigned char)0xcf),
    Pixel((unsigned char)0xff, (unsigned char)0x00,
          (unsigned char)0x00, (unsigned char)0xcf)
  },
  { // 0x4d : Stone button
    Pixel((unsigned char)0xfa, (unsigned char)0xfa,
          (unsigned char)0xfa, (unsigned char)0x0f),
    Pixel((unsigned char)0xfa, (unsigned char)0xfa,
          (unsigned char)0xfa, (unsigned char)0x0f)
  },
  { // 0x4e : Snow
    Pixel((unsigned char)0xef, (unsigned char)0xef,
          (unsigned char)0xef, (unsigned char)0xff),
    Pixel((unsigned char)0xef, (unsigned char)0xef,
          (unsigned char)0xef, (unsigned char)0xff)
  },
  { // 0x4f : Ice
    Pixel((unsigned char)0x8f, (unsigned char)0xdf,
          (unsigned char)0xff, (unsigned char)0x4f),
    Pixel((unsigned char)0x8f, (unsigned char)0xdf,
          (unsigned char)0xff, (unsigned char)0x4f)
  },
  { // 0x50 : Snow block
    Pixel((unsigned char)0xfa, (unsigned char)0xfa,
          (unsigned char)0xfa, (unsigned char)0xff),
    Pixel((unsigned char)0xfa, (unsigned char)0xfa,
          (unsigned char)0xfa, (unsigned char)0xff)
  },
  { // 0x51 : Cactus
    Pixel((unsigned char)0x2f, (unsigned char)0xcf,
          (unsigned char)0x2f, (unsigned char)0xff),
    Pixel((unsigned char)0x2f, (unsigned char)0xcf,
          (unsigned char)0x2f, (unsigned char)0xff)
  },
  { // 0x52 : Clay
    Pixel((unsigned char)0x9f, (unsigned char)0x9f,
          (unsigned char)0xcf, (unsigned char)0xff),
    Pixel((unsigned char)0x9f, (unsigned char)0x9f,
          (unsigned char)0xcf, (unsigned char)0xff)
  },
  { // 0x53 : Reed
    Pixel((unsigned char)0x7f, (unsigned char)0xff,
          (unsigned char)0x7f, (unsigned char)0x1f),
    Pixel((unsigned char)0x7f, (unsigned char)0xff,
          (unsigned char)0x7f, (unsigned char)0x1f)
  },
  { // 0x54 : Jukebox
    Pixel((unsigned char)0x9f, (unsigned char)0x5f,
          (unsigned char)0x1f, (unsigned char)0xff),
    Pixel((unsigned char)0x9f, (unsigned char)0x5f,
          (unsigned char)0x1f, (unsigned char)0xff)
  },
  { // 0x55 : Fence
    Pixel((unsigned char)0x7f, (unsigned char)0x5f,
          (unsigned char)0x2f, (unsigned char)0x4f),
    Pixel((unsigned char)0x7f, (unsigned char)0x5f,
          (unsigned char)0x2f, (unsigned char)0x4f)
  },
  { // 0x56 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x57 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x58 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x59 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x5a :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x5b :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x5c :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x5d :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x5e :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x5f :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x60 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x61 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x62 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x63 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x64 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x65 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x66 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x67 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x68 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x69 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x6a :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x6b :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x6c :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x6d :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x6e :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x6f :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x70 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x71 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x72 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x73 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x74 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x75 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x76 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x77 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x78 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x79 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x7a :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x7b :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x7c :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x7d :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x7e :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x7f :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x80 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x81 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x82 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x83 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x84 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x85 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x86 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x87 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x88 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x89 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x8a :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x8b :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x8c :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x8d :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x8e :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x8f :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x90 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x91 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x92 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x93 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x94 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x95 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x96 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x97 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x98 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x99 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x9a :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x9b :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x9c :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x9d :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x9e :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0x9f :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xa0 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xa1 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xa2 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xa3 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xa4 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xa5 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xa6 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xa7 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xa8 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xa9 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xaa :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xab :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xac :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xad :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xae :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xaf :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xb0 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xb1 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xb2 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xb3 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xb4 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xb5 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xb6 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xb7 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xb8 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xb9 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xba :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xbb :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xbc :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xbd :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xbe :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xbf :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xc0 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xc1 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xc2 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xc3 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xc4 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xc5 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xc6 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xc7 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xc8 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xc9 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xca :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xcb :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xcc :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xcd :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xce :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xcf :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xd0 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xd1 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xd2 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xd3 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xd4 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xd5 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xd6 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xd7 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xd8 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xd9 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xda :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xdb :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xdc :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xdd :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xde :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xdf :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xe0 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xe1 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xe2 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xe3 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xe4 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xe5 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xe6 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xe7 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xe8 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xe9 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xea :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xeb :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xec :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xed :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xee :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xef :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xf0 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xf1 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xf2 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xf3 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xf4 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xf5 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xf6 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xf7 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xf8 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xf9 :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xfa :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xfb :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xfc :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xfd :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xfe :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  },
  { // 0xff :
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00),
    Pixel((unsigned char)0xff, (unsigned char)0xff,
          (unsigned char)0xff, (unsigned char)0x00)
  }
};
