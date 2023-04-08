#pragma once

#include "input.h"
#include "ui.h"
#include "map_data.h"
#include "enum.h"

#include <stdio.h>
#include <tonc.h> // debug

#define CART_RAM ((SAVE_DATA*) 0x0E000000)

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

// the input and output of the gamestate() function
typedef struct GAMESTATE {
    SAVE_DATA data;
    UI menu;

    MAP map;
    UNIT_STATUS units_plr[MAX_PLR_UNITS];
    UNIT_STATUS units_enemy[MAX_ENEMY_UNITS];
    int map_units[10][15]; // 0: empty, 1 — MAX_PLR_UNITS: units_plr, MAX_PLR_UNITS+1 — MAX_ENEMY_UNITS: units_enemy
    
    int selected_unit;
    int selected_unit_map_x;
    int selected_unit_map_y;

    int targetcount;
    int targetindex;
    int targets[12];
    char *forecast_attacker_name;
    char *forecast_attacker_weapon;
    int forecast_attacker_hp;
    int forecast_attacker_damage;
    int forecast_attacker_chance;
    char *forecast_defender_name;
    char *forecast_defender_weapon;
    int forecast_defender_hp;
    int forecast_defender_damage;
    int forecast_defender_chance;
} GAMESTATE;

void fight();
void damage();
void item();
void wait();

void forecast(GAMESTATE *gs, int attacker_index, int cursor_index);
void get_targets(GAMESTATE *gs, int range, int x, int y);

int gameinit(GAMESTATE* gamestate);

int game(unsigned int frame, INPUTSTATE* inputstate, GAMESTATE* gamestate);