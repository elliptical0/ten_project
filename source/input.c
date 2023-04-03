#include "input.h"

#define RANGE(a, x, y) (a < x ? x : (a > y ? y : a))

// bool debounce = false; // when debounce is true, ignore current inputs

INPUT inputbuffer[INPUTBUFFER_SIZE]; // queue
int inputbuffer_count = 0; // elements in queue
int inputbuffer_front = 0;
int inputbuffer_rear = 0;

int queue(INPUT in) {
    if(inputbuffer_count < INPUTBUFFER_SIZE) {
        inputbuffer[inputbuffer_rear] = in;
        inputbuffer_rear = INPUTBUFFER_MODULO(inputbuffer_rear + 1);
        inputbuffer_count++;
        return 0;
    }
    return 1;
}
INPUT dequeue() {
    if(inputbuffer_count > 0) {
        //INPUT out = inputbuffer[inputbuffer_front];
        inputbuffer_front = INPUTBUFFER_MODULO(inputbuffer_front + 1);
        inputbuffer_count--;
        return inputbuffer[INPUTBUFFER_MODULO(inputbuffer_front - 1)];
    }
    return NONE;
}
// #define peek() inputbuffer[inputbuffer_front]

int inputinit(INPUTSTATE* inputstate) {
    for(int i = 0; i < INPUTBUFFER_SIZE; i++) {
        inputbuffer[i] = NONE;
    }

    inputstate->cursor_anim = NONE;
    inputstate->cursor_anim_frame = 0;

    inputstate->cursor_map_x = 0;
    inputstate->cursor_map_y = 0;

    inputstate->cursor_menu_pos = 0;
    return 0;
}

int input(unsigned int frame, INPUTSTATE* inputstate) {
    key_poll();
    for(INPUT i = A; i < NONE; i++) {
        if(key_hit(1 << i)) {
            if(queue(i) == 1) {
                break;
            }
        }
    }

    if(inputstate->cursor_anim_frame == 0) {
        inputstate->cursor_anim = dequeue();
        if(inputstate->cursor_anim != NONE) {
            switch(inputstate->cursor_anim) {
                case UP:
                    inputstate->cursor_map_y = RANGE(inputstate->cursor_map_y - 1, 0, 9);
                    inputstate->cursor_anim_frame = CURSOR_ANIM_FRAMES - 1;
                    break;
                case DOWN:
                    inputstate->cursor_map_y = RANGE(inputstate->cursor_map_y + 1, 0, 9);
                    inputstate->cursor_anim_frame = CURSOR_ANIM_FRAMES - 1;
                    break;
                case LEFT:
                    inputstate->cursor_map_x = RANGE(inputstate->cursor_map_x - 1, 0, 14);
                    inputstate->cursor_anim_frame = CURSOR_ANIM_FRAMES - 1;
                    break;
                case RIGHT:
                    inputstate->cursor_map_x = RANGE(inputstate->cursor_map_x + 1, 0, 14);
                    inputstate->cursor_anim_frame = CURSOR_ANIM_FRAMES - 1;
                    break;
                default:
            }
        }
    } else {
        inputstate->cursor_anim_frame--;
    }
    return 0;
}