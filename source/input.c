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
    return NOINPUT;
}
// #define peek() inputbuffer[inputbuffer_front]

int inputinit(INPUTSTATE* is) {
    for(int i = 0; i < INPUTBUFFER_SIZE; i++) {
        inputbuffer[i] = NOINPUT;
    }

    is->input = NOINPUT;
    is->input_read = false;
    is->mapmode = true;
    is->anim_frame = 0;

    is->cursor_map_x = 0;
    is->cursor_map_y = 0;

    is->cursor_menu_pos = 0;
    return 0;
}

int input(unsigned int frame, INPUTSTATE* is) {
    key_poll();
    for(INPUT i = A; i <= L; i++) {
        if(key_hit(1 << i)) {
            if(queue(i) == 1) {
                break;
            }
        }
    }

    if(is->anim_frame == 0) {
        is->input = dequeue();
        if(is->input != NOINPUT) {
            is->input_read = false;
            if(is->mapmode) {
                switch(is->input) {
                    case UP:
                        is->cursor_map_y = RANGE(is->cursor_map_y - 1, 0, 9);
                        is->anim_frame = CURSOR_ANIM_FRAMES - 1;
                        break;
                    case DOWN:
                        is->cursor_map_y = RANGE(is->cursor_map_y + 1, 0, 9);
                        is->anim_frame = CURSOR_ANIM_FRAMES - 1;
                        break;
                    case LEFT:
                        is->cursor_map_x = RANGE(is->cursor_map_x - 1, 0, 14);
                        is->anim_frame = CURSOR_ANIM_FRAMES - 1;
                        break;
                    case RIGHT:
                        is->cursor_map_x = RANGE(is->cursor_map_x + 1, 0, 14);
                        is->anim_frame = CURSOR_ANIM_FRAMES - 1;
                        break;
                    default:
                }
            } else {
                switch(is->input) {
                    case UP:
                        is->cursor_menu_pos--;
                        is->anim_frame = CURSOR_ANIM_FRAMES - 1;
                        break;
                    case DOWN:
                        is->cursor_menu_pos++;
                        is->anim_frame = CURSOR_ANIM_FRAMES - 1;
                        break;
                    default:
                }
            }
        }
    } else {
        is->anim_frame--;
    }
    return 0;
}