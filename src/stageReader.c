#include <SDL2/SDL.h>
#include <stdio.h>

#include "blocks.h"
#include "stageReader.h"
#include "wrapper/SDL_Controls.h"
#include "wrapper/vartypes.h"

StageEnemies loadStage(char* stagePath, bool isGame) {
  FILE* stageFile = fopen(stagePath, "r");
  u8 byte;
  u16 curPos = 0;
  u8 row = 0, column = 0;

  u16 fileLen;
  u8 lineLen;
  if (isGame) {
    fileLen = 169;
    lineLen = 13 - 1;
  } else {
    fileLen = 676;
    lineLen = 26 - 1;
  }

  u8 gridCellSize = (isGame == true ? GRID_CELL_SIZE : MENU_GRID_CELL_SIZE);
  u8 fieldOffset = (isGame == true ? FIELD_POS_X : TITLE_OFFSET);

  u8 enemArrLen = 0;
  u8 enemArr[5];
  u8 totalEnems = 0;
  StageEnemies enemiesList;

  clearBlocks();
  while ((byte = fgetc(stageFile)) != 255) {
    if (curPos >= fileLen) {
      if (isGame) {
        enemArr[enemArrLen] = byte;
        enemArrLen++;
      } else {
        return enemiesList;
      }
    } else {
      createBlock(fieldOffset + gridCellSize * column, fieldOffset + gridCellSize * row, byte);
      curPos++;
      fseek(stageFile, curPos, SEEK_SET);
      if (column / lineLen) {
        row++;
        column = 0;
      } else {
        column++;
      }
    }
  }
  if (isGame) {
    totalEnems = enemArr[0] + enemArr[1] + enemArr[2] + enemArr[3];
    enemiesList =
        (StageEnemies){enemArr[0], enemArr[1], enemArr[2], enemArr[3], totalEnems, enemArr[4]};
  }
  return enemiesList;
}