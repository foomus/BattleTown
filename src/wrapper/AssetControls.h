#pragma once

#include <stdbool.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#define getSpriteFromSheet(row, col, spriteWidth, spriteHeight)                  \
  &(SDL_Rect) {                                                                  \
    (spriteWidth) * (col), (spriteHeight) * (row), (spriteWidth), (spriteHeight) \
  }

SDL_Texture* renderTexture(char* filePath, SDL_Renderer* renderer, bool isIMG);
SDL_Texture* renderText(char* text, TTF_Font* font, SDL_Color color, SDL_Renderer* renderer);

Mix_Chunk* loadSound(char* filePath);
Mix_Music* loadMusic(char* filePath);

void textureCleanup();
void chunkCleanup();
void musicCleanup();
