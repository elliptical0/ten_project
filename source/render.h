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

// tilesets
#include "tsfield.h"

typedef enum tileset { // value of enum is a corresponding index in TSDATA
    field = 0
} tileset;


// sprites
#include "metr.h"
#include "test.h"

typedef enum sprite { // value of enum is a corresponding index in SPDATA and SPATTR
    metr = 0, 
    test = 1
} sprite;


// maps
#include "map0.h"

typedef struct MAPDATA {
    const void *map;
    int maplen;
    tileset ts;
} MAPDATA;

typedef enum map { // value of enum is a corresponding index in MAPSDATA
    testmap = 0
} map;

// environment, ui, and text background#, charblock#, and screenblock#
#define ENV_BG 0
#define ENV_CB 0
#define ENV_SB 12

#define UI_BG 2
#define UI_CB 3
#define UI_SB 30

#define TEXT_BG 3
#define TEXT_CB 2
#define TEXT_SB 31

// set background control registers
#define env_bgcnt BG_PRIO(0) | BG_CBB(ENV_CB) | BG_4BPP | BG_SBB(ENV_SB) | BG_SIZE(0)
#define ui_bgcnt BG_PRIO(1) | BG_CBB(UI_CB) | BG_4BPP | BG_SBB(UI_SB) | BG_SIZE(0)
#define text_bgcnt BG_PRIO(2) | BG_CBB(TEXT_CB) | BG_SBB(TEXT_SB) | BG_SIZE(0)

// set display control register
#define dcnt DCNT_MODE0 | DCNT_BG0 | DCNT_BG2 | DCNT_BG3 | DCNT_OBJ | DCNT_OBJ_1D

//#include "input.h"
#include "game.h"

#define BG_TILE_START = 0;

// the input and output of the render() function
typedef struct RENDERSTATE {
    OBJ_ATTR *metroid; // temp, for demo
} RENDERSTATE;

int renderinit(RENDERSTATE* renderstate);

int render(INPUTSTATE* inputstate, GAMESTATE* gamestate, RENDERSTATE* renderstate);
