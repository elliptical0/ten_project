#include "map_data.h"

// index, x-position, y-position
const ENEMY mpfieldEnemies[] = {
    {0, 1, 1}, {1, 2, 2}
};
const int mpfieldPlayers[MAX_PLR_UNITS][2] = {
    {1, 7}, {2, 7}, {3,7}, {4,7}, {1,8}, {2,8}, {3,8}, {4,8}
};
const char mpfieldTerrain[10][15] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,0,0,0},
    {0,1,0,0,0,0,1,1,1,0,0,0,0,1,1},
    {1,1,1,1,1,0,0,1,0,0,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

// index, x-position, y-position
const ENEMY mpislandEnemies[] = {
    {0, 6, 1}, {0, 7, 1}, {1, 6, 2}, {2, 7, 2}
};
const int mpislandPlayers[MAX_PLR_UNITS][2] = {
    {6, 1}, {7,1}, {8,1}, {9,1}, {6,2}, {7,2}, {8,2}, {9,2}
};
const char mpislandTerrain[10][15] = {
    {1,1,1,1,0,1,1,1,1,1,1,0,0,0,0},
    {1,1,1,1,0,1,1,1,1,1,1,0,0,0,0},
    {1,1,1,1,0,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,1,0,1,1,1,1,0,0,0,0,0,0},
    {1,1,1,1,0,1,1,1,0,0,0,0,1,1,1},
    {1,1,1,1,0,0,1,0,0,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

const char MPENEMYDATA_LEN[] = { // index is a MAP enum value
    2,
    4,
    0
};
const ENEMY *MPENEMYDATA[] = { // index is a MAP enum value
    mpfieldEnemies,
    mpislandEnemies,
    0
};
const int (*MPPLRDATA[])[MAX_PLR_UNITS][2] = { // starting positions for players
    &mpfieldPlayers,
    &mpislandPlayers,
    0
};
const char (*MPTERRAINDATA[])[10][15] = { // index is a MAP enum value
    &mpfieldTerrain,
    &mpislandTerrain,
    0
};