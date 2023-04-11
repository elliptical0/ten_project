#include "game.h"

//#define RANGE(a, x, y) if(a <= x) { a = x; } else if(a > y) { a = y; } // TODO: pls replace (use ternary?)

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
    UNIT_STATUS *attacker_status = &gs->units_plr[gs->forecast.attacker_index - 1];
    UNIT_ATTR *attacker_attr = attacker_status->unit_attr;
    UNIT_STATUS *defender_status = &gs->units_plr[gs->forecast.defender_index - 1];
    UNIT_ATTR *defender_attr = defender_status->unit_attr;

    if(rng() < gs->forecast.attacker_chance) { // calculate attack
        defender_status->hp = RANGE(defender_status->hp - gs->forecast.attacker_damage, 0, defender_attr->maxhp);
        gs->attacker_combat_anim = 1; // hit
    } else {
        gs->attacker_combat_anim = 2; // miss
    }
    if(defender_status->hp > 0) {
        if(rng() < gs->forecast.defender_chance) { // calculate counterattack
            attacker_status->hp = RANGE(attacker_status->hp - gs->forecast.defender_damage, 0, attacker_attr->maxhp);
            gs->defender_combat_anim = 1; // hit
        } else {
            gs->defender_combat_anim = 2; // miss
        }
    }

    is->anim_frame = HIT_ANIM_FRAMES - 1;

    wait(is, gs);
}
void item(INPUTSTATE *is, GAMESTATE *gs) {
    tte_set_pos(92, 80);
    tte_write("item"); // debug
}
void wait(INPUTSTATE *is, GAMESTATE *gs) {
    gs->map_units[gs->selected_unit_map_y][gs->selected_unit_map_x] = 0;
    gs->map_units[is->cursor_map_y][is->cursor_map_x] = gs->selected_unit;
    //gs->units_plr[gs->selected_unit - 1].can_act = false;
    gs->selected_unit = 0;

    memset16(gs->map_canmove, 0, (10 * 15) / 2);
    gs->map_canmove_stale = true;
    set_map_threatened(gs);

    gs->menu = NOUI;
}

void forecast(GAMESTATE *gs, int attacker_index, int defender_index) { // programming for skills goes here    
    UNIT_ATTR *attacker_attr = gs->units_plr[attacker_index - 1].unit_attr;
    ITEM_ATTR *attacker_weapon = &(ITEMDATA[attacker_attr->inventory[gs->units_plr[attacker_index - 1].weapon]]);
    ITEM_ATTR *attacker_equipment = &(ITEMDATA[attacker_attr->inventory[gs->units_plr[attacker_index - 1].equipment]]);
    
    UNIT_ATTR *defender_attr = gs->units_plr[defender_index - 1].unit_attr;
    ITEM_ATTR *defender_weapon = &(ITEMDATA[defender_attr->inventory[gs->units_plr[defender_index - 1].weapon]]);
    ITEM_ATTR *defender_equipment = &(ITEMDATA[defender_attr->inventory[gs->units_plr[defender_index - 1].equipment]]);

    gs->forecast.attacker_index = attacker_index;
    gs->forecast.defender_index = defender_index;

    //gs->forecast.attacker_name = &attacker_attr->name;
    gs->forecast.attacker_weapon = &attacker_weapon->name;
    gs->forecast.attacker_hp = gs->units_plr[attacker_index - 1].hp;
    gs->forecast.attacker_damage = attacker_attr->str + attacker_weapon->str;
    gs->forecast.attacker_chance = attacker_attr->hit + attacker_weapon->hit - defender_attr->avo - defender_weapon->avo;
    
    //gs->forecast.defender_name = &defender_attr->name;
    gs->forecast.defender_weapon = &defender_weapon->name;
    gs->forecast.defender_hp = gs->units_plr[defender_index - 1].hp;
    gs->forecast.defender_damage = defender_attr->str + defender_weapon->str;
    gs->forecast.defender_chance = defender_attr->hit + defender_weapon->hit - attacker_attr->avo - attacker_weapon->avo;

    // TODO: calculate attacker skills
    // TODO: calculate defender skills
    // TODO: calculate adjacent unit skills
}
void get_targets(GAMESTATE *gs, int range, int x, int y) {
    gs->targetcount = 0;
    for(int v = -range; v <= range; v++) {
        if(y + v >= 0 && y + v < 10) {
            for(int h = -(range - ABS(v)); h <= (range - ABS(v)); h++) {
                if(x + h >= 0 && x + h < 15 && !(v == 0 && h == 0)) {
                    if(gs->map_units[y + v][x + h] > MAX_PLR_UNITS) { // is an enemy unit
                        gs->targets[gs->targetcount] = gs->map_units[y + v][x + h];
                        gs->target_locations[gs->targetcount][0] = x + h;
                        gs->target_locations[gs->targetcount][1] = y + v;
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

/*
 * note: does not zero gs->map_canmove !!!
 */
void set_map_canmove(GAMESTATE* gs, int unit, int speed, int range, int unit_x, int unit_y, bool threatened) {
    if(unit_x >= 0 && unit_x < 15 && unit_y >= 0 && unit_y < 10) {
        if((*MPTERRAINDATA[gs->map])[unit_y][unit_x] != impassable
        && (unit > MAX_PLR_UNITS ? (gs->map_units[unit_y][unit_x] > MAX_PLR_UNITS || gs->map_units[unit_y][unit_x] == 0) : gs->map_units[unit_y][unit_x] <= MAX_PLR_UNITS)) {
            if(gs->map_units[unit_y][unit_x] == 0 || gs->map_units[unit_y][unit_x] == unit) {
                if(threatened) {
                    //gs->map_threatened[unit_y][unit_x] = 1;
                    for(int v = -range; v <= range; v++) {
                        if(unit_y + v >= 0 && unit_y + v < 10) {
                            for(int h = -(range - ABS(v)); h <= (range - ABS(v)); h++) {
                                if(unit_x + h >= 0 && unit_x + h < 15) { // && !(v == 0 && h == 0)) {
                                    gs->map_threatened[unit_y + v][unit_x + h] = 1;
                                }
                            }
                        }
                    }
                } else {
                    gs->map_canmove[unit_y][unit_x] = 1;
                }
            }
            if(speed > 0) { // check adjacent tiles
                set_map_canmove(gs, unit, speed - 1, range, unit_x - 1, unit_y, threatened);
                set_map_canmove(gs, unit, speed - 1, range, unit_x + 1, unit_y, threatened);
                set_map_canmove(gs, unit, speed - 1, range, unit_x, unit_y - 1, threatened);
                set_map_canmove(gs, unit, speed - 1, range, unit_x, unit_y + 1, threatened);
            }
        }
    }
}

void set_map_threatened(GAMESTATE* gs) {
    memset16(gs->map_threatened, 0, (10 * 15) / 2);
    for(int y = 0; y < 10; y++) {
        for(int x = 0; x < 15; x++) {
            if(gs->map_units[y][x] > MAX_PLR_UNITS) { // is enemy unit
                set_map_canmove(gs, gs->map_units[y][x], 3, gs->units_plr[gs->map_units[y][x] - 1].unit_attr->job == bow ? 2 : 1, x, y, true);
            }
        }
    }
    gs->map_threatened_stale = true;
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

void player_turn(GAMESTATE *gs) {
    set_map_threatened(gs);
    for(int i = 0; i < MAX_PLR_UNITS; i++) {
        gs->units_plr[i].can_act = true;
    }
}

void change_map(INPUTSTATE *is, GAMESTATE *gs, MAP map) {
    gs->map = map;
    if(map != mpmainmenu) {
        init_unit_status(gs, map);
        player_turn(gs);
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

    if(gs->attacker_combat_anim == 0 && gs->defender_combat_anim == 0 && is->anim_frame == 0 && gs->menu != uifight) { // check if units are dead
        gs->forecast.defender_index = 0;
        gs->forecast.attacker_index = 0;
        set_map_threatened(gs);
    }

    if(!is->input_read) { // is->input != NOINPUT && !is->input_read) {
        if(is->mapmode) {
            switch(is->input) {
                case A:
                    if(gs->selected_unit != 0) {
                        //if(gs->map_units[is->cursor_map_y][is->cursor_map_x] == gs->selected_unit
                        //|| (gs->map_units[is->cursor_map_y][is->cursor_map_x] == 0 && (*MPTERRAINDATA[gs->map])[is->cursor_map_y][is->cursor_map_x] != impassable)) {
                        if(gs->map_canmove[is->cursor_map_y][is->cursor_map_x] == gs->selected_unit) {
                            gs->menu = uiact;
                            is->cursor_menu_pos = 0;
                            is->mapmode = false;
                        }
                        //} else if(gs->map_units[is->cursor_map_y][is->cursor_map_x] > MAX_PLR_UNITS) { // enemy selected
                        //    fight(is, gs);
                    } else if(gs->selected_unit == 0) {
                        gs->selected_unit = gs->map_units[is->cursor_map_y][is->cursor_map_x];
                        if(gs->selected_unit > 0 && gs->selected_unit <= MAX_PLR_UNITS && gs->units_plr[gs->selected_unit - 1].can_act) {
                            gs->selected_unit_map_x = is->cursor_map_x;
                            gs->selected_unit_map_y = is->cursor_map_y;
                            memset16(gs->map_canmove, 0, (10 * 15) / 2);
                            gs->map_canmove_stale = true;
                            set_map_canmove(gs, gs->selected_unit, 3, gs->units_plr[gs->map_units[is->cursor_map_y][is->cursor_map_x] - 1].unit_attr->job == bow ? 2 : 1, is->cursor_map_x, is->cursor_map_y, false);
                        } else {
                            gs->selected_unit = 0;
                        }
                    }
                    break;
                case B:
                    if(gs->selected_unit != 0) {
                        gs->selected_unit = 0;
                        memset16(gs->map_canmove, 0, (10 * 15) / 2);
                        gs->map_canmove_stale = true;
                    }
                    break;
                default:
            }
        } else {
            switch(is->input) {
                case A:
                    (*(MENUDATA[gs->menu].choice_functions[is->cursor_menu_pos]))(is, gs);
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