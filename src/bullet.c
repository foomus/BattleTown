#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#include "bullet.h"
#include "explosions.h"
#include "tank.h"
#include "wrapper/SDL_Controls.h"
#include "wrapper/vartypes.h"

static Bullet* bulletList[50];
static u8 bulletListLen = 0;
static SDL_Texture* bulletTexture;

static Mix_Chunk* bulletCollisionSound;

Bullet* createBullet(u16 posX, u16 posY, u8 speed) {
  Bullet* bullet = malloc(sizeof(Bullet));
  *bullet = (Bullet){posX, posY, (s8)0, (s8)0, speed, 180, false};
  bulletList[bulletListLen] = bullet;
  bulletListLen++;
  return bullet;
}

void resetBullet(Bullet* bullet) {
  bullet->posX = BULLET_DEFAULT_POS;
  bullet->posY = BULLET_DEFAULT_POS;
  bullet->velX = 0;
  bullet->velY = 0;
}

void setBulletTexture(SDL_Texture* texture) {
  bulletTexture = texture;
}
void setBulletSounds(Mix_Chunk* bulcollision) {
  bulletCollisionSound = bulcollision;
}

bool checkFieldCollisionX_Bullet(Bullet* bullet) {
  bool collidingWithBordersX = true;

  if (bullet->posX > FIELD_POS_X && bullet->posX + GRID_CELL_SIZE / 4 < FIELD_END_X) {
    collidingWithBordersX = false;
  } else {
    if (bullet->posX <= FIELD_POS_X && bullet->velX > 0) {
      collidingWithBordersX = false;
    } else if (bullet->posX + GRID_CELL_SIZE >= FIELD_END_X && bullet->velX < 0) {
      collidingWithBordersX = false;
    }
  }
  return collidingWithBordersX;
}

bool checkFieldCollisionY_Bullet(Bullet* bullet) {
  bool collidingWithBordersY = true;

  if (bullet->posY > FIELD_POS_Y && bullet->posY + GRID_CELL_SIZE / 4 < FIELD_END_Y) {
    collidingWithBordersY = false;
  } else {
    if (bullet->posY <= FIELD_POS_Y && bullet->velY > 0) {
      collidingWithBordersY = false;
    } else if (bullet->posY + GRID_CELL_SIZE >= FIELD_END_Y && bullet->velY < 0) {
      collidingWithBordersY = false;
    }
  }
  return collidingWithBordersY;
}

bool checkBlocksCollisionX_Bullet(Bullet* bullet, Block** blockList, u16 blockListLen) {
  for (u16 i = 0; i < blockListLen; i++) {
    bool collidingBlockX = false;

    Block* block = blockList[i];

    if (block->type != BUSH && block->type != POOL && block->type != AIR &&
        block->type != SPAWNER && (bullet->direction == LEFT || bullet->direction == RIGHT)) {
      u16 left = block->posX, top = block->posY;
      u16 right = block->posX + block->size.x, down = block->posY + block->size.y;

      u16 leftB = bullet->posX, topB = bullet->posY;
      u16 rightB = bullet->posX + BULLET_SIZE, downB = bullet->posY + BULLET_SIZE;

      if (downB > top && topB < down) {
        if (rightB >= left && leftB <= right) {
          collidingBlockX = true;
          Block newBlock = *block;

          if (block->type == BRICK || (bullet->breaksSteel && block->type == STEEL)) {
            if (bullet->direction == RIGHT) {
              newBlock.posX += GRID_CELL_SIZE / 2;
            }

            free(blockList[i]);
            blockList[i] = breakBlock(newBlock.posX, newBlock.posY, -90, false,
                                      newBlock.type == STEEL ? true : false,
                                      (BlockSize){GRID_CELL_SIZE / 2, GRID_CELL_SIZE});
          } else if (block->type == HALFBRICK ||
                     (bullet->breaksSteel && block->type == HALFSTEEL)) {
            free(blockList[i]);
            blockList[i] = breakBlock(newBlock.posX, newBlock.posY, 0, true,
                                      newBlock.type == HALFSTEEL ? true : false, (BlockSize){0, 0});
          }

          if (block->type == BASE) {
            blockList[i]->type++;
            setLives(-1);
            Mix_PlayChannel(-1, bulletCollisionSound, 0);
            setScoreScreenTimer();
          }
        }
      }
      if (collidingBlockX) {
        return true;
      }
    }
  }

  return false;
}

bool checkBlocksCollisionY_Bullet(Bullet* bullet, Block** blockList, u16 blockListLen) {
  for (u16 i = 0; i < blockListLen; i++) {
    bool collidingBlockY = false;

    Block* block = blockList[i];

    if (block->type != BUSH && block->type != POOL && block->type != AIR &&
        block->type != SPAWNER && (bullet->direction == UP || bullet->direction == DOWN)) {
      u16 left = block->posX, top = block->posY;
      u16 right = block->posX + block->size.x, down = block->posY + block->size.y;

      u16 leftB = bullet->posX, topB = bullet->posY;
      u16 rightB = bullet->posX + BULLET_SIZE, downB = bullet->posY + BULLET_SIZE;

      if (rightB > left && leftB < right) {
        if (downB >= top && topB <= down) {
          collidingBlockY = true;
          Block newBlock = *block;

          if (block->type == BRICK || (bullet->breaksSteel && block->type == STEEL)) {
            if (bullet->direction == DOWN) {
              newBlock.posY += GRID_CELL_SIZE / 2;
            }

            // free(blockList[i]);
            blockList[i] = breakBlock(newBlock.posX, newBlock.posY, 0, false,
                                      newBlock.type == STEEL ? true : false,
                                      (BlockSize){GRID_CELL_SIZE, GRID_CELL_SIZE / 2});
          } else if (block->type == HALFBRICK ||
                     (bullet->breaksSteel && block->type == HALFSTEEL)) {
            free(blockList[i]);
            blockList[i] = breakBlock(newBlock.posX, newBlock.posY, 0, true,
                                      newBlock.type == HALFSTEEL ? true : false, (BlockSize){0, 0});
          }
          if (block->type == BASE) {
            blockList[i]->type++;
            setLives(-1);
            Mix_PlayChannel(-1, bulletCollisionSound, 0);
            setScoreScreenTimer();
          }
        }
      }
      if (collidingBlockY) {
        return true;
      }
    }
  }

  return false;
}

bool checkBulletsCollisionX_Bullet(Bullet* bullet) {
  if (bulletListLen > 1) {
    for (u16 i = 0; i < bulletListLen; i++) {
      bool collidingBulletX = false;

      Bullet* anotherBullet = bulletList[i];

      if ((anotherBullet->posX == bullet->posX && anotherBullet->posY == bullet->posY) ||
          anotherBullet->posX == BULLET_DEFAULT_POS) {
        continue;
      }

      u16 left = anotherBullet->posX, top = anotherBullet->posY;
      u16 right = anotherBullet->posX + BULLET_SIZE, down = anotherBullet->posY + BULLET_SIZE;

      u16 leftB = bullet->posX, topB = bullet->posY;
      u16 rightB = bullet->posX + BULLET_SIZE, downB = bullet->posY + BULLET_SIZE;

      if (downB > top && topB < down) {
        if (rightB >= left && leftB <= right) {
          collidingBulletX = true;
        }
        if (collidingBulletX) {
          resetBullet(anotherBullet);
          return true;
        }
      }
    }
  }

  return false;
}

bool checkBulletsCollisionY_Bullet(Bullet* bullet) {
  if (bulletListLen > 1) {
    for (u16 i = 0; i < bulletListLen; i++) {
      bool collidingBulletY = false;

      Bullet* anotherBullet = bulletList[i];

      if ((anotherBullet->posX == bullet->posX && anotherBullet->posY == bullet->posY) ||
          anotherBullet->posX == BULLET_DEFAULT_POS) {
        continue;
      }

      u16 left = anotherBullet->posX, top = anotherBullet->posY;
      u16 right = anotherBullet->posX + BULLET_SIZE, down = anotherBullet->posY + BULLET_SIZE;

      u16 leftB = bullet->posX, topB = bullet->posY;
      u16 rightB = bullet->posX + BULLET_SIZE, downB = bullet->posY + BULLET_SIZE;

      if (rightB > left && leftB < right) {
        if (downB >= top && topB <= down) {
          collidingBulletY = true;
        }
        if (collidingBulletY) {
          resetBullet(anotherBullet);
          return true;
        }
      }
    }
  }

  return false;
}

void renderBullet(SDL_Renderer* renderer, Block** blockList, u16 blockListLen) {
  for (u16 i = 0; i < bulletListLen; i++) {
    Bullet* bullet = bulletList[i];

    bullet->posX += bullet->velX;
    bullet->posY += bullet->velY;

    bool collidingWithBordersX = checkFieldCollisionX_Bullet(bullet);
    bool collidingWithBordersY = checkFieldCollisionY_Bullet(bullet);
    bool collidingWithBlocks =
        (checkBlocksCollisionX_Bullet(bullet, blockList, blockListLen)
             ? true
             : checkBlocksCollisionY_Bullet(bullet, blockList, blockListLen));
    bool collidingWithBulletX = checkBulletsCollisionX_Bullet(bullet);
    bool collidingWithBulletY = checkBulletsCollisionY_Bullet(bullet);
    // bool collidingWithTanks = checkTanksCollisionX_Bullet(bullet, )

    if (collidingWithBordersX || collidingWithBordersY || collidingWithBlocks ||
        collidingWithBulletX || collidingWithBulletY) {
      if (bullet->velX != 0 || bullet->velY != 0) {
        createBExplosion(bullet->posX, bullet->posY);
        if (i < 2) {
          Mix_PlayChannel(-1, bulletCollisionSound, 0);
        }
      }
      resetBullet(bullet);
    }

    SDL_Rect bulletRect = {bullet->posX, bullet->posY, BULLET_SIZE, BULLET_SIZE};

    SDL_RenderCopyEx(renderer, bulletTexture, NULL, &bulletRect, (double)bullet->direction,
                     &(SDL_Point){BULLET_SIZE / 2, BULLET_SIZE / 2},
                     (SDL_RendererFlip)SDL_FLIP_NONE);
  }
}

void clearBullets(bool keepPlayer) {
  for (u8 i = keepPlayer * 2; i < bulletListLen; i++) {
    free(bulletList[i]);
    bulletList[i] = NULL;
  }
  bulletListLen = keepPlayer * 2;
}