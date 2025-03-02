#include <stdbool.h>

#include "wrapper/vartypes.h"

typedef struct {
  u8 basic, fast, power, armored;
  u8 totalEnems;
  u8 enemiesAtScreen;

} StageEnemies;

StageEnemies loadStage(char* stagePath, bool isGame);