#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "stageReader.h"
#include "ui.h"
#include "wrapper/AssetControls.h"
#include "wrapper/SDL_Controls.h"
#include "wrapper/vartypes.h"

static TTF_Font* font32;

static SDL_Texture *plricon, *enmicon, *flag, *firstplr, *start32, *stage32[2], *pts32, *total32,
    *score32, *tankTexture;

static SDL_Texture* nums[2][10];

static enum SCENES gameScene = TITLE;

static u8* destroyedEnemsList;

static u32 gameScore = 0;

// SDL_Texture* updateScore(u16 score, TTF_Font* font, SDL_Rect* rect, SDL_Renderer* renderer) {
//   int scoreTextW, scoreTextH;
//   char scoreStr[5] = "\0";
//   sprintf(scoreStr, "%d", score);
//   SDL_Texture* scoreTexture = renderText(scoreStr, font, SCORE_COLOR, renderer);
//   TTF_SizeText(font, scoreStr, &scoreTextW, &scoreTextH);
//   *rect = (SDL_Rect){15, 15, scoreTextW, scoreTextH};
//   return scoreTexture;
// }

void loadUIAssets(SDL_Renderer* renderer) {
  font32 = TTF_OpenFont("./assets/fonts/PressStart.ttf", 32);

  for (u8 j = 0; j < 2; j++) {
    SDL_Color colors[2] = {UI_GAME_TEXT_COLOR, UI_MENU_TEXT_COLOR};
    for (u8 i = 0; i < 10; i++) {
      char buf[2];
      sprintf(buf, "%d", i);
      nums[j][i] = renderText(buf, font32, colors[j], renderer);
    }
  }

  start32 = renderText("START", font32, UI_SELECTED_TEXT_COLOR, renderer);
  stage32[BLACK] = renderText("STAGE", font32, UI_GAME_TEXT_COLOR, renderer);
  stage32[WHITE] = renderText("STAGE", font32, UI_MENU_TEXT_COLOR, renderer);
  pts32 = renderText("PTS", font32, UI_MENU_TEXT_COLOR, renderer);
  total32 = renderText("TOTAL", font32, UI_MENU_TEXT_COLOR, renderer);
  score32 = renderText("SCORE", font32, UI_MENU_TEXT_COLOR, renderer);

  firstplr = renderText("IP", font32, UI_GAME_TEXT_COLOR, renderer);

  plricon = renderTexture("./assets/sprites/plricon.png", renderer, true);
  enmicon = renderTexture("./assets/sprites/enmicon.png", renderer, true);
  flag = renderTexture("./assets/sprites/flag.png", renderer, true);
}

void renderNumber(u32 num, u8 charSpace, SDL_Point pos, bool isWhite, SDL_Renderer* renderer) {
  char numStr[7] = "\0";
  u8 numStrLen;

  sprintf(numStr, "%d", num);
  numStrLen = strlen(numStr);

  for (u8 i = 0; i < numStrLen; i++) {
    char buffer[2] = " \0";
    buffer[0] = numStr[i];

    SDL_Rect numRect = {pos.x + (32 + charSpace) * i, pos.y, 32, 32};
    u8 num = strtol(buffer, NULL, 0);
    SDL_RenderCopy(renderer, nums[isWhite][num], NULL, &numRect);
  }
}

enum SCENES getScene() {
  return gameScene;
}
void setScene(enum SCENES scene) {
  gameScene = scene;
}
void setDestroyedTanks(u8* destroyedTanksList) {
  destroyedEnemsList = destroyedTanksList;
}
void setTanksTexture_UI(SDL_Texture* texture) {
  tankTexture = texture;
}
void addScore(u32 score) {
  gameScore += score;
}
void resetScore() {
  gameScore = 0;
}

void updateLives_UI(u8 lives, SDL_Renderer* renderer) {
  SDL_Rect firstplrRect = {FIRSTPLR_LIVES_X, FIRSTPLR_LIVES_Y, GRID_CELL_SIZE, GRID_CELL_SIZE / 2};
  SDL_Rect firstplrIconRect = {FIRSTPLR_LIVES_X, FIRSTPLR_LIVES_Y + GRID_CELL_SIZE / 2,
                               GRID_CELL_SIZE / 2, GRID_CELL_SIZE / 2};
  SDL_Rect firstplrLivesRect = {FIRSTPLR_LIVES_X + GRID_CELL_SIZE / 2,
                                FIRSTPLR_LIVES_Y + GRID_CELL_SIZE / 2, GRID_CELL_SIZE / 2,
                                GRID_CELL_SIZE / 2};

  renderNumber(lives, 0, (SDL_Point){firstplrLivesRect.x, firstplrLivesRect.y}, false, renderer);

  SDL_RenderCopy(renderer, firstplr, NULL, &firstplrRect);
  SDL_RenderCopy(renderer, plricon, NULL, &firstplrIconRect);
}

void updateEnemiesLeft_UI(u8 enemiesLeft, SDL_Renderer* renderer) {
  for (u16 i = 0; i < enemiesLeft; i++) {
    u8 row = i / 2, column = i - row * 2;
    SDL_Rect enmiconRect = {ENEMIES_LEFT_X + column * GRID_CELL_SIZE / 2,
                            ENEMIES_LEFT_Y + row * GRID_CELL_SIZE / 2, GRID_CELL_SIZE / 2,
                            GRID_CELL_SIZE / 2};
    SDL_RenderCopy(renderer, enmicon, NULL, &enmiconRect);
  }
}

void updateStage_UI(u8 stage, SDL_Renderer* renderer) {
  SDL_Rect flagRect = {STAGE_X, STAGE_Y, GRID_CELL_SIZE, GRID_CELL_SIZE};
  SDL_Rect stageRect = {STAGE_X + GRID_CELL_SIZE / 2, STAGE_Y + GRID_CELL_SIZE, GRID_CELL_SIZE / 2,
                        GRID_CELL_SIZE / 2};

  renderNumber(stage, 0, (SDL_Point){stageRect.x, stageRect.y}, false, renderer);

  SDL_RenderCopy(renderer, flag, NULL, &flagRect);
}

void updateStartButton_UI(SDL_Renderer* renderer) {
  int btnW, btnH;
  TTF_SizeText(font32, "START", &btnW, &btnH);
  SDL_Rect startRect = {(WINDOW_WIDTH - btnW) / 2, 600, btnW, btnH};
  SDL_RenderCopy(renderer, start32, NULL, &startRect);
}

void updateStageSelect_UI(SDL_Renderer* renderer, u8 stage) {
  SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
  SDL_RenderFillRect(renderer, &(SDL_Rect){0, 0, WINDOW_WIDTH, WINDOW_HEIGHT});

  int txtW, txtH;
  TTF_SizeText(font32, "STAGE", &txtW, &txtH);
  SDL_Rect stageTextRect = {GRID_CELL_SIZE * 6, (WINDOW_HEIGHT - txtH) / 2, txtW, txtH};
  SDL_Rect stageNumRect = {GRID_CELL_SIZE * 9, (WINDOW_HEIGHT - txtH) / 2, 32, 32};

  SDL_RenderCopy(renderer, stage32[BLACK], NULL, &stageTextRect);
  renderNumber(stage, 0, (SDL_Point){stageNumRect.x, stageNumRect.y}, false, renderer);
}

void updateGlobalScore_UI(SDL_Renderer* renderer, u8 stage) {
  SDL_Rect scoreTextRect = {GRID_CELL_SIZE * 5.5, GRID_CELL_SIZE, 160, 32};
  SDL_Point scorePnt = {GRID_CELL_SIZE * 8.5, GRID_CELL_SIZE};

  SDL_Rect stageTextRect = {GRID_CELL_SIZE * 6, GRID_CELL_SIZE * 2.5, 160, 32};
  SDL_Point stagePnt = {GRID_CELL_SIZE * 9, GRID_CELL_SIZE * 2.5};

  SDL_RenderCopy(renderer, score32, NULL, &scoreTextRect);
  renderNumber(gameScore, 4, scorePnt, true, renderer);

  SDL_RenderCopy(renderer, stage32[WHITE], NULL, &stageTextRect);
  renderNumber(stage, 4, stagePnt, true, renderer);
}
void updateStageScore_UI(SDL_Renderer* renderer) {
  u8 total = 0;

  SDL_Point tankScorePnt = {GRID_CELL_SIZE * 1.25, GRID_CELL_SIZE * 5};
  SDL_Rect ptsRect = {GRID_CELL_SIZE * 4, GRID_CELL_SIZE * 5, 96, 32};
  SDL_Point tankNumPnt = {GRID_CELL_SIZE * 6, GRID_CELL_SIZE * 5};
  SDL_Rect tankSpriteRect = {0, 0, 15, 16};
  SDL_Rect tankRect = {GRID_CELL_SIZE * 8, GRID_CELL_SIZE * 5, GRID_CELL_SIZE, GRID_CELL_SIZE};

  SDL_Rect separatorLine = {GRID_CELL_SIZE * 6, GRID_CELL_SIZE * 10.5, GRID_CELL_SIZE * 4, 4};
  SDL_Rect totalTextRect = {GRID_CELL_SIZE * 3, GRID_CELL_SIZE * 10.75, 160, 32};
  SDL_Point totalPnt = {GRID_CELL_SIZE * 6, GRID_CELL_SIZE * 10.75};

  for (u8 i = 0; i < 4; i++) {
    u16 scoreForTank = 100 + 100 * i;

    renderNumber(destroyedEnemsList[i] * scoreForTank, 4, tankScorePnt, true, renderer);
    SDL_RenderCopy(renderer, pts32, NULL, &ptsRect);
    renderNumber(destroyedEnemsList[i], 4, tankNumPnt, true, renderer);
    SDL_RenderCopy(renderer, tankTexture, &tankSpriteRect, &tankRect);

    tankScorePnt.y += 96;
    ptsRect.y += 96;
    tankNumPnt.y += 96;
    tankRect.y += 96;
    tankSpriteRect.y += 16;

    total += destroyedEnemsList[i];
  }
  SDL_SetRenderDrawColor(renderer, UI_MENU_TEXT_COLOR.r, UI_MENU_TEXT_COLOR.g, UI_MENU_TEXT_COLOR.b,
                         255);
  SDL_RenderFillRect(renderer, &separatorLine);
  SDL_RenderCopy(renderer, total32, NULL, &totalTextRect);
  renderNumber(total, 4, totalPnt, true, renderer);
}

void gameOver(TTF_Font* font, SDL_Renderer* renderer, u16 windowsWidth, u16 windowHeight) {
  int textW, textH;
  SDL_Texture* gameOverTexture = renderText("GAME OVER", font, GAME_OVER_COLOR, renderer);
  TTF_SizeText(font, "GAME OVER", &textW, &textH);
  SDL_Rect gameOverRect = {(windowsWidth - textW) / 2, (windowHeight - textH) / 2, textW, textH};
  SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);

  SDL_RenderPresent(renderer);
  SDL_Delay(2000);
  SDL_Shutdown();
  exit(0);
}