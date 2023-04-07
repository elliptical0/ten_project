#pragma once

#include "input.h"
#include "ui.h"
#include <tonc.h> // debugging. todo: remove

typedef enum MAP { // value of enum is a corresponding index in MAPSDATA (render.c)
    mpfield = 0
} MAP;

typedef enum UI { // value of enum is a corresponding index in MENUS and UIDATA (render.c)
    act = 0,
    NOUI = -1
} UI;
typedef struct MENU {
    int choices;
    void ((**choice_functions)());
    UI previous;
} MENU;
extern const MENU MENUDATA[];

typedef enum SPRITE { // value of enum is a corresponding index in SPDATA and SPATTR (render.c)
    awaw,metr,test
} SPRITE;

// the input and output of the gamestate() function
typedef struct GAMESTATE {
    UI menu;
} GAMESTATE;

void fight();
void item();
void wait();

int gameinit(GAMESTATE* gamestate);

int game(unsigned int frame, INPUTSTATE* inputstate, GAMESTATE* gamestate);