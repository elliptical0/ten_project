#include "units.h"

const ITEM_ATTR ITEMDATA[] = {
    {
        "iron sword",     // name, up to ITEM_NAME_SIZE_BYTES - 1 characters
        weapon, // type
        2,      // str
        10,     // hit
        10,     // avo
        sword   // job
    }, {
        "steel sword",     // name
        weapon, // type
        4,      // str
        5,      // hit
        5,      // avo
        sword   // job
    }, {
        "silver sword",     // name
        weapon, // type
        6,      // str
        0,      // hit
        0,      // avo
        sword   // job
    }, {
        "rapier",       // name
        weapon, // type
        4,      // str
        10,     // hit
        20,     // avo
        sword   // job
    }, {
        "iron bow",
        weapon,
        2,
        10,
        10,
        bow
    }
};

const UNIT_ATTR UNITDATA_PLR[] = {
    {
        "sasha",    // name, up to UNIT_NAME_SIZE_BYTES - 1 characters
        chsword,    // sprite
        6,          // maxhp
        2,          // str
        100,        // hit
        10,         // avo
        sword,      // job
        {noskill, noskill, noskill, noskill}, // skills
        {iron_sword, noitem, noitem} // inventory
    }, {
        "bradley", 
        chsword,    // sprite
        5,          // maxhp
        3,          // str
        110,        // hit
        20,         // avo
        bow,      // job
        {noskill, noskill, noskill, noskill}, // skills
        {iron_bow, noitem, noitem} // inventory
    }
};

const UNIT_ATTR UNITDATA_ENEMY[] = { // inventory = {weapon, equipment, drop}
    {
        "ruffian",  // name, up to UNIT_NAME_SIZE_BYTES - 1 characters
        chsword,    // sprite
        4,          // maxhp
        1,          // str
        80,         // hit
        0,          // avo
        sword,      // job
        {noskill, noskill, noskill, noskill}, // skills
        {iron_sword, noitem, noitem} // inventory
    }, {
        "thief",  // name, up to UNIT_NAME_SIZE_BYTES - 1 characters
        chsword,    // sprite
        3,          // maxhp
        1,          // str
        80,         // hit
        10,          // avo
        sword,      // job
        {noskill, noskill, noskill, noskill}, // skills
        {iron_sword, noitem, noitem} // inventory
    }, {
        "assassin",
        chsword,
        3,
        1,
        90,
        15,
        bow,
        {noskill, noskill, noskill, noskill}, // skills
        {iron_bow, noitem, noitem} // inventory
    }
};