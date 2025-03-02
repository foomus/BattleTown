#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "powerup.h"
#include "wrapper/SDL_Controls.h"
#include "wrapper/vartypes.h"

static SDL_Texture* powerupsTexture;
static Powerup* currPowerup;

static Mix_Chunk* powerupSpawnSound;

Powerup* createPowerup(enum POWERUP_TYPES type) {
  Powerup* powerup = malloc(sizeof(Powerup));
  *powerup = (Powerup){FIELD_POS_X + rand() % (FIELD_WIDTH - GRID_CELL_SIZE),
                       FIELD_POS_Y + rand() % (FIELD_HEIGHT - GRID_CELL_SIZE), 0, 0, type};
  currPowerup = powerup;
  Mix_PlayChannel(-1, powerupSpawnSound, 0);
  return powerup;
}

void destroyPowerup() {
  free(currPowerup);
  currPowerup = NULL;
}

void setPowerupsTexture(SDL_Texture* texture) {
  powerupsTexture = texture;
}
void setPowerupSounds(Mix_Chunk* pwrupspawn) {
  powerupSpawnSound = pwrupspawn;
}

Powerup* getPowerup() {
  return currPowerup;
}

void renderPowerup(SDL_Renderer* renderer) {
  u8 offset = 0;
  if (currPowerup != NULL) {
    u8 blinkCooldown;
    if (currPowerup->existsFor < 63) {
      blinkCooldown = 12;
    } else if (currPowerup->existsFor < 127) {
      blinkCooldown = 8;
    } else if (currPowerup->existsFor < 195) {
      blinkCooldown = 4;
    } else {
      destroyPowerup();
      return;
    }

    if (currPowerup->blinkCooldownCount >= blinkCooldown) {
      offset = 6;
      currPowerup->blinkCooldownCount = 0;
    } else {
      currPowerup->blinkCooldownCount++;
    }
    currPowerup->existsFor++;
    SDL_Rect powerupRect = {currPowerup->posX, currPowerup->posY, GRID_CELL_SIZE, GRID_CELL_SIZE};
    SDL_Rect spriteRect = {(currPowerup->type + offset) * POWERUP_SPRITE_SIZE, 0,
                           POWERUP_SPRITE_SIZE, POWERUP_SPRITE_SIZE};
    SDL_RenderCopy(renderer, powerupsTexture, &spriteRect, &powerupRect);
  }
}