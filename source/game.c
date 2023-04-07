#include "game.h"

//#define RANGE(a, x, y) if(a <= x) { a = x; } else if(a > y) { a = y; } // TODO: pls replace (use ternary?)
#define RANGE(a, x, y) (a < x ? x : (a > y ? y : a))

const void ((*act_functions[])()) = {&fight, &item, &wait};
const MENU MENUDATA[] = {
    {3, act_functions, NOUI}
};

void fight() {
    tte_set_pos(92, 68);
    tte_write("fight"); // debug
}
void item() {
    tte_set_pos(92, 80);
    tte_write("item"); // debug
}
void wait() {
    tte_set_pos(92, 92);
    tte_write("wait"); // debug
}

int gameinit(GAMESTATE* gs) {
    gs->menu = NOUI;

    return 0;
}

// @return error code, or 0 if there are no errors.
int game(unsigned int frame, INPUTSTATE* is, GAMESTATE* gs) {
    if(is->input != NOINPUT && !is->input_read) {
        if(is->mapmode) {
            switch(is->input) {
                case A:
                    gs->menu = act;
                    is->cursor_menu_pos = 0;
                    is->mapmode = false;
                    break;
                default:
            }
        } else {
            switch(is->input) {
                case A:
                    (*(MENUDATA[gs->menu].choice_functions[is->cursor_menu_pos]))();
                    is->mapmode = gs->menu == NOUI;
                    break;
                case B:
                    gs->menu = MENUDATA[gs->menu].previous;
                    is->cursor_menu_pos = 0;
                    is->mapmode = gs->menu == NOUI;
                    break;
                case UP:
                case DOWN:
                    if(is->cursor_menu_pos < 0) {
                        is->cursor_menu_pos = 0;
                        is->anim_frame = 0;
                    } else if(!(is->cursor_menu_pos < MENUDATA[gs->menu].choices)) {
                        is->cursor_menu_pos = MENUDATA[gs->menu].choices - 1;
                        is->anim_frame = 0;
                    }
                    break;
                default:
            }
        }
        is->input_read = true;
    }
    return 0;
}