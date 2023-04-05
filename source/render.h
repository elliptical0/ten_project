#pragma once

#include <tonc.h>
#include <tonc_types.h>
#include <tonc_memmap.h>

typedef struct IMGDATA {
    const void *tiles;
    int tileslen;
    const void *pal;
    int pallen;
} IMGDATA;


// tilesets ---------------------------------------------------
#include "tsui.h"
#include "tsfield.h"

typedef enum TILESET { // value of enum is a corresponding index in TSDATA
    tsui = 0,
    tsfield = 1
} TILESET;
extern const IMGDATA TSDATA[];


// sprites ----------------------------------------------------
#include "curs.h"
#include "metr.h"
#include "test.h"

typedef enum SPRITE { // value of enum is a corresponding index in SPDATA and SPATTR
    awaw,metr,test
} SPRITE;
extern const IMGDATA SPDATA[];
extern const u16 SPATTR[][3];


// maps -------------------------------------------------------
#include "map0.h"

typedef struct MAPDATA {
    const void *map;
    int maplen;
    TILESET ts;
} MAPDATA;
extern const MAPDATA MAPSDATA[];
typedef enum MAP { // value of enum is a corresponding index in MAPSDATA
    testmap = 0
} MAP;


// ui ---------------------------------------------------------
#include "ui.h"

typedef struct UIELEMENT {
    const unsigned short *ui;
    int width;
    int height;
} UIELEMENT;
typedef enum UI { // value of enum is a corresponding index in UIDATA
    act = 0
} UI;
extern const UIELEMENT UIDATA[];


// environment, ui, and text background#, charblock#, screenblock#, pal#
#define ENV_BG 0
#define ENV_CB 2
#define ENV_SB 11

#define UI_BG 2
#define UI_CB 3
#define UI_SB 12
#define UI_PAL 14

#define TEXT_BG 3
#define TEXT_CB 0
#define TEXT_SB 10

// set background control registers
#define env_bgcnt BG_PRIO(3) | BG_CBB(ENV_CB) | BG_4BPP | BG_SBB(ENV_SB) | BG_SIZE(0)
#define ui_bgcnt BG_PRIO(2) | BG_CBB(UI_CB) | BG_4BPP | BG_SBB(UI_SB) | BG_SIZE(0)
#define text_bgcnt BG_PRIO(0) | BG_CBB(TEXT_CB) | BG_SBB(TEXT_SB) | BG_SIZE(0)

// set display control register
#define dcnt DCNT_MODE0 | DCNT_BG0 | DCNT_BG2| DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D

// animation properties
#define CURS_SPD (16/CURSOR_ANIM_FRAMES) // cursor speed, pixels per frame

//#include "input.h"
#include "game.h"

#define BG_TILE_START = 0;

// the input and output of the render() function
typedef struct RENDERSTATE {
    OBJ_ATTR *metroid; // temp, for demo
} RENDERSTATE;

int renderinit(RENDERSTATE* renderstate);

int render(unsigned int frame, INPUTSTATE* inputstate, GAMESTATE* gamestate, RENDERSTATE* renderstate);
