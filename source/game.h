#pragma once

#include "input.h"

#define curs

// the input and output of the gamestate() function
typedef struct GAMESTATE {
    //int cursor_map_x;
    //int cursor_map_y;
} GAMESTATE;

int gameinit(GAMESTATE* gamestate);

int game(INPUTSTATE* inputstate, GAMESTATE* gamestate);