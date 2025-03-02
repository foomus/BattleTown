#include <stdbool.h>
#include <stdio.h>

#include <SDL2/SDL.h>

#include "SDL_Controls.h"
#include "errHandler.h"

void catchError(bool fatal, char* fileName) {
  char prefix = (fatal == true ? 'F' : ' ');
  fprintf(stderr, "%cSDL error: %s in file %s\n", prefix, SDL_GetError(),
          fileName);  // TODO: remove line and file name on release
  if (fatal) {
    SDL_Delay(3000);
    SDL_Shutdown();
    exit(EXIT_FAILURE);
  }
}