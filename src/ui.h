#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

#include "wrapper/SDL_Controls.h"
#include "wrapper/vartypes.h"

#define UI_GAME_TEXT_COLOR \
  (SDL_Color) {            \
    0, 0, 0, 255           \
  }
#define UI_MENU_TEXT_COLOR \
  (SDL_Color) {            \
    245, 245, 245, 255     \
  }
#define UI_SELECTED_TEXT_COLOR \
  (SDL_Color) {                \
    220, 220, 0, 255           \
  }
#define GAME_OVER_COLOR \
  (SDL_Color) {         \
    200, 0, 0, 255      \
  }

#define FIRSTPLR_LIVES_X (FIELD_WIDTH + GRID_CELL_SIZE * 1.5)
#define FIRSTPLR_LIVES_Y (GRID_CELL_SIZE * 10)

#define ENEMIES_LEFT_X (FIELD_WIDTH + GRID_CELL_SIZE * 1.5)
#define ENEMIES_LEFT_Y (GRID_CELL_SIZE * 2)
#define STAGE_X (FIELD_WIDTH + GRID_CELL_SIZE * 1.5)
#define STAGE_Y (GRID_CELL_SIZE * 11.5)

enum SCENES {
  TITLE,
  STAGE,
  GAME,
  SCORE,
  GAME_OVER,
};

enum COLORS {
  WHITE = 0,
  BLACK = 1,
};

// SDL_Texture* updateScore(u16 score, TTF_Font* font, SDL_Rect* rect, SDL_Renderer* renderer);

void loadUIAssets(SDL_Renderer* renderer);

void renderNumber(u32 num, u8 charSpace, SDL_Point pos, bool isWhite, SDL_Renderer* renderer);

enum SCENES getScene();
void setScene(enum SCENES scene);
void setDestroyedTanks(u8* destroyedTanksList);
void setTanksTexture_UI(SDL_Texture* texture);

void addScore(u32 score);
void resetScore();

void updateLives_UI(u8 lives, SDL_Renderer* renderer);
void updateEnemiesLeft_UI(u8 enemiesLeft, SDL_Renderer* renderer);
void updateStage_UI(u8 stage, SDL_Renderer* renderer);
void updateStartButton_UI(SDL_Renderer* renderer);
void updateStageSelect_UI(SDL_Renderer* renderer, u8 stage);

void updateGlobalScore_UI(SDL_Renderer* renderer, u8 stage);
void updateStageScore_UI(SDL_Renderer* renderer);

void gameOver(TTF_Font* font, SDL_Renderer* renderer, u16 windowsWidth, u16 windowHeight);