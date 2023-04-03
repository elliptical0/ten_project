#pragma once

#include <tonc_input.h> 

// the input and output of the input() function
typedef struct INPUTSTATE {
    int cursor_map_x;
    int cursor_map_y;
    int metr_map_x;
    int metr_map_y;
    int spriteNum;
} INPUTSTATE;

int inputinit(INPUTSTATE* inputstate);

int input(INPUTSTATE* inputstate);