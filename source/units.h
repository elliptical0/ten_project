#pragma once

#include <stdlib.h>

#include "enum.h"

#define ITEM_NAME_SIZE_BYTES 13
#define UNIT_NAME_SIZE_BYTES 9

#define MAX_PLR_UNITS 8
#define MAX_ENEMY_UNITS 16

#define INVENTORY_SPACES 3
#define SKILL_SLOTS 4

typedef enum JOB {
    jobless = 0,
    sword = 1,
    bow = 2
} JOB;
typedef enum SKILLS {
    noskill = 0
} SKILLS;

typedef enum ITEMS { // corresponds to an entry in ITEMDATA.
    iron_sword = 0,
    steel_sword = 1,
    silver_sword = 2,
    rapier = 3,
    iron_bow = 4,
    noitem = -1
} ITEMS;
typedef enum ITEM_TYPE {
    weapon = 0,
    equipment = 1
} ITEM_TYPE;
typedef struct ITEM_ATTR {
    char name[ITEM_NAME_SIZE_BYTES];
    char type;
    unsigned char str; // +str for weapons, +def for equipment
    unsigned char hit;
    unsigned char avo;
    unsigned char job;
} ITEM_ATTR;
//} __attribute__((__packed__)) ITEM_ATTR;
extern const ITEM_ATTR ITEMDATA[];

typedef struct UNIT_ATTR { // GBA storage has an 8-bit bus, so bytes are used!!
    char name[UNIT_NAME_SIZE_BYTES];
    char sprite;
    unsigned char maxhp;
    unsigned char str;
    unsigned char hit;
    unsigned char avo;
    char job;
    char skills[SKILL_SLOTS];
    char inventory[INVENTORY_SPACES];
} UNIT_ATTR;
//} __attribute__((__packed__)) UNIT_ATTR;

extern const UNIT_ATTR UNITDATA_PLR[];
extern const UNIT_ATTR UNITDATA_ENEMY[];