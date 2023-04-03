#include "input.h"

#define RANGE(a, x, y) (a < x ? x : (a > y ? y : a))

int inputinit(INPUTSTATE* inputstate) {
    inputstate->cursor_map_x = 0;
    inputstate->cursor_map_y = 0;

    inputstate->metr_map_x = 0;
    inputstate->metr_map_y = 0;

    inputstate-> spriteNum = 0;
    return 0;
}

int input(INPUTSTATE* inputstate) {
    key_poll();

    if(key_hit(KEY_START)){
        inputstate->spriteNum = 1;
    }
    if(key_hit(KEY_SELECT)){
        inputstate->spriteNum = 0;
    }

    //inputstate->x = key_tri_horz();
    //inputstate->y = key_tri_vert();


        if(key_hit(KEY_UP)) {
            inputstate->cursor_map_y = RANGE(inputstate->cursor_map_y - 1, 0, 9);
        }
        if(key_hit(KEY_DOWN)) {
            inputstate->cursor_map_y = RANGE(inputstate->cursor_map_y + 1, 0, 9);
        }
        if(key_hit(KEY_LEFT)) {
            inputstate->cursor_map_x = RANGE(inputstate->cursor_map_x - 1, 0, 14);
        }
        if(key_hit(KEY_RIGHT)) {
            inputstate->cursor_map_x = RANGE(inputstate->cursor_map_x + 1, 0, 14);
        }
    


        if(key_hit(KEY_L)) {
            inputstate->metr_map_y = RANGE(inputstate->metr_map_y - 1, 0, 9);
        }
        if(key_hit(KEY_R)) {
            inputstate->metr_map_y = RANGE(inputstate->metr_map_y + 1, 0, 9);
        }
        if(key_hit(KEY_A)) {
            inputstate->metr_map_x = RANGE(inputstate->metr_map_x - 1, 0, 14);
        }
        if(key_hit(KEY_B)) {
            inputstate->metr_map_x = RANGE(inputstate->metr_map_x + 1, 0, 14);
        }
    
    return 0;
}