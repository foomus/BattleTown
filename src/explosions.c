#include <SDL2/SDL.h>

#include "explosions.h"
#include "wrapper/SDL_Controls.h"
#include "wrapper/vartypes.h"

static BExplosion* bExplosionList[30];
static u8 bExplosionListLen = 0;
static SDL_Texture* bExplosionTexture;

static TExplosion* tExplosionList[30];
static u8 tExplosionListLen = 0;
static SDL_Texture* tExplosionTexture;

void setExplosionsTexture(SDL_Texture* BTexture, SDL_Texture* TTexture) {
  bExplosionTexture = BTexture, tExplosionTexture = TTexture;
}

void createBExplosion(u16 posX, u16 posY) {
  BExplosion* bExplosion = malloc(sizeof(BExplosion));
  *bExplosion = (BExplosion){posX - GRID_CELL_SIZE / 4, posY - GRID_CELL_SIZE / 4, 0};
  if (bExplosionListLen >= 27) {
    clearBExplosions();
  }
  bExplosionList[bExplosionListLen] = bExplosion;
  bExplosionListLen++;
}

void createTExplosion(u16 posX, u16 posY) {
  TExplosion* tExplosion = malloc(sizeof(TExplosion));
  *tExplosion = (TExplosion){posX - GRID_CELL_SIZE / 2, posY - GRID_CELL_SIZE / 2, 0};
  if (tExplosionListLen >= 27) {
    clearTExplosions();
  }
  tExplosionList[tExplosionListLen] = tExplosion;
  tExplosionListLen++;
}

void renderExplosions(SDL_Renderer* renderer) {
  if (bExplosionListLen > 0) {
    for (u16 i = 0; i < bExplosionListLen; i++) {
      BExplosion* bExplosion = bExplosionList[i];

      SDL_Rect bExplosionRect = {bExplosion->posX, bExplosion->posY, GRID_CELL_SIZE,
                                 GRID_CELL_SIZE};
      SDL_Rect spriteRect = {bExplosion->spriteIndex * 16, 0, 16, 16};

      if (bExplosion->spriteIndex >= 2) {
        bExplosion->posX = BULLET_DEFAULT_POS;
      } else {
        bExplosion->spriteIndex++;
      }

      SDL_RenderCopy(renderer, bExplosionTexture, &spriteRect, &bExplosionRect);
    }
  }

  if (tExplosionListLen > 0) {
    for (u16 i = 0; i < tExplosionListLen; i++) {
      TExplosion* tExplosion = tExplosionList[i];

      SDL_Rect tExplosionRect = {tExplosion->posX, tExplosion->posY, GRID_CELL_SIZE * 2,
                                 GRID_CELL_SIZE * 2};
      SDL_Rect spriteRect = {tExplosion->spriteIndex * 32, 0, 32, 32};

      if (tExplosion->spriteIndex >= 5) {
        tExplosion->posX = BULLET_DEFAULT_POS;
      } else {
        tExplosion->spriteIndex++;
      }

      SDL_RenderCopy(renderer, tExplosionTexture, &spriteRect, &tExplosionRect);
    }
  }
}

void clearBExplosions() {
  for (u16 i = 0; i < bExplosionListLen; i++) {
    free(bExplosionList[i]);
    bExplosionList[i] = NULL;
  }
  bExplosionListLen = 0;
}

void clearTExplosions() {
  for (u16 i = 0; i < tExplosionListLen; i++) {
    free(tExplosionList[i]);
    tExplosionList[i] = NULL;
  }
  tExplosionListLen = 0;
}