#pragma once

#include <tonc.h>
#include <tonc_types.h>
#include <tonc_memmap.h>

#include "enum.h"

typedef struct IMGDATA {
    const void *tiles;
    int tileslen;
    const void *pal;
    int pallen;
} IMGDATA;


// tilesets ---------------------------------------------------
#include "tsui.h"

typedef enum TILESET { // value of enum is a corresponding index in TSDATA
    tsui = 0,
    tsfield = 1,
    tsmainmenu = 2
} TILESET;
extern const IMGDATA TSDATA[];


// maps -------------------------------------------------------
#include "mpfield.h"
#include "mpMainMenu.h"

typedef struct MAPDATA {
    const void *map;
    int maplen;
    TILESET ts;
} MAPDATA;
extern const MAPDATA MPDATA[];


// ui ---------------------------------------------------------
#include "ui.h"

typedef struct UIELEMENT {
    const unsigned short *ui;
    int width;
    int height;
    int x;
    int y;
    const char **text;
    int lines;
} UIELEMENT;
extern const UIELEMENT UIDATA[];


// sprites ----------------------------------------------------
#include "curs.h"
#include "metr.h"
#include "test.h"
#include "chsword.h"

extern const IMGDATA SPDATA[];
extern const u16 SPATTR[][3];


// environment, ui, and text background#, charblock#, screenblock#, pal#
#define ENV_BG 0
#define ENV_CB 2
#define ENV_SB 11
#define ENV_PAL 0

#define UI_BG 2
#define UI_CB 3
#define UI_SB 12
#define UI_PAL 14

#define TEXT_BG 3
#define TEXT_CB 0
#define TEXT_SB 10

// player and enemy pal#
#define PLR_PAL1 10
#define PLR_PAL2 11
#define PLR_PAL3 12
#define ENEMY_PAL1 13
#define ENEMY_PAL2 14
#define ENEMY_PAL3 15

// set background control registers
#define env_bgcnt BG_PRIO(3) | BG_CBB(ENV_CB) | BG_4BPP | BG_SBB(ENV_SB) | BG_SIZE(0)
#define ui_bgcnt BG_PRIO(2) | BG_CBB(UI_CB) | BG_4BPP | BG_SBB(UI_SB) | BG_SIZE(0)
#define text_bgcnt BG_PRIO(0) | BG_CBB(TEXT_CB) | BG_SBB(TEXT_SB) | BG_SIZE(0)

// set display control register
#define dcnt DCNT_MODE0 | DCNT_BG0 | DCNT_BG2| DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D

// animation properties
#define CURS_SPD (16/CURSOR_ANIM_FRAMES) // cursor speed, pixels per frame

#include "game.h"

#define BG_TILE_START = 0;

// the input and output of the render() function
typedef struct RENDERSTATE {
    OBJ_ATTR *metroid; // temp, for demo
} RENDERSTATE;

int renderinit(RENDERSTATE* renderstate);

int render(unsigned int frame, INPUTSTATE* inputstate, GAMESTATE* gamestate, RENDERSTATE* renderstate);
