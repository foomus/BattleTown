#pragma once

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "blocks.h"
#include "bullet.h"
#include "wrapper/vartypes.h"

#define TANK_SPRITE_WIDTH 15
#define TANK_SPRITE_HEIGHT 16
#define SHOOT_COOLDOWN 15
#define ENEMY_ROTATE_COOLDOWN 7
#define ENEMY_SPAWN_COOLDOWN (MAX_FPS / UPDATE_RATE) * 2.25
#define FREEZE_DURATION (MAX_FPS / UPDATE_RATE) * 6
#define SHIELD_DURATION (MAX_FPS / UPDATE_RATE) * 6
#define BASE_SHIELD_DURATION (MAX_FPS / UPDATE_RATE) * 6
#define SCORE_SCREEN_DELAY (MAX_FPS / UPDATE_RATE) * 2.5

enum DIRECTION {
  LEFT = 270,
  RIGHT = 90,
  UP = 0,
  DOWN = 180,
};

enum ENEMY_TYPES {
  BASIC,
  FAST,
  POWER,
  ARMORED,
};

typedef struct {
  u16 prevPosX, prevPosY;
  bool collidingX, collidingY;
  u8 rotateCooldown;
} EnemySensors;

typedef struct {
  u16 posX, posY;
  s8 velX, velY;
  u8 speed;
  s8 armor;
  SDL_Texture* sprite;
  bool spriteIndex;
  enum ENEMY_TYPES type;
  enum DIRECTION direction;
  Bullet *Bullet, *Bullet2;
  bool isPlayer;
  u8 shootCooldown;
  EnemySensors* enemyInfo;
  bool destroyed;
  bool dropsPowerup;
  u8 shieldSpriteIndex;
} Tank;

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
                 bool addToList);
Tank* deleteTank(Bullet* bullet, EnemySensors* sensors);

Tank* destroyTanks(bool createExplosion);
void clearTanks(bool keepPlayer);

void setEnemiesTexture(SDL_Texture* texture, SDL_Texture* purpleTexture);
void setShieldTexture(SDL_Texture* texture);
void setTankSounds(Mix_Chunk* enemexplosion,
                   Mix_Chunk* damaged,
                   Mix_Chunk* levelup,
                   Mix_Chunk* plrexplosion,
                   Mix_Chunk* plrfire,
                   Mix_Chunk* pwrupcollect,
                   Mix_Music* idle,
                   Mix_Music* moving);

s8 getLives();
void setLives(u8 amount);

void setTankDirection(Tank* tank, enum DIRECTION direction, bool keyDown);

bool checkFieldCollisionX_Tank(Tank* tank);
bool checkFieldCollisionY_Tank(Tank* tank);

bool checkBlocksCollisionX_Tank(Tank* tank, Block** blockList, u16 blockListLen);
bool checkBlocksCollisionY_Tank(Tank* tank, Block** blockList, u16 blockListLen);

bool checkTanksCollisionX_Tank(Tank* tank);
bool checkTanksCollisionY_Tank(Tank* tank);

bool checkBulletsCollisionX_Tank(Tank* tank);
bool checkBulletsCollisionY_Tank(Tank* tank);

bool checkPowerupCollisionX_Tank(Tank* tank);
bool checkPowerupCollisionY_Tank(Tank* tank);

void renderTanks(SDL_Renderer* renderer, Block** blockList, u16 blockListLen);

bool shoot(Tank* tank);

Tank** getTanksList(u8* tankListLength);

void spawnEnemy(Block** blockList, u16 blockListLen, bool dropsPowerup);

void updateEnemies();

void freezeTanks();
void shieldTank(Tank* tank);
void shieldBase(Block** blockList, u16 blockListLen, bool steel);
void levelUpPlayer(Tank* tank, bool resetLevel);

void setScoreScreenTimer();

u8 getScreenEnemsNum();