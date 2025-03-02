#include <SDL2/SDL.h>
#include <stdbool.h>

#include "blocks.h"
#include "tank.h"
#include "wrapper/SDL_Controls.h"
#include "wrapper/vartypes.h"

static SDL_Texture* blocksTexture;
static Block* blockList[700];
static u16 blockListLen = 0;

static u8 enemyType;

Block* createBlock(u16 posX, u16 posY, enum BLOCK_TYPES type) {
  Block* block = malloc(sizeof(Block));
  *block =
      (Block){posX, posY, type, 0, 0, 0, (BlockSize){GRID_CELL_SIZE, GRID_CELL_SIZE}, false, false};
  blockList[blockListLen] = block;
  blockListLen++;
  return block;
}

Block* breakBlock(u16 posX, u16 posY, s16 angle, bool destroy, bool steel, BlockSize size) {
  Block* block = malloc(sizeof(Block));
  if (!destroy) {
    if (steel) {
      *block = (Block){posX, posY, HALFSTEEL, 0, 0, angle, size};
    } else {
      *block = (Block){posX, posY, HALFBRICK, 0, 0, angle, size};
    }

  } else {
    *block = (Block){posX, posY, AIR, 0, 0, angle, size};
  }

  return block;
}

void setBlockTexture(SDL_Texture* texture) {
  blocksTexture = texture;
}

void setEnemy(u8 type) {
  enemyType = type;
}

void renderBlocks(SDL_Renderer* renderer, u8 gridCellSize) {
  for (u16 i = 0; i < blockListLen; i++) {
    Block* block = blockList[i];

    if (block->type == POOL) {
      if (block->animDelay >= 16) {
        if (block->spriteIndex == 0) {
          block->spriteIndex = 1;
        } else {
          block->spriteIndex = 0;
        }
        block->animDelay = 0;
      } else {
        block->animDelay++;
      }
    }
    if (block->type == SPAWNER && block->spawnEnemies) {
      if (block->animDelay >= 3) {
        if (block->spriteIndex >= 3) {
          block->spriteIndex = 0;
          block->spawnEnemies = false;

          u8 speed, bulletSpeed, armor;
          switch (enemyType) {
            case BASIC:
              speed = 3 + 3 * 1, bulletSpeed = 8 + 8 * 1, armor = 0;
              break;
            case FAST:
              speed = 3 + 3 * 3, bulletSpeed = 8 + 8 * 2, armor = 0;
              break;
            case POWER:
              speed = 3 + 3 * 2, bulletSpeed = 8 + 8 * 3, armor = 0;
              break;
            case ARMORED:
              speed = 3 + 3 * 2, bulletSpeed = 8 + 8 * 2, armor = 3;
              break;
            default:
              break;
          }
          Bullet* bullet = createBullet(BULLET_DEFAULT_POS, BULLET_DEFAULT_POS, bulletSpeed);
          createTank(block->posX, block->posY, speed, armor, NULL, enemyType, DOWN, bullet, NULL,
                     false, block->dropsPowerup, true);
          block->dropsPowerup = false;
        } else {
          block->spriteIndex++;
        }
        block->animDelay = 0;
      } else {
        block->animDelay++;
      }
    }

    SDL_Rect blockRect = {block->posX, block->posY, gridCellSize, gridCellSize};
    SDL_Rect spriteRect = {
        (block->type + (block->type == POOL || SPAWNER ? block->spriteIndex : 0)) *
            BLOCK_SPRITE_SIZE,
        0, BLOCK_SPRITE_SIZE, BLOCK_SPRITE_SIZE};

    if (block->type == HALFBRICK || block->type == HALFSTEEL) {
      SDL_RenderCopyEx(renderer, blocksTexture, &spriteRect, &blockRect, block->angle,
                       &(SDL_Point){gridCellSize / 2, gridCellSize / 2},
                       (SDL_RendererFlip)SDL_FLIP_NONE);
    } else {
      SDL_RenderCopy(renderer, blocksTexture, &spriteRect, &blockRect);
    }
  }
}

Block** getBlockList(u16* blockListLength) {
  *blockListLength = blockListLen;
  return blockList;
}

void clearBlocks() {
  for (u16 i = 0; i < blockListLen; i++) {
    free(blockList[i]);
    blockList[i] = NULL;
  }
  blockListLen = 0;
}
