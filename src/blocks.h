#pragma once

#include <SDL2/SDL.h>
#include <stdbool.h>

#include "wrapper/vartypes.h"

#define BLOCK_SPRITE_SIZE 16

enum BLOCK_TYPES {
  BRICK = 0,
  STEEL = 1,
  BUSH = 2,
  POOL = 3,
  HALFBRICK = 5,
  HALFSTEEL = 6,
  SPAWNER = 7,
  BASE = 11,
  AIR = 15,
};

typedef struct {
  u8 x, y;
} BlockSize;

typedef struct {
  u16 posX, posY;
  enum BLOCK_TYPES type;

  u8 animDelay;
  u8 spriteIndex;

  s16 angle;
  BlockSize size;
  bool spawnEnemies;
  bool dropsPowerup;
} Block;

Block* createBlock(u16 posX, u16 posY, enum BLOCK_TYPES type);

Block* breakBlock(u16 posX, u16 posY, s16 angle, bool destroy, bool steel, BlockSize size);

void setBlockTexture(SDL_Texture* texture);

void setEnemy(u8 type);

void renderBlocks(SDL_Renderer* renderer, u8 gridCellSize);

Block** getBlockList(u16* blockListLength);

void clearBlocks();