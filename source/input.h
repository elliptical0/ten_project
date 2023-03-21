#pragma once

#include <tonc_input.h> 

// the input and output of the input() function
typedef struct INPUTSTATE {
    int x;
    int y;
} INPUTSTATE;

int inputinit(INPUTSTATE* inputstate);

int input(INPUTSTATE* inputstate);