#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include "blocks.h"
#include "explosions.h"
#include "powerup.h"
#include "stageReader.h"
#include "tank.h"
#include "ui.h"
// #include "stringStorage.h"
#include "wrapper/AssetControls.h"
#include "wrapper/LinkedList.h"
#include "wrapper/SDL_Controls.h"
#include "wrapper/errHandler.h"
#include "wrapper/timer.h"
#include "wrapper/vartypes.h"

// Window related macros are stored in wrapper/SDL_Controls.h

static SDL_Window* window;
static SDL_Renderer* renderer;

static SDL_Texture *yellowplr, *grayenm, *purpleenm, *blocks, *bullet, *pwrups, *shield,
    *bulletexplosion, *tankexplosion;

static Mix_Chunk *bulcollision, *enemexplosion, *damaged, *levelup, *plrexplosion, *plrfire,
    *pwrupcollect, *pwrupspawn;

static Mix_Music *stagestart, *gameover, *idle, *moving;

void init() {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
  IMG_Init(IMG_INIT_PNG);
  TTF_Init();

  if (Mix_OpenAudio(44100, AUDIO_F32SYS, 2, 2048) < 0) {
    catchError(false, __FILE__);
    if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 2048) < 0) {
      catchError(false, __FILE__);
    }
  }
  Mix_Volume(-1, 32);
  Mix_VolumeMusic(32);

  window = SDL_CreateWindow("Battle town", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            WINDOW_WIDTH, WINDOW_HEIGHT, 0);
  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (window == NULL || renderer == NULL) {
    catchError(true, __FILE__);
  }
  srand(time(NULL));
}

void loadAssets() {
  // if (font == NULL) {
  //   catchError(false, __FILE__);
  // }
  // rotateSound = loadSound("./assets/sounds/rotate.wav");
  yellowplr = renderTexture("./assets/sprites/yellowplr.png", renderer, true);
  grayenm = renderTexture("./assets/sprites/grayenm.png", renderer, true);
  purpleenm = renderTexture("./assets/sprites/purpleenm.png", renderer, true);
  blocks = renderTexture("./assets/sprites/blocks.png", renderer, true);
  bullet = renderTexture("./assets/sprites/bullet.png", renderer, true);
  bulletexplosion = renderTexture("./assets/sprites/bulletexplosion.png", renderer, true);
  tankexplosion = renderTexture("./assets/sprites/tankexplosion.png", renderer, true);
  pwrups = renderTexture("./assets/sprites/pwrups.png", renderer, true);
  shield = renderTexture("./assets/sprites/shield.png", renderer, true);

  bulcollision = loadSound("./assets/sounds/bulcollision.wav");
  enemexplosion = loadSound("./assets/sounds/enemexplosion.wav");
  damaged = loadSound("./assets/sounds/damaged.wav");
  levelup = loadSound("./assets/sounds/levelup.wav");
  plrexplosion = loadSound("./assets/sounds/plrexplosion.wav");
  plrfire = loadSound("./assets/sounds/plrfire.wav");
  pwrupcollect = loadSound("./assets/sounds/pwrupcollect.wav");
  pwrupspawn = loadSound("./assets/sounds/pwrupspawn.wav");

  stagestart = loadMusic("./assets/music/stagestart.mp3");
  gameover = loadMusic("./assets/music/gameover.mp3");
  idle = loadMusic("./assets/music/idle.mp3");
  moving = loadMusic("./assets/music/moving.mp3");
  loadUIAssets(renderer);
}

Tank* createPlayer() {
  Bullet* playerBullet = createBullet(BULLET_DEFAULT_POS, BULLET_DEFAULT_POS, 16);
  Bullet* playerBullet2 = createBullet(BULLET_DEFAULT_POS, BULLET_DEFAULT_POS, 16);
  Tank* player =
      createTank(FIELD_POS_X + GRID_CELL_SIZE * 4, FIELD_POS_Y + GRID_CELL_SIZE * 12, 8, 0,
                 yellowplr, BASIC, UP, playerBullet, playerBullet2, true, false, true);
  return player;
}

void resetStage(u8* spawnedEnems, u8* totalSpawnedEnems, bool keepPlayer) {
  clearBullets(keepPlayer);
  clearTanks(keepPlayer);
  clearBExplosions();
  clearTExplosions();
  destroyPowerup();
  for (u8 i = 0; i < 4; i++) {
    spawnedEnems[i] = 0;
  }
  *totalSpawnedEnems = 0;
  setLives(2);
}

int main(int argc, char** arg) {
  u8 spawnCooldownCount;
  bool quit = false;
  u8 updateCooldown = 0;
  u16 blockListLen = 0;
  StageEnemies stageEnems;
  u8 spawnedEnems[4] = {0, 0, 0, 0};
  u8 totalSpawnedEnems = 0;
  bool isScoreScreenTimer = false;

  enum SCENES scene;
  u8 stage = 1;

  init();

  loadAssets();

  setBlockTexture(blocks);
  setBulletTexture(bullet);
  setExplosionsTexture(bulletexplosion, tankexplosion);
  setEnemiesTexture(grayenm, purpleenm);
  setTanksTexture_UI(grayenm);
  setPowerupsTexture(pwrups);
  setShieldTexture(shield);

  setTankSounds(enemexplosion, damaged, levelup, plrexplosion, plrfire, pwrupcollect, idle, moving);
  setBulletSounds(bulcollision);
  setPowerupSounds(pwrupspawn);

  setScene(TITLE);
  loadStage("./assets/stages/title.stage", false);

  Block** blockListPtr = getBlockList(&blockListLen);

  Tank* player = createPlayer();

  // spawnEnemy(blockListPtr, blockListLen, false);
  // totalSpawnedEnems++;

  SDL_Rect field = {FIELD_POS_X, FIELD_POS_Y, FIELD_WIDTH, FIELD_HEIGHT};
  SDL_Event e;

  while (!quit) {
    scene = getScene();
    SDL_PollEvent(&e);
    switch (e.type) {
      case SDL_KEYDOWN:
        switch (e.key.keysym.sym) {
          case SDLK_F1:  // Debug key - Destroy tanks
            destroyTanks(true);
            break;
          case SDLK_LEFT:
            switch (scene) {
              case STAGE:
                if (stage > 1) {
                  stage--;
                }
                break;
              case GAME:
                setTankDirection(player, LEFT, true);
                break;
            }
            break;
          case SDLK_RIGHT:
            switch (scene) {
              case STAGE:
                if (stage < 5) {
                  stage++;
                }
                break;
              case GAME:
                setTankDirection(player, RIGHT, true);
                break;
                break;
            }
            break;
          case SDLK_UP:
            if (scene == GAME)
              setTankDirection(player, UP, true);
            break;
          case SDLK_DOWN:
            if (scene == GAME)
              setTankDirection(player, DOWN, true);
            break;
          case SDLK_x:
          case SDLK_z:
            if (scene == GAME) {
              shoot(player);
            }
            break;
          case SDLK_RETURN:
            switch (scene) {
              case TITLE:
                setScene(STAGE);
                break;
              case STAGE:
                setScene(GAME);
                Mix_PlayMusic(stagestart, 0);
                switch (stage) {
                  case 1:
                    stageEnems = loadStage("./assets/stages/1.stage", true);
                    break;
                  case 2:
                    stageEnems = loadStage("./assets/stages/2.stage", true);
                    break;
                  case 3:
                    stageEnems = loadStage("./assets/stages/3.stage", true);
                    break;
                  case 4:
                    stageEnems = loadStage("./assets/stages/4.stage", true);
                    break;
                  case 5:
                    stageEnems = loadStage("./assets/stages/5.stage", true);
                    break;

                  default:
                    break;
                }
                player->posX = FIELD_POS_X + GRID_CELL_SIZE * 4;
                player->posY = FIELD_POS_Y + GRID_CELL_SIZE * 12;

                blockListPtr = getBlockList(&blockListLen);
                setEnemy(BASIC);
                spawnEnemy(blockListPtr, blockListLen, false);
                totalSpawnedEnems++;
                break;
              case SCORE:
                isScoreScreenTimer = false;
                if (getLives() == -1) {
                  setScene(GAME_OVER);
                  Mix_PlayMusic(gameover, 0);
                  loadStage("./assets/stages/gameover.stage", false);
                } else {
                  resetStage(spawnedEnems, &totalSpawnedEnems, true);
                  setScene(STAGE);
                  stage++;
                }
                break;
              case GAME_OVER:
                setScene(TITLE);
                resetStage(spawnedEnems, &totalSpawnedEnems, false);
                player = createPlayer();
                loadStage("./assets/stages/title.stage", false);
                break;
            }
          default:
            break;
        }
        break;
      case SDL_KEYUP:
        switch (e.key.keysym.sym) {
          case SDLK_LEFT:
            setTankDirection(player, LEFT, false);
            break;
          case SDLK_RIGHT:
            setTankDirection(player, RIGHT, false);
            break;
          case SDLK_UP:
            setTankDirection(player, UP, false);
            break;
          case SDLK_DOWN:
            setTankDirection(player, DOWN, false);
            break;
          default:
            break;
        }
        break;
      case SDL_QUIT:
        quit = true;
        break;
      default:
        break;
    }

    if (updateCooldown == UPDATE_RATE) {
      if (scene == GAME) {
        SDL_SetRenderDrawColor(renderer, 125, 125, 125, 255);
      } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
      }
      SDL_RenderClear(renderer);

      if (scene == GAME) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderFillRect(renderer, &field);

        // puts("before render tanks");
        renderTanks(renderer, blockListPtr, blockListLen);
        // puts("after render tanks");
        updateEnemies();
        // puts("after update enemies");
      } else if (scene == TITLE) {
        updateStartButton_UI(renderer);
      }
      if (scene != SCORE) {
        renderBlocks(renderer, scene == GAME ? GRID_CELL_SIZE : MENU_GRID_CELL_SIZE);
      }
      // puts("after render blocks");
      if (scene == GAME) {
        renderBullet(renderer, blockListPtr, blockListLen);
        // puts("after render bullet");
        renderExplosions(renderer);
        // puts("after render explosions");
        renderPowerup(renderer);
        // puts("after render powerups");

        updateEnemiesLeft_UI(stageEnems.totalEnems - totalSpawnedEnems, renderer);
        updateStage_UI(1, renderer);

        if (totalSpawnedEnems < stageEnems.totalEnems &&
            getScreenEnemsNum() < stageEnems.enemiesAtScreen) {
          if (spawnCooldownCount == (ENEMY_SPAWN_COOLDOWN - stage * 8)) {
            u8 enemyType;
            bool success = true;
            do {
              success = true;
              switch (rand() % 4) {
                case 0:
                  enemyType = BASIC;
                  if (stageEnems.basic - spawnedEnems[enemyType] <= 0) {
                    success = false;
                  }
                  break;
                case 1:
                  enemyType = FAST;
                  if (stageEnems.fast - spawnedEnems[enemyType] <= 0) {
                    success = false;
                  }
                  break;
                case 2:
                  enemyType = POWER;
                  if (stageEnems.power - spawnedEnems[enemyType] <= 0) {
                    success = false;
                  }
                  break;
                case 3:
                  enemyType = ARMORED;
                  if (stageEnems.armored - spawnedEnems[enemyType] <= 0) {
                    success = false;
                  }
                  break;
                default:
                  break;
              }
            } while (success != true);
            setEnemy(enemyType);
            if (totalSpawnedEnems == 3 || totalSpawnedEnems == 10 || totalSpawnedEnems == 17 ||
                totalSpawnedEnems == 24 || totalSpawnedEnems == 31) {
              spawnEnemy(blockListPtr, blockListLen, true);
            } else {
              spawnEnemy(blockListPtr, blockListLen, false);
            }

            spawnedEnems[enemyType]++;
            totalSpawnedEnems++;
            spawnCooldownCount = 0;
          } else {
            spawnCooldownCount++;
          }
        } else if (getScreenEnemsNum() == 0 && totalSpawnedEnems == stageEnems.totalEnems) {
          if (!isScoreScreenTimer) {
            setScoreScreenTimer();
            isScoreScreenTimer = true;
          }
        }
      } else if (scene == STAGE) {
        updateStageSelect_UI(renderer, stage);
      } else if (scene == SCORE) {
        updateStageScore_UI(renderer);
        updateGlobalScore_UI(renderer, stage);
      }
      // for (u16 i = 0; i < blockListLen; i++) {
      //   Block* block = blockListPtr[i];
      //   if (block->type != AIR) {
      //     SDL_SetRenderDrawColor(renderer, 200, 0, 100, 255);
      //     SDL_RenderFillRect(renderer,
      //                        &(SDL_Rect){block->posX, block->posY, block->size.x,
      //                        block->size.y});
      //   }
      // }
      updateCooldown = 0;

      SDL_RenderPresent(renderer);
    } else {
      updateCooldown++;
    }
    SDL_Delay(1000 / MAX_FPS);
  }

  // TTF_CloseFont(font);
  SDL_Shutdown();
  exit(EXIT_SUCCESS);
}
