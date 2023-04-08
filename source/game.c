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

void save_game(SAVE_DATA *data) {
    *CART_RAM = *data;
}

void init_unit_status(GAMESTATE *gs, MAP map) {
    //char debug_maxhp[2] = {gs->data.units[0].maxhp + 0x30, 0};
    //tte_write(debug_maxhp);
    //tte_write(gs->data.units[0].name);
    memset16(gs->units_plr, 0, (MAX_PLR_UNITS + MAX_ENEMY_UNITS) * (sizeof(UNIT_STATUS) / 2)); // re-initialize
    for(int i = 0; i < MAX_PLR_UNITS && gs->data.units[i].maxhp != 0; i++) {
        gs->units_plr[i].unit_attr = gs->data.units + i;
        //tte_write(gs->units_plr[i].unit_attr->name);
        gs->units_plr[i].hp = gs->data.units[i].maxhp;
        gs->units_plr[i].weapon = -1;
        gs->units_plr[i].equipment = -1;
        for(int j = INVENTORY_SPACES - 1; j >= 0; j--) { // reverse order: first item takes priority
            switch(ITEMDATA[gs->data.units[i].inventory[j]].type) {
                case weapon:
                    gs->units_plr[i].weapon = j;
                    break;
                case equipment:
                    gs->units_plr[i].equipment = j;
                    break;
                default:
            }
        }
        gs->map_units[(*MPPLRDATA)[map][i][1]][(*MPPLRDATA)[map][i][0]] = i + 1;
    }
    for(int i = 0; i < MAX_ENEMY_UNITS && i < MPENEMYDATA_LEN[map]; i++) {
        gs->units_enemy[i].unit_attr = UNITDATA_ENEMY + MPENEMYDATA[map][i].unitdata_index;
        gs->units_enemy[i].hp = gs->units_enemy[i].unit_attr->maxhp;
        gs->units_enemy[i].weapon = 0; //gs->units_enemy[i].unit_attr->inventory[0];
        gs->units_enemy[i].equipment = 1; //gs->units_enemy[i].unit_attr->inventory[1];
        gs->map_units[MPENEMYDATA[map][i].y][MPENEMYDATA[map][i].x] = i + 1 + MAX_PLR_UNITS;
    }
}

void change_map(INPUTSTATE *is, GAMESTATE *gs, MAP map) {
    gs->map = map;
    if(map != mpmainmenu) {
        init_unit_status(gs, map);
        is->mapmode = true;
    }
}

int gameinit(GAMESTATE* gs) {
    //if(CART_RAM->map == 0) { // initialize save data
    if(true) {
        //memcpy16(gs->data.units, UNITDATA_PLR, sizeof(UNIT_ATTR));
        gs->data.map = mpfield;
        gs->data.units[0] = UNITDATA_PLR[0]; // sasha
    } else { // load saved game
        gs->data = *CART_RAM;
    }

    gs->menu = NOUI;
    gs->map = mpmainmenu;

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
                case START:
                    if(gs->map == mpmainmenu) {
                        change_map(is, gs, gs->data.map);
                    }
                    break;
                default:
            }
        }
        is->input_read = true;
    }
    return 0;
}