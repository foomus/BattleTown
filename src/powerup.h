#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "wrapper/vartypes.h"

#define POWERUP_SPRITE_SIZE 16

enum POWERUP_TYPES {
  HELMET,
  TIMER,
  SHOVEL,
  STAR,
  GRENADE,
  TANK,
};

typedef struct {
  u16 posX, posY;
  u8 existsFor;
  u8 blinkCooldownCount;
  enum POWERUP_TYPES type;
} Powerup;

Powerup* createPowerup(enum POWERUP_TYPES type);

void destroyPowerup();

void setPowerupsTexture(SDL_Texture* texture);
void setPowerupSounds(Mix_Chunk* pwrupspawn);

Powerup* getPowerup();

void renderPowerup(SDL_Renderer* renderer);