#pragma once

typedef enum SPRITE { // value of enum is a corresponding index in SPDATA and SPATTR (render.c)
    awaw = 0,
    metr = 1,
    test = 2,
    chsword = 3
} SPRITE;

typedef enum MAP { // value of enum is a corresponding index in MAPSDATA (render.c)
    mpfield = 0,
    mpisland = 1,
    mpmainmenu = 2
} MAP;

typedef enum UI { // value of enum is a corresponding index in MENUS and UIDATA (render.c)
    uiact = 0,
    uifight = 1,
    NOUI = -1
} UI;