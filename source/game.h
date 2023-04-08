#pragma once

#include "input.h"
#include "ui.h"
#include "map_data.h"
#include "enum.h"

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

// the input and output of the gamestate() function
typedef struct GAMESTATE {
    SAVE_DATA data;
    UI menu;
    MAP map;
    UNIT_STATUS units_plr[MAX_PLR_UNITS];
    UNIT_STATUS units_enemy[MAX_ENEMY_UNITS];
    int map_units[10][15]; // 0: empty, 1 — MAX_PLR_UNITS: units_plr, MAX_PLR_UNITS+1 — MAX_ENEMY_UNITS: units_enemy
} GAMESTATE;

void fight();
void item();
void wait();

int gameinit(GAMESTATE* gamestate);

int game(unsigned int frame, INPUTSTATE* inputstate, GAMESTATE* gamestate);