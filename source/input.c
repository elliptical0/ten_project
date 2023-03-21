#include "input.h"

int inputinit(INPUTSTATE* inputstate) {
    return 0;
}

int input(INPUTSTATE* inputstate) {
    key_poll();
    inputstate->x = key_tri_horz();
    inputstate->y = key_tri_vert();
    return 0;
}