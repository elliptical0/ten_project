#pragma once

typedef enum SPRITE { // value of enum is a corresponding index in SPDATA and SPATTR (render.c)
    awaw = 0,
    metr = 1,
    test = 2,
    chsword = 3
} SPRITE;

typedef enum MAP { // value of enum is a corresponding index in MAPSDATA (render.c)
    mpfield = 0,
    mpmainmenu = 1
} MAP;

typedef enum UI { // value of enum is a corresponding index in MENUS and UIDATA (render.c)
    act = 0,
    NOUI = -1
} UI;