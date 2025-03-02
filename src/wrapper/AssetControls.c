#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "AssetControls.h"
#include "errHandler.h"

static SDL_Texture* textureList[50];  // Increase if needed
static int textureListLen;

static Mix_Chunk* chunkList[9];
static int chunkListLen;

static Mix_Music* musicList[4];
static int musicListLen;

SDL_Texture* renderTexture(char* filePath, SDL_Renderer* renderer, bool isIMG) {
  SDL_Surface* surface = NULL;
  if (isIMG) {
    surface = IMG_Load(filePath);
  } else {
    surface = SDL_LoadBMP(filePath);
  }
  if (surface == NULL) {
    catchError(false, __FILE__);
    surface = SDL_LoadBMP("./nofile.bmp");
    if (surface == NULL) {
      catchError(false, __FILE__);
      return 0;
    }
  }

  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == NULL) {
    catchError(false, __FILE__);
    return 0;
  }

  SDL_FreeSurface(surface);

  textureList[textureListLen] = texture;
  textureListLen++;

  return texture;
}

SDL_Texture* renderText(char* text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer) {
  SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
  if (surface == NULL) {
    catchError(false, __FILE__);
    return 0;
  }
  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  if (texture == NULL) {
    catchError(false, __FILE__);
    return 0;
  }

  SDL_FreeSurface(surface);

  textureList[textureListLen] = texture;
  textureListLen++;

  return texture;
}

Mix_Chunk* loadSound(char* filePath) {
  Mix_Chunk* sound = Mix_LoadWAV(filePath);
  if (sound == NULL) {
    catchError(false, __FILE__);
  }

  chunkList[chunkListLen] = sound;
  chunkListLen++;

  return sound;
}

Mix_Music* loadMusic(char* filePath) {
  Mix_Music* music = Mix_LoadMUS(filePath);
  if (music == NULL) {
    catchError(false, __FILE__);
  }

  musicList[musicListLen] = music;
  musicListLen++;

  return music;
}

void textureCleanup() {
  for (int i = 0; i < textureListLen; i++) {
    SDL_DestroyTexture(textureList[i]);
  }
}
void chunkCleanup() {
  for (int i = 0; i < chunkListLen; i++) {
    Mix_FreeChunk(chunkList[i]);
  }
}
void musicCleanup() {
  for (int i = 0; i < musicListLen; i++) {
    Mix_FreeMusic(musicList[i]);
  }
}