#pragma once

#include <tonc_input.h> 

typedef enum INPUT { // value is the number of the bit in REG_KEYINPUT
    A = 0,
    B = 1,
    SELECT = 2,
    START = 3,
    RIGHT = 4,
    LEFT = 5,
    UP = 6,
    DOWN = 7,
    R = 8,
    L = 9,
    NONE = 10
} INPUT;

#define INPUTBUFFER_SIZE 8 // MUST be a power of 2 for fast modulo. will have 1 empty space
#define INPUTBUFFER_MODULO(n) ((n) & (INPUTBUFFER_SIZE - 1))

#define CURSOR_ANIM_FRAMES 16 // MUST be a power of 2

// the input and output of the input() function
typedef struct INPUTSTATE {
    INPUT cursor_anim;
    uint cursor_anim_frame; // decrementing frame counter for cursor animation

    int cursor_map_x;
    int cursor_map_y;

    uint cursor_menu_pos;
} INPUTSTATE;

int queue(INPUT in);
INPUT dequeue();

int inputinit(INPUTSTATE* inputstate);

int input(unsigned int frame, INPUTSTATE* inputstate);