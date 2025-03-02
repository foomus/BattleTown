#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "AssetControls.h"
#include "SDL_Controls.h"

void SDL_Shutdown() {
  textureCleanup();
  chunkCleanup();
  musicCleanup();
  Mix_CloseAudio();
  Mix_Quit();
  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}