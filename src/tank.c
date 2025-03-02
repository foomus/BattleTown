#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "blocks.h"
#include "bullet.h"
#include "explosions.h"
#include "powerup.h"
#include "tank.h"
#include "ui.h"
#include "wrapper/SDL_Controls.h"
#include "wrapper/vartypes.h"

static Tank* tankList[50];
static u8 tankListLen = 0;
static u8 totalDestroyedTanks = 0;
static u8 destroyedTanksList[4];

static SDL_Texture *enemyTexture, *purpleEnemyTexture, *shieldTexture;

static u16 freezedCount = FREEZE_DURATION;
static u16 shieldedCount = SHIELD_DURATION;
static u16 shieldedBaseCount = BASE_SHIELD_DURATION;
static u16 scoreDelayCount = SCORE_SCREEN_DELAY;

static s8 lives = 2;

static Mix_Chunk *enemyDeathSound, *enemyDamagedSound, *levelUpSound, *playerDeathSound,
    *playerFireSound, *powerupCollectSound;
static Mix_Music *playerIdlingMusic, *playerMovingMusic;

Tank* createTank(u16 posX,
                 u16 posY,
                 u8 speed,
                 s8 armor,
                 SDL_Texture* sprite,
                 u8 type,
                 enum DIRECTION direction,
                 Bullet* bullet,
                 Bullet* bullet2,
                 bool isPlayer,
                 bool dropsPowerup,
                 bool addToList) {
  Tank* tank = malloc(sizeof(Tank));
  EnemySensors* enemyInfo = malloc(sizeof(EnemySensors));
  if (isPlayer == false) {
    sprite = enemyTexture;
    *enemyInfo = (EnemySensors){posX - 1, posY - 1, false, false, 31};
  }
  *tank = (Tank){posX,   posY,    0,        0, speed,     armor, sprite,       0, type, direction,
                 bullet, bullet2, isPlayer, 0, enemyInfo, false, dropsPowerup, 0};
  if (addToList) {
    tankList[tankListLen] = tank;
    tankListLen++;
  }
  return tank;
}

Tank* deleteTank(Bullet* bullet, EnemySensors* sensors) {
  Tank* tank = malloc(sizeof(Tank));  // Add malloc success check
  if (tank == NULL) {
    puts("Malloc for deleted tank failed!");
  }
  *tank = (Tank){TANK_DEFAULT_POS,
                 TANK_DEFAULT_POS,
                 0,
                 0,
                 0,
                 0,
                 enemyTexture,
                 0,
                 BASIC,
                 UP,
                 bullet,
                 NULL,
                 false,
                 0,
                 sensors,
                 true,
                 0};
  totalDestroyedTanks++;
  return tank;
}

Tank* destroyTanks(bool createExplosion) {
  for (u16 i = 1; i < tankListLen; i++) {
    if (createExplosion) {
      createTExplosion(tankList[i]->posX, tankList[i]->posY);
    }
    if (tankList[i]->posX != TANK_DEFAULT_POS) {
      totalDestroyedTanks++;
    }
    free(tankList[i]);
    tankList[i] = malloc(sizeof(Tank));  // Add malloc success check
    *tankList[i] = (Tank){TANK_DEFAULT_POS,
                          TANK_DEFAULT_POS,
                          0,
                          0,
                          0,
                          0,
                          enemyTexture,
                          0,
                          BASIC,
                          UP,
                          tankList[i]->Bullet,
                          NULL,
                          false,
                          0,
                          tankList[i]->enemyInfo,
                          true,
                          0};
  }
}
void clearTanks(bool keepPlayer) {
  for (u16 i = keepPlayer; i < tankListLen; i++) {
    free(tankList[i]);
    tankList[i] = NULL;
  }
  tankListLen = keepPlayer;

  for (u8 i = 0; i < 4; i++) {
    destroyedTanksList[i] = 0;
  }
  totalDestroyedTanks = 0;
}

void setEnemiesTexture(SDL_Texture* texture, SDL_Texture* purpleTexture) {
  enemyTexture = texture, purpleEnemyTexture = purpleTexture;
}
void setShieldTexture(SDL_Texture* texture) {
  shieldTexture = texture;
}
void setTankSounds(Mix_Chunk* enemexplosion,
                   Mix_Chunk* damaged,
                   Mix_Chunk* levelup,
                   Mix_Chunk* plrexplosion,
                   Mix_Chunk* plrfire,
                   Mix_Chunk* pwrupcollect,
                   Mix_Music* idle,
                   Mix_Music* moving) {
  enemyDeathSound = enemexplosion;
  enemyDamagedSound = damaged;
  levelUpSound = levelup;
  playerDeathSound = plrexplosion;
  playerFireSound = plrfire;
  powerupCollectSound = pwrupcollect;
  playerIdlingMusic = idle;
  playerMovingMusic = moving;
}

s8 getLives() {
  return lives;
}
void setLives(u8 amount) {
  lives = amount;
}

void setTankDirection(Tank* tank, enum DIRECTION direction, bool keyDown) {
  if (tank->direction != direction) {
    tank->posX =
        (u16)round((double)tank->posX / (double)(GRID_CELL_SIZE / 2)) * (GRID_CELL_SIZE / 2);
    tank->posY =
        (u16)round((double)tank->posY / (double)(GRID_CELL_SIZE / 2)) * (GRID_CELL_SIZE / 2);
  }
  if (keyDown) {
    if (tank->isPlayer) {
      if (!(bool)Mix_PlayingMusic || (tank->velX == 0 && tank->velY == 0)) {
        Mix_PlayMusic(playerMovingMusic, 0);
      }
    }
    switch (direction) {
      case LEFT:
        tank->velX = -tank->speed;
        tank->velY = 0;
        tank->direction = LEFT;
        break;
      case RIGHT:
        tank->velX = tank->speed;
        tank->velY = 0;
        tank->direction = RIGHT;
        break;
      case UP:
        tank->velY = -tank->speed;
        tank->velX = 0;
        tank->direction = UP;
        break;
      case DOWN:
        tank->velY = tank->speed;
        tank->velX = 0;
        tank->direction = DOWN;
        break;
      default:
        break;
    }

  } else {
    if (tank->isPlayer) {
      if (!(bool)Mix_PlayingMusic || (tank->velX != 0 || tank->velY != 0)) {
        Mix_PlayMusic(playerIdlingMusic, 0);
      }
    }
    switch (direction) {
      case LEFT:
      case RIGHT:
        tank->velX = 0;
        break;
      case UP:
      case DOWN:
        tank->velY = 0;
        break;
      default:
        break;
    }
  }
}

bool checkFieldCollisionX_Tank(Tank* tank) {
  bool collidingWithBordersX = true;

  if (tank->posX > FIELD_POS_X && tank->posX + GRID_CELL_SIZE < FIELD_END_X) {
    collidingWithBordersX = false;
  } else {
    if (tank->posX <= FIELD_POS_X && tank->velX > 0) {
      collidingWithBordersX = false;
    } else if (tank->posX + GRID_CELL_SIZE >= FIELD_END_X && tank->velX < 0) {
      collidingWithBordersX = false;
    }
  }
  return collidingWithBordersX;
}

bool checkFieldCollisionY_Tank(Tank* tank) {
  bool collidingWithBordersY = true;

  if (tank->posY > FIELD_POS_Y && tank->posY + GRID_CELL_SIZE < FIELD_END_Y) {
    collidingWithBordersY = false;
  } else {
    if (tank->posY <= FIELD_POS_Y && tank->velY > 0) {
      collidingWithBordersY = false;
    } else if (tank->posY + GRID_CELL_SIZE >= FIELD_END_Y && tank->velY < 0) {
      collidingWithBordersY = false;
    }
  }
  return collidingWithBordersY;
}

bool checkBlocksCollisionX_Tank(Tank* tank, Block** blockList, u16 blockListLen) {
  for (u16 i = 0; i < blockListLen; i++) {
    bool collidingBlockX = false;

    Block* block = blockList[i];

    if (block->type != BUSH && block->type != AIR && block->type != SPAWNER) {
      u16 left = block->posX, top = block->posY;
      u16 right = block->posX + block->size.x, down = block->posY + block->size.y;

      u16 leftT = tank->posX, topT = tank->posY;
      u16 rightT = tank->posX + GRID_CELL_SIZE, downT = tank->posY + GRID_CELL_SIZE;

      if (downT > top && topT < down) {
        if (rightT >= left && leftT <= right) {
          collidingBlockX = true;
          if ((abs(rightT - left) > abs(leftT - right) && tank->velX > 0) ||
              (abs(rightT - left) < abs(leftT - right) && tank->velX < 0)) {
            collidingBlockX = false;
          }
        }
      }
      if (collidingBlockX) {
        // printf("Block %d: posX %d posY %d sizeX %d sizeY %d\n", block->type, block->posX,
        //        block->posY, block->size.x, block->size.y);
        return true;
      }
    }
  }

  return false;
}

bool checkBlocksCollisionY_Tank(Tank* tank, Block** blockList, u16 blockListLen) {
  for (u16 i = 0; i < blockListLen; i++) {
    bool collidingBlockY = false;

    Block* block = blockList[i];

    if (block->type != BUSH && block->type != AIR && block->type != SPAWNER) {
      u16 left = block->posX, top = block->posY;
      u16 right = block->posX + block->size.x, down = block->posY + block->size.y;

      u16 leftT = tank->posX, topT = tank->posY;
      u16 rightT = tank->posX + GRID_CELL_SIZE, downT = tank->posY + GRID_CELL_SIZE;

      if (rightT > left && leftT < right) {
        if (downT >= top && topT <= down) {
          collidingBlockY = true;
          if ((abs(downT - top) > abs(topT - down) && tank->velY > 0) ||
              (abs(downT - top) < abs(topT - down) && tank->velY < 0)) {
            collidingBlockY = false;
          }
        }
      }
      if (collidingBlockY) {
        // printf("Block %d: posX %d posY %d sizeX %d sizeY %d\n", block->type, block->posX,
        //        block->posY, block->size.x, block->size.y);
        return true;
      }
    }
  }

  return false;
}

bool checkTanksCollisionX_Tank(Tank* tank) {
  if (tankListLen > 1) {
    for (u16 i = 0; i < tankListLen; i++) {
      bool collidingTankX = false;

      Tank* anotherTank = tankList[i];
      if (anotherTank->destroyed) {
        continue;
      }

      if (anotherTank->posX == tank->posX && anotherTank->posY == tank->posY &&
          anotherTank->isPlayer == tank->isPlayer)
        continue;

      u16 left = anotherTank->posX, top = anotherTank->posY;
      u16 right = anotherTank->posX + GRID_CELL_SIZE, down = anotherTank->posY + GRID_CELL_SIZE;

      u16 leftT = tank->posX, topT = tank->posY;
      u16 rightT = tank->posX + GRID_CELL_SIZE, downT = tank->posY + GRID_CELL_SIZE;

      if (downT > top && topT < down) {
        if (rightT >= left && leftT <= right) {
          collidingTankX = true;
          if ((abs(rightT - left) > abs(leftT - right) && tank->velX > 0) ||
              (abs(rightT - left) < abs(leftT - right) && tank->velX < 0)) {
            collidingTankX = false;
          }
        }
      }
      if (collidingTankX) {
        return true;
      }
    }
  }

  return false;
}

bool checkTanksCollisionY_Tank(Tank* tank) {
  if (tankListLen > 1) {
    for (u16 i = 0; i < tankListLen; i++) {
      bool collidingTankY = false;

      Tank* anotherTank = tankList[i];
      if (anotherTank->destroyed) {
        continue;
      }

      if (anotherTank->posX == tank->posX && anotherTank->posY == tank->posY &&
          anotherTank->isPlayer == tank->isPlayer)
        continue;

      u16 left = anotherTank->posX, top = anotherTank->posY;
      u16 right = anotherTank->posX + GRID_CELL_SIZE, down = anotherTank->posY + GRID_CELL_SIZE;

      u16 leftT = tank->posX, topT = tank->posY;
      u16 rightT = tank->posX + GRID_CELL_SIZE, downT = tank->posY + GRID_CELL_SIZE;

      if (rightT > left && leftT < right) {
        if (downT >= top && topT <= down) {
          collidingTankY = true;
          if ((abs(downT - top) > abs(topT - down) && tank->velY > 0) ||
              (abs(downT - top) < abs(topT - down) && tank->velY < 0)) {
            collidingTankY = false;
          }
        }
      }
      if (collidingTankY) {
        return true;
      }
    }
  }
  return false;
}

bool checkBulletsCollisionX_Tank(Tank* tank) {
  if (tankListLen > 1) {
    u8 offset = 0;
    for (u8 j = 0; j < tankListLen + 1; j++) {
      u16 i = j - offset;
      bool collidingBulletX = false;

      Bullet* bullet = tankList[i]->Bullet;

      if (j == 1) {
        bullet = tankList[0]->Bullet2;  // Checking collision of 2nd player bullet
        offset = 1;
        i = j - offset;
        if (tank->isPlayer) {
          continue;  // Continue if 2nd player bullet is colliding with player
        }
      }

      if (tankList[i]->destroyed) {
        continue;
      }

      if (bullet == tank->Bullet || (tank == tankList[0] && bullet == tankList[0]->Bullet2))
        continue;

      u16 left = bullet->posX, top = bullet->posY;
      u16 right = bullet->posX + BULLET_SIZE, down = bullet->posY + BULLET_SIZE;

      u16 leftT = tank->posX, topT = tank->posY;
      u16 rightT = tank->posX + GRID_CELL_SIZE, downT = tank->posY + GRID_CELL_SIZE;

      if (downT > top && topT < down) {
        if (rightT >= left && leftT <= right) {
          if (tank->isPlayer != tankList[i]->isPlayer) {
            collidingBulletX = true;
            ;
          }
        }
      }
      if (collidingBulletX) {
        resetBullet(bullet);
        return true;
      }
    }
  }

  return false;
}

bool checkBulletsCollisionY_Tank(Tank* tank) {
  if (tankListLen > 1) {
    u8 offset = 0;
    for (u8 j = 0; j < tankListLen; j++) {
      u16 i = j - offset;
      bool collidingBulletY = false;

      Bullet* bullet = tankList[i]->Bullet;

      if (j == 1) {
        bullet = tankList[0]->Bullet2;  // Checking collision of 2nd player bullet
        offset = 1;
        i = j - offset;
        if (tank->isPlayer) {
          continue;  // Continue if 2nd player bullet is colliding with player
        }
      }

      if (tankList[i]->destroyed) {
        continue;
      }

      if (bullet == tank->Bullet || (tank == tankList[0] && bullet == tankList[0]->Bullet2))
        continue;

      u16 left = bullet->posX, top = bullet->posY;
      u16 right = bullet->posX + BULLET_SIZE, down = bullet->posY + BULLET_SIZE;

      u16 leftT = tank->posX, topT = tank->posY;
      u16 rightT = tank->posX + GRID_CELL_SIZE, downT = tank->posY + GRID_CELL_SIZE;

      if (rightT > left && leftT < right) {
        if (downT >= top && topT <= down) {
          if (tank->isPlayer != tankList[i]->isPlayer) {
            collidingBulletY = true;
          }
        }
      }
      if (collidingBulletY) {
        resetBullet(bullet);
        return true;
      }
    }
  }
  return false;
}

bool checkPowerupCollisionX_Tank(Tank* tank) {
  Powerup* powerup = getPowerup();
  if (powerup != NULL) {
    for (u16 i = 0; i < tankListLen; i++) {
      bool collidingPowerupX = false;
      if (tankList[i]->destroyed) {
        continue;
      }
      u16 left = powerup->posX, top = powerup->posY;
      u16 right = powerup->posX + GRID_CELL_SIZE, down = powerup->posY + GRID_CELL_SIZE;

      u16 leftT = tank->posX, topT = tank->posY;
      u16 rightT = tank->posX + GRID_CELL_SIZE, downT = tank->posY + GRID_CELL_SIZE;

      if (downT > top && topT < down) {
        if (rightT >= left && leftT <= right) {
          collidingPowerupX = true;
        }
      }
      if (collidingPowerupX) {
        return true;
      }
    }
  }
  return false;
}

bool checkPowerupCollisionY_Tank(Tank* tank) {
  Powerup* powerup = getPowerup();
  if (powerup != NULL) {
    for (u16 i = 0; i < tankListLen; i++) {
      bool collidingPowerupY = false;
      if (tankList[i]->destroyed) {
        continue;
      }
      u16 left = powerup->posX, top = powerup->posY;
      u16 right = powerup->posX + GRID_CELL_SIZE, down = powerup->posY + GRID_CELL_SIZE;

      u16 leftT = tank->posX, topT = tank->posY;
      u16 rightT = tank->posX + GRID_CELL_SIZE, downT = tank->posY + GRID_CELL_SIZE;

      if (rightT > left && leftT < right) {
        if (downT >= top && topT <= down) {
          collidingPowerupY = true;
        }
      }
      if (collidingPowerupY) {
        return true;
      }
    }
  }
  return false;
}

void renderTanks(SDL_Renderer* renderer, Block** blockList, u16 blockListLen) {
  for (u16 i = 0; i < tankListLen; i++) {
    Tank* tank = tankList[i];

    if (tank->destroyed) {
      continue;
    }

    bool collidingWithBulletX = checkBulletsCollisionX_Tank(tank);
    bool collidingWithBulletY = checkBulletsCollisionY_Tank(tank);

    if (collidingWithBulletX || collidingWithBulletY) {
      if (!tank->isPlayer || lives <= 0) {
        tank->armor--;
        if (tank->armor != -1) {
          Mix_PlayChannel(-1, enemyDamagedSound, 0);
          continue;
        }
        createTExplosion(tank->posX, tank->posY);

        if (tank->dropsPowerup) {
          destroyPowerup();
          createPowerup(rand() % 5);
          puts("Tank dropped powerup!");
        };

        resetBullet(tank->Bullet);
        Bullet* emptyBullet = tank->Bullet;
        EnemySensors* emptySensors = tank->enemyInfo;

        if (shieldedCount >= SHIELD_DURATION) {
          Mix_PlayChannel(-1, playerDeathSound, 0);
          if (tank->isPlayer == true) {
            lives = -1;
            setScoreScreenTimer();
          } else {
            destroyedTanksList[tank->type]++;
          }
        }

        free(tankList[i]);
        tankList[i] = deleteTank(emptyBullet, emptySensors);
      } else {
        if (shieldedCount >= SHIELD_DURATION) {
          createTExplosion(tank->posX, tank->posY);
          Mix_PlayChannel(-1, playerDeathSound, 0);
          lives--;
          tank->posX = FIELD_POS_X + GRID_CELL_SIZE * 4,
          tank->posY = FIELD_POS_Y + GRID_CELL_SIZE * 12;
          shieldTank(tank);
          levelUpPlayer(tank, true);
        }
      }
    }

    if (tank->velX != 0 || tank->velY != 0) {
      bool collidingWithBordersX = checkFieldCollisionX_Tank(tank);
      bool collidingWithBordersY = checkFieldCollisionY_Tank(tank);

      bool collidingWithBlockX = checkBlocksCollisionX_Tank(tank, blockList, blockListLen);
      bool collidingWithBlockY = checkBlocksCollisionY_Tank(tank, blockList, blockListLen);

      bool collidingWithTankX = checkTanksCollisionX_Tank(tank);
      bool collidingWithTankY = checkTanksCollisionY_Tank(tank);

      bool collidingWithPowerupX = checkPowerupCollisionX_Tank(tank);
      bool collidingWithPowerupY = checkPowerupCollisionY_Tank(tank);

      if (!tank->isPlayer) {
        tankList[i]->enemyInfo->prevPosX = tank->posX;
        tankList[i]->enemyInfo->prevPosY = tank->posY;
      }

      if (!collidingWithBordersX && !collidingWithBlockX && !collidingWithTankX) {
        tank->posX += tank->velX;
        if (!tank->isPlayer)
          tankList[i]->enemyInfo->collidingX = false;
      } else {
        if (!tank->isPlayer)
          tankList[i]->enemyInfo->collidingX = true;
      }
      if (!collidingWithBordersY && !collidingWithBlockY && !collidingWithTankY) {
        tank->posY += tank->velY;
        if (!tank->isPlayer)
          tankList[i]->enemyInfo->collidingY = false;
      } else {
        if (!tank->isPlayer)
          tankList[i]->enemyInfo->collidingY = true;
      }

      if (collidingWithPowerupX || collidingWithPowerupY) {
        Powerup* powerup = getPowerup();
        addScore(500);
        if (tank->isPlayer) {
          switch (powerup->type) {
            case TANK:
              lives++;
              break;
            case GRENADE:
              destroyTanks(true);
              break;
            case TIMER:
              freezeTanks();
              break;
            case HELMET:
              shieldTank(tank);
              break;
            case STAR:
              levelUpPlayer(tank, false);
              break;
            case SHOVEL:
              shieldBase(blockList, blockListLen, true);
              break;
            default:
              break;
          }
          if (powerup->type != STAR) {
            Mix_PlayChannel(-1, powerupCollectSound, 0);
          } else {
            Mix_PlayChannel(-1, levelUpSound, 0);
          }
        }
        destroyPowerup();
      }
    }

    if (tank->shootCooldown < SHOOT_COOLDOWN) {
      tank->shootCooldown++;
    }

    SDL_Rect tankRect = {tank->posX, tank->posY, GRID_CELL_SIZE, GRID_CELL_SIZE};
    SDL_Rect spriteRect = {TANK_SPRITE_WIDTH * tank->spriteIndex, TANK_SPRITE_HEIGHT * tank->type,
                           TANK_SPRITE_WIDTH, TANK_SPRITE_HEIGHT};

    if (tank->dropsPowerup && tank->spriteIndex) {
      SDL_RenderCopyEx(
          renderer, purpleEnemyTexture, &spriteRect, &tankRect, (double)tank->direction,
          &(SDL_Point){GRID_CELL_SIZE / 2, GRID_CELL_SIZE / 2}, (SDL_RendererFlip)SDL_FLIP_NONE);
    } else {
      SDL_RenderCopyEx(
          renderer,
          freezedCount >= FREEZE_DURATION || tank->isPlayer ? tank->sprite : purpleEnemyTexture,
          &spriteRect, &tankRect, (double)tank->direction,
          &(SDL_Point){GRID_CELL_SIZE / 2, GRID_CELL_SIZE / 2}, (SDL_RendererFlip)SDL_FLIP_NONE);
    }
    if (tank->isPlayer == true) {
      if (shieldedCount < SHIELD_DURATION) {
        SDL_Rect shieldSpriteRect = {16 * tank->shieldSpriteIndex, 0, 16, TANK_SPRITE_HEIGHT};
        SDL_RenderCopy(renderer, shieldTexture, &shieldSpriteRect, &tankRect);
        if (tank->shieldSpriteIndex) {
          tank->shieldSpriteIndex = 0;
        } else {
          tank->shieldSpriteIndex++;
        }
        shieldedCount++;
      }
    }

    if (lives > -1) {
      updateLives_UI(lives, renderer);
    }
    if (tank->spriteIndex == 1) {
      tank->spriteIndex = 0;
    } else if (tank->velX != 0 || tank->velY != 0) {
      tank->spriteIndex = 1;
    }
  }
  if (scoreDelayCount != SCORE_SCREEN_DELAY) {
    if (scoreDelayCount == SCORE_SCREEN_DELAY - 1) {
      u32 score = 0;
      setDestroyedTanks(destroyedTanksList);

      for (u8 j = 0; j < 4; j++) {
        score += destroyedTanksList[j] * (100 * (j + 1));
      }
      addScore(score);

      Mix_HaltMusic();

      setScene(SCORE);
    }
    scoreDelayCount++;
  }
  if (shieldedBaseCount != BASE_SHIELD_DURATION) {
    if (shieldedBaseCount == 0) {
      shieldBase(blockList, blockListLen, false);
      shieldedBaseCount = BASE_SHIELD_DURATION;
    } else {
      shieldedBaseCount--;
    }
  }
}

bool shoot(Tank* tank) {
  Bullet* bullet = tank->Bullet;
  if (tank->destroyed) {
    return false;
  }
  if ((bullet->posX == BULLET_DEFAULT_POS &&
       (tank->shootCooldown == SHOOT_COOLDOWN || tank->type == POWER)) ||
      tank->isPlayer) {
    if (tank->isPlayer && bullet->posX != BULLET_DEFAULT_POS && tank->type >= 2) {
      if (tank->Bullet2->posX == BULLET_DEFAULT_POS) {
        bullet = tank->Bullet2;
      } else {
        return false;
      }
    } else if (tank->isPlayer && bullet->posX != BULLET_DEFAULT_POS && tank->type < 2) {
      return false;
    }
    bullet->posX = tank->posX + (GRID_CELL_SIZE - BULLET_SIZE) / 2;
    bullet->posY = tank->posY + (GRID_CELL_SIZE - BULLET_SIZE) / 2;
    bullet->velX = 0;
    bullet->velY = 0;
    switch (tank->direction) {
      case LEFT:
        bullet->posX -= GRID_CELL_SIZE / 4;
        bullet->velX = -bullet->speed;
        break;
      case RIGHT:
        bullet->posX += GRID_CELL_SIZE / 4;
        bullet->velX = bullet->speed;
        break;
      case UP:
        bullet->posY -= GRID_CELL_SIZE / 4;
        bullet->velY = -bullet->speed;
        break;
      case DOWN:
        bullet->posY += GRID_CELL_SIZE / 4;
        bullet->velY = bullet->speed;
        break;
      default:
        break;
    }
    bullet->direction = tank->direction;
    if (tank->isPlayer) {
      Mix_PlayChannel(-1, playerFireSound, 0);
    }
    return true;
  } else {
    return false;
  }
}

Tank** getTanksList(u8* tankListLength) {
  *tankListLength = tankListLen;
  return tankList;
}

void spawnEnemy(Block** blockList, u16 blockListLen, bool dropsPowerup) {
  Block* spawners[3];
  u8 spawnersLen = 0;
  for (u16 i = 0; i < blockListLen; i++) {
    Block* block = blockList[i];
    if (block->type == SPAWNER) {
      spawners[spawnersLen] = blockList[i];
      spawnersLen++;
    }
  }
  u8 selectedSpawner = rand() % 3;
  spawners[selectedSpawner]->spawnEnemies = true;
  if (dropsPowerup) {
    spawners[selectedSpawner]->dropsPowerup = true;
  }
}

void updateEnemies() {
  if (freezedCount >= FREEZE_DURATION) {
    for (u16 i = 0; i < tankListLen; i++) {
      Tank* tank = tankList[i];

      if (tank->destroyed) {
        continue;
      }
      u16 newDirection;
      u8 directionRand = rand() % 2;
      u16 veerOfDirectionRand = rand() % ENEMY_ROTATE_COOLDOWN;
      if (!tank->isPlayer) {
        // printf(
        //     "-----Tank%d-----\nposX=%d,\nposY=%d\nprevPosX=%d\nprevPosY=%d\ncollidingX=%d"
        //     "\ncollidingY=%d\n",
        //     i, tank->posX, tank->posY, tank->enemyInfo->prevPosX, tank->enemyInfo->prevPosY,
        //     tank->enemyInfo->collidingX, tank->enemyInfo->collidingY);
        if (tank->velX == 0 && tank->velY == 0) {  // Check is tank made first move
          newDirection = (rand() % 4) * 90;
          setTankDirection(tank, newDirection, true);
        } else {
          if (tank->enemyInfo->collidingX == false &&
              (tank->direction == UP || tank->direction == DOWN) && veerOfDirectionRand == 0) {
            switch (directionRand) {
              case 0:
                newDirection = LEFT;
                break;
              case 1:
                newDirection = RIGHT;
                break;
              default:
                break;
            }
            setTankDirection(tank, newDirection, true);
          } else if (tank->enemyInfo->collidingY == false &&
                     (tank->direction == LEFT || tank->direction == RIGHT) &&
                     veerOfDirectionRand == 0) {
            switch (directionRand) {
              case 0:
                newDirection = UP;
                break;
              case 1:
                newDirection = DOWN;
                break;
              default:
                break;
            }
            setTankDirection(tank, newDirection, true);
          } else {
            if (tank->velX != 0 && abs(tank->enemyInfo->prevPosX - tank->posX) < 1) {
              switch (directionRand) {
                case 0:
                  newDirection = UP;
                  break;
                case 1:
                  newDirection = DOWN;
                  break;
                default:
                  break;
              }
              setTankDirection(tank, newDirection, true);
            } else if (tank->velY != 0 && abs(tank->enemyInfo->prevPosY - tank->posY) < 1) {
              switch (directionRand) {
                case 0:
                  newDirection = LEFT;
                  break;
                case 1:
                  newDirection = RIGHT;
                  break;
                default:
                  break;
              }
              setTankDirection(tank, newDirection, true);
            }
          }
        }
        if (tank->Bullet->posX == BULLET_DEFAULT_POS) {  // Check is tank shooting
          if (shoot(tank)) {
            tankList[i]->shootCooldown = 0;
          }
        }
      }
    }
  } else {
    freezedCount++;
  }
}

void freezeTanks() {
  freezedCount = 0;
  for (u16 i = 0; i < tankListLen; i++) {
    tankList[i]->velX = 0;
    tankList[i]->velY = 0;
  }
}
void shieldTank(Tank* tank) {
  shieldedCount = 0;
  tank->shieldSpriteIndex = 0;
}
void shieldBase(Block** blockList, u16 blockListLen, bool steel) {
  for (u16 i = 0; i < blockListLen; i++) {
    Block* block = blockList[i];
    if (i == 148 || i == 149 || i == 150 || i == 161 || i == 163) {
      if (steel) {
        block->type = STEEL;
        block->size.x = GRID_CELL_SIZE;
        block->size.y = GRID_CELL_SIZE;
        switch (i) {
          case 148:
            block->posX = FIELD_POS_X + 5 * GRID_CELL_SIZE;
            block->posY = FIELD_POS_X + 11 * GRID_CELL_SIZE;
            break;
          case 149:
            block->posX = FIELD_POS_X + 6 * GRID_CELL_SIZE;
            block->posY = FIELD_POS_X + 11 * GRID_CELL_SIZE;
            break;
          case 150:
            block->posX = FIELD_POS_X + 7 * GRID_CELL_SIZE;
            block->posY = FIELD_POS_X + 11 * GRID_CELL_SIZE;
            break;
          case 161:
            block->posX = FIELD_POS_X + 5 * GRID_CELL_SIZE;
            block->posY = FIELD_POS_X + 12 * GRID_CELL_SIZE;
            break;
          case 163:
            block->posX = FIELD_POS_X + 7 * GRID_CELL_SIZE;
            block->posY = FIELD_POS_X + 12 * GRID_CELL_SIZE;
            break;
        }
        shieldedBaseCount--;
      } else {
        block->type = BRICK;
      }
    }
  }
}
void levelUpPlayer(Tank* tank, bool resetLevel) {
  // 0 index because this function will only apply to player. Player tank is always first on a list
  tank->type++;
  if (resetLevel)
    tank->type = 0;
  switch (tank->type) {
    case 0:
      tank->Bullet->speed = 16;
      tank->Bullet->breaksSteel = false;
      tank->Bullet2->breaksSteel = false;
      break;
    case 1:
      tank->Bullet->speed = 32;
      break;
    case 2:
      tank->Bullet->speed = 32;
      tank->Bullet2->speed = 32;
      break;
    case 3:
      tank->Bullet->breaksSteel = true;
      tank->Bullet2->breaksSteel = true;
      break;
    default:
      tank->type = 3;
      break;
  };
}
u8 getScreenEnemsNum() {
  return tankListLen - 1 - totalDestroyedTanks;
}

void setScoreScreenTimer() {
  scoreDelayCount = 0;
}