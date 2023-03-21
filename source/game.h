#pragma once

#include "input.h"

// the input and output of the gamestate() function
typedef struct GAMESTATE {
    int xpos; // temp, for demo
    int ypos; // temp, for demo
} GAMESTATE;

int gameinit(GAMESTATE* gamestate);

int game(INPUTSTATE* inputstate, GAMESTATE* gamestate);