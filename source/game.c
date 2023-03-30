#include "game.h"

//#define RANGE(a, x, y) if(a <= x) { a = x; } else if(a > y) { a = y; } // TODO: pls replace (use ternary?)
//#define RANGE(a, x, y) (a < x ? x : (a > y ? y : a))

int gameinit(GAMESTATE* gamestate) {
    return 0;
}

// @return error code, or 0 if there are no errors.
int game(INPUTSTATE* inputstate, GAMESTATE* gamestate) {
    //gamestate->xpos = RANGE(gamestate->xpos + inputstate->x, 0, 176); // TODO: replace magic number
    //gamestate->ypos = RANGE(gamestate->ypos + inputstate->y, 0, 96); // TODO: replace magic number
    return 0;
}