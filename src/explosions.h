#include <SDL2/SDL.h>

#include "wrapper/vartypes.h"

typedef struct {
  u16 posX, posY;
  u8 spriteIndex;
} BExplosion;
typedef struct {
  u16 posX, posY;
  u8 spriteIndex;
} TExplosion;

void setExplosionsTexture(SDL_Texture* BTexture, SDL_Texture* TTexture);

void createBExplosion(u16 posX, u16 posY);
void createTExplosion(u16 posX, u16 posY);

void renderExplosions(SDL_Renderer* renderer);

void clearBExplosions();
void clearTExplosions();