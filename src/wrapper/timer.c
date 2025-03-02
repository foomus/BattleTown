#include <SDL2/SDL.h>
#include <stdbool.h>

static int startTime, pauseTime, pausedForMs;
static bool isPaused;

int startTimer() {
  int ms = SDL_GetTicks() - startTime - pausedForMs;
  startTime = SDL_GetTicks();
  pausedForMs = 0;
  return ms;
}

int getTimerTime() {
  return SDL_GetTicks() - startTime - pausedForMs;
}

int pauseTimer() {
  int ms = SDL_GetTicks() - startTime - pausedForMs;
  pauseTime = SDL_GetTicks();
  return ms;
}

int resumeTimer() {
  pausedForMs += SDL_GetTicks() - pauseTime;
  return pausedForMs;
}