#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>

#include "blocks.h"

#include "wrapper/vartypes.h"

typedef struct {
  u16 posX, posY;
  s8 velX, velY;
  s8 speed;
  u16 direction;
  bool breaksSteel;
} Bullet;

Bullet* createBullet(u16 posX, u16 posY, u8 speed);

void resetBullet(Bullet* bullet);

void setBulletTexture(SDL_Texture* texture);
void setBulletSounds(Mix_Chunk* bulcollision);

bool checkFieldCollisionX_Bullet(Bullet* bullet);
bool checkFieldCollisionY_Bullet(Bullet* bullet);

bool checkBlocksCollisionX_Bullet(Bullet* bullet, Block** blockList, u16 blockListLen);
bool checkBlocksCollisionY_Bullet(Bullet* bullet, Block** blockList, u16 blockListLen);

bool checkBulletsCollisionX_Bullet(Bullet* bullet);
bool checkBulletsCollisionY_Bullet(Bullet* bullet);

void renderBullet(SDL_Renderer* renderer, Block** blockList, u16 blockListLen);

void clearBullets(bool keepPlayer);