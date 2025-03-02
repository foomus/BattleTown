#pragma once

#include <SDL2/SDL.h>

#define calcFPS(totalFrames) (float)(totalFrames) / (SDL_GetTicks() / 1000.0f)

int startTimer();

int getTimerTime();

int pauseTimer();

int resumeTimer();