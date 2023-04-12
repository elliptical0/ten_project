#pragma once

#include "input.h"
#include "ui.h"
#include "map_data.h"
#include "enum.h"
#include "rng.h"

#include <stdio.h>
#include <tonc_tte.h> // debug

#define RANGE(a, x, y) (a < x ? x : (a > y ? y : a))

#define CART_RAM ((SAVE_DATA*) 0x0E000000)

// enemy targeting weights
#define ENEMY_HP_WEIGHT 8000
#define ENEMY_HIT_WEIGHT 2000
// enemy movement weights
#define ENEMY_THREATENING_WEIGHT 128
#define ENEMY_SAFE_WEIGHT 64
#define ENEMY_APPROACHING_WEIGHT 32 // should be greater than 25

typedef struct MENU {
    int choices;
    void ((**choice_functions)());
    UI previous;
} MENU;
extern const MENU MENUDATA[];

#include "units.h"

typedef struct SAVE_DATA {
    MAP map; // if map == 0 (mpfield), save data does not exist!
    UNIT_ATTR units[MAX_PLR_UNITS];
} SAVE_DATA;

typedef struct UNIT_STATUS {
    int hp;
    int weapon; // inventory index
    int equipment; // inventory index
    bool can_act;
    UNIT_ATTR *unit_attr;
} UNIT_STATUS;

typedef struct FORECAST {
    int attacker_index;
    //char *attacker_name;
    char *attacker_weapon;
    int attacker_hp;
    int attacker_damage;
    int attacker_chance;
    int defender_index;
    //char *defender_name;
    char *defender_weapon;
    int defender_hp;
    int defender_damage;
    int defender_chance;
} FORECAST;

// the input and output of the gamestate() function
typedef struct GAMESTATE {
    SAVE_DATA data;
    UI menu;

    MAP map;
    UNIT_STATUS units_plr[MAX_PLR_UNITS];
    UNIT_STATUS units_enemy[MAX_ENEMY_UNITS]; // right after units_plr, so units_plr[map_units[y][x]] will give the right player *or* enemy unit
    int map_units[10][15]; // 0: empty, 1 — MAX_PLR_UNITS: units_plr, MAX_PLR_UNITS+1 — MAX_ENEMY_UNITS: units_enemy
    
    int selected_unit; // player AND enemy
    int selected_unit_map_x;
    int selected_unit_map_y;
    char map_canmove[10][15]; // 0: cant move to this tile, 1: can move to this tile
    bool map_canmove_stale; // for rendering
    char map_threatened[10][15];
    bool map_threatened_stale;

    int targetcount;
    int targetindex;
    int targets[12];
    int target_locations[12][2]; // [i][0] is x, [i][1] is y
    FORECAST forecast;
    int attacker_combat_anim; //0: none, 1: hit, 2: miss
    int defender_combat_anim; //0: none, 1: hit, 2: miss

    bool enemy_phase;
} GAMESTATE;

void fight();
void damage();
void item();
void wait();

void forecast(GAMESTATE *gs, int attacker_index, int cursor_index);
void get_targets(GAMESTATE *gs, int range, int x, int y);
void set_map_threatened(GAMESTATE *gs, bool ally);

void player_turn(GAMESTATE *gs);
void enemy_turn(INPUTSTATE *is, GAMESTATE *gs);


int gameinit(GAMESTATE* gamestate);

int game(unsigned int frame, INPUTSTATE* inputstate, GAMESTATE* gamestate);