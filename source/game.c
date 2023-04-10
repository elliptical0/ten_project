#include "game.h"

//#define RANGE(a, x, y) if(a <= x) { a = x; } else if(a > y) { a = y; } // TODO: pls replace (use ternary?)
#define RANGE(a, x, y) (a < x ? x : (a > y ? y : a))

const void ((*act_functions[])(INPUTSTATE *is, GAMESTATE *gs)) = {&fight, &item, &wait};
const void ((*fight_functions[])(INPUTSTATE *is, GAMESTATE *gs)) = {&damage};
const MENU MENUDATA[] = {
    {3, act_functions, NOUI},
    {1, fight_functions, uiact}
};

void fight(INPUTSTATE *is, GAMESTATE *gs) {
    get_targets(gs, (ITEMDATA[gs->units_plr[gs->selected_unit-1].unit_attr->inventory[gs->units_plr[gs->selected_unit - 1].weapon]].job == bow ? 2 : 1), is->cursor_map_x, is->cursor_map_y);
    if(gs->targetcount > 0) {
        gs->menu = uifight;
        gs->targetindex = 0;
        forecast(gs, gs->selected_unit, gs->targets[gs->targetindex]);
    }
}
void damage(INPUTSTATE *is, GAMESTATE *gs) {
    tte_write("damage"); // debug

    wait(is, gs);
}
void item(INPUTSTATE *is, GAMESTATE *gs) {
    tte_set_pos(92, 80);
    tte_write("item"); // debug
}
void wait(INPUTSTATE *is, GAMESTATE *gs) {
    gs->map_units[gs->selected_unit_map_y][gs->selected_unit_map_x] = 0;
    gs->map_units[is->cursor_map_y][is->cursor_map_x] = gs->selected_unit;
    gs->units_plr[gs->selected_unit - 1].can_act = false;
    gs->selected_unit = 0;

    gs->menu = NOUI;
}

void forecast(GAMESTATE *gs, int attacker_index, int defender_index) { // programming for skills goes here
    UNIT_ATTR *attacker_attr = gs->units_plr[attacker_index - 1].unit_attr;
    ITEM_ATTR *attacker_weapon = &ITEMDATA[attacker_attr->inventory[gs->units_plr[attacker_index - 1].weapon]];
    ITEM_ATTR *attacker_equipment = &ITEMDATA[attacker_attr->inventory[gs->units_plr[attacker_index - 1].equipment]];
    
    UNIT_ATTR *defender_attr = gs->units_plr[defender_index - 1].unit_attr;
    ITEM_ATTR *defender_weapon = &ITEMDATA[defender_attr->inventory[gs->units_plr[defender_index - 1].weapon]];
    ITEM_ATTR *defender_equipment = &ITEMDATA[defender_attr->inventory[gs->units_plr[defender_index - 1].equipment]];

    gs->forecast_attacker_name = &attacker_attr->name;
    gs->forecast_attacker_weapon = &attacker_weapon->name;
    gs->forecast_attacker_hp = gs->units_plr[attacker_index - 1].hp;
    gs->forecast_attacker_damage = attacker_attr->str + attacker_weapon->str;
    gs->forecast_attacker_chance = attacker_attr->hit + attacker_weapon->hit - defender_attr->avo - defender_weapon->avo;
    
    gs->forecast_defender_name = &defender_attr->name;
    gs->forecast_defender_weapon = &defender_weapon->name;
    gs->forecast_defender_hp = gs->units_plr[defender_index - 1].hp;
    gs->forecast_defender_damage = defender_attr->str + defender_weapon->str;
    gs->forecast_defender_chance = defender_attr->hit + defender_weapon->hit - attacker_attr->avo - attacker_weapon->avo;

    // TODO: calculate attacker skills
    // TODO: calculate defender skills
    // TODO: calculate adjacent unit skills
}
void get_targets(GAMESTATE *gs, int range, int x, int y) {
    gs->targetcount = 0;
    for(int v = -range; v <= range; v++) {
        if(y + v > 0 && y + v < 10) {
            for(int h = -(range - ABS(v)); h <= (range - ABS(v)); h++) {
                if(x + h > 0 && x + h < 10 && !(v == 0 && h == 0)) {
                    if(gs->map_units[y + v][x + h] > MAX_PLR_UNITS) { // is an enemy unit
                        gs->targets[gs->targetcount] = gs->map_units[y + v][x + h];
                        gs->targetcount++;
                    }
                }
            }
        }
    }
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
        gs->units_plr[i].can_act = true;
        gs->map_units[(*MPPLRDATA)[map][i][1]][(*MPPLRDATA)[map][i][0]] = i + 1;
    }
    for(int i = 0; i < MAX_ENEMY_UNITS && i < MPENEMYDATA_LEN[map]; i++) {
        gs->units_enemy[i].unit_attr = UNITDATA_ENEMY + MPENEMYDATA[map][i].unitdata_index;
        gs->units_enemy[i].hp = gs->units_enemy[i].unit_attr->maxhp;
        gs->units_enemy[i].weapon = 0; //gs->units_enemy[i].unit_attr->inventory[0];
        gs->units_enemy[i].equipment = 1; //gs->units_enemy[i].unit_attr->inventory[1];
        gs->units_enemy[i].can_act = true;
        gs->map_units[MPENEMYDATA[map][i].y][MPENEMYDATA[map][i].x] = i + 1 + MAX_PLR_UNITS;
    }
}

void player_turn(GAMESTATE *gs, MAP map) {
    for(int i = 0; i < MAX_PLR_UNITS; i++) {
        gs->units_plr[i].can_act = true;
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
                    note_play(NOTE_C, 1);
                    if(gs->selected_unit != 0) {
                        if(gs->map_units[is->cursor_map_y][is->cursor_map_x] == gs->selected_unit
                        || (gs->map_units[is->cursor_map_y][is->cursor_map_x] == 0 && (*MPTERRAINDATA[gs->map])[is->cursor_map_y][is->cursor_map_x] != impassable)) {
                            gs->menu = uiact;
                            is->cursor_menu_pos = 0;
                            is->mapmode = false;
                        //} else if(gs->map_units[is->cursor_map_y][is->cursor_map_x] > MAX_PLR_UNITS) { // enemy selected
                        //    fight(is, gs);
                        }
                    } else if(gs->selected_unit == 0) {
                        gs->selected_unit = gs->map_units[is->cursor_map_y][is->cursor_map_x];
                        if(gs->selected_unit > 0 && gs->selected_unit <= MAX_PLR_UNITS && gs->units_plr[gs->selected_unit - 1].can_act) {
                            gs->selected_unit_map_x = is->cursor_map_x;
                            gs->selected_unit_map_y = is->cursor_map_y;
                        } else {
                            gs->selected_unit = 0;
                        }
                    }
                    break;
                case B:
                note_play(NOTE_C, -1);
                    if(gs->selected_unit != 0) {
                        gs->selected_unit = 0;
                    }
                    break;
                default:
            }
        } else {
            switch(is->input) {
                case A:
                    note_play(NOTE_E, 1);
                    (*(MENUDATA[gs->menu].choice_functions[is->cursor_menu_pos]))(is, gs);
                    is->mapmode = gs->menu == NOUI;
                    break;
                case B:
                    note_play(NOTE_C, -1);
                    gs->menu = MENUDATA[gs->menu].previous;
                    is->cursor_menu_pos = 0;
                    is->mapmode = gs->menu == NOUI;
                    break;
                case UP:
                case DOWN:
                    note_play(NOTE_D, 1);
                    if(is->cursor_menu_pos < 0) {
                        is->cursor_menu_pos = 0;
                        is->anim_frame = 0;
                    } else if(!(is->cursor_menu_pos < MENUDATA[gs->menu].choices)) {
                        is->cursor_menu_pos = MENUDATA[gs->menu].choices - 1;
                        is->anim_frame = 0;
                    }
                    break;
                case LEFT:
                    if(gs->menu == uifight) {
                        gs->targetindex = RANGE(gs->targetindex - 1, 0, gs->targetcount - 1);
                        forecast(gs, gs->selected_unit, gs->targets[gs->targetindex]);
                    }
                    break;
                case RIGHT:
                    if(gs->menu == uifight) {
                        gs->targetindex = RANGE(gs->targetindex + 1, 0, gs->targetcount - 1);
                        forecast(gs, gs->selected_unit, gs->targets[gs->targetindex]);
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