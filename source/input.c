#include "input.h"

int input(INPUTSTATE* inputstate) {
    key_poll();
    inputstate->x = key_tri_horz;
    inputstate->y = key_tri_vert;
    return 0;
}