#include <tonc.h>
#include <tonc_types.h>

#include "input.h"
#include "game.h"

// the input and output of the render() function
typedef struct RENDERSTATE {
    int xpos; // temp, for demo
    int ypos; // temp, for demo
    OBJ_ATTR *metr;
} RENDERSTATE;

int render(INPUTSTATE* inputstate, GAMESTATE* gamestate, RENDERSTATE* renderstate);