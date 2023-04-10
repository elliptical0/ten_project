#pragma once

#include "units.h"

typedef enum TERRAIN {
    impassable = 0,
    noeffect = 1
} TERRAIN;

typedef struct ENEMY {
    unsigned char unitdata_index;
    unsigned char x;
    unsigned char y;
} ENEMY;

extern const char MPENEMYDATA_LEN[];
extern const ENEMY *MPENEMYDATA[];
extern const int (*MPPLRDATA[])[MAX_PLR_UNITS][2];
extern const char (*MPTERRAINDATA[])[10][15]; 