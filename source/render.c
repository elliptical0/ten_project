#include "render.h"
#include "sound.h"

const IMGDATA TSDATA[] = { // tileset data
    {tsuiTiles, tsuiTilesLen, tsuiPal, tsuiPalLen},
    {mpfieldTiles, mpfieldTilesLen, mpfieldPal, mpfieldPalLen},
    {mpMainMenuTiles, mpMainMenuTilesLen, mpMainMenuPal, mpMainMenuPalLen}
};
const MAPDATA MPDATA[] = { // map data
    {mpfieldMap, mpfieldMapLen, tsfield},
    {mpMainMenuMap, mpMainMenuMapLen, tsmainmenu}
};
const UIELEMENT UIDATA[] = {
    {ui_act, ui_act_width, ui_act_height, ui_act_x, ui_act_y, ui_act_text, ui_act_lines},
    {ui_fight, ui_fight_width, ui_fight_height, ui_fight_x, ui_fight_y, NULL, 0}
};
const IMGDATA SPDATA[] = { // sprite data
    {cursTiles, cursTilesLen, cursPal, cursPalLen},
    {metrTiles, metrTilesLen, metrPal, metrPalLen},
    {testTiles, testTilesLen, testPal, testPalLen},
    {chswordTiles, chswordTilesLen, NULL, 0}
};
const u16 SPATTR[][3] = { // sprite attributes. a0, a1, a2 WITHOUT PALBANK OR TILE INDEX!!!
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_16, ATTR2_PRIO(0)},
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_64, ATTR2_PRIO(0)},
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_64, ATTR2_PRIO(0)},
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_16, ATTR2_PRIO(2)},
};

// vram variables
int spritememindex = 0;
u8 pals = 0;
OBJ_ATTR obj_buffer[128];
u8 objs = 0; // number of OBJ_ATTR currently in the obj_buffer
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

// cursor variables
OBJ_ATTR *cursor;
int cursormemindex = 0;
#define cursor_setmode(n) cursor->attr2 = cursormemindex + n ? 0 : 4
int cursor_menu_x;
int cursor_menu_y;

// background variables
UI loaded_menu = NOUI;
int loaded_forecast;
MAP loaded_map = -1;

/**
 * loads the sprite into memory
 * @param mode 0: load into current spritememindex (ignore index param)
 * @param mode 1: load as a player unit (at index param)
 * @param mode 2: load as an enemy unit (at index param)
 * @return the OBJ_ATTR for the sprite (should it return the index in obj_buffer instead? hmmm,,,)
*/
OBJ_ATTR* loadSprite(enum SPRITE s, int mode, int index) {
    int memindex, palindex, objindex;
    switch(mode) {
        case 1: // player unit
            memindex = 512 - ((MAX_ENEMY_UNITS + MAX_PLR_UNITS - index) * 4);
            palindex = PLR_PAL1;
            objindex = 128 - MAX_ENEMY_UNITS - MAX_PLR_UNITS + index;
            break;
        case 2: // enemy unit
            memindex = 512 - ((MAX_ENEMY_UNITS - index) * 4);
            palindex = ENEMY_PAL1;
            objindex = 128 - MAX_ENEMY_UNITS + index;
            break;
        default:
            memindex = spritememindex;
            palindex = pals;
            objindex = objs;
            memcpy32(&pal_obj_mem[palindex * 16], SPDATA[s].pal, 8); // copy palette
            break;
    }

    memcpy32(&tile_mem[4][memindex], SPDATA[s].tiles, SPDATA[s].tileslen / 4); // copy tiles
    OBJ_ATTR *out = &obj_buffer[objindex];
    obj_set_attr(out, SPATTR[s][0], SPATTR[s][1], SPATTR[s][2] | ATTR2_PALBANK(palindex) | memindex);
    
    if(mode == 0) {
        objs++;
        spritememindex += SPDATA[s].tileslen / 32;
        pals++;
    }
    return out;
}
    
/**
 * loads the tileset into the provided background charblock (ENV_CB or UI_CB) and palette index
*/
void loadTileset(TILESET ts, int cb, int pal) {
    memcpy32(&tile_mem[cb][0], TSDATA[ts].tiles, TSDATA[ts].tileslen / 4);
    memcpy32(&pal_bg_mem[pal * 16], TSDATA[ts].pal, TSDATA[ts].pallen / 4);
}

/**
 * loads the map into memory and unloads sprites
*/
void loadMap(GAMESTATE* gs) {
    //memset16(obj_buffer, 0, 128 * 4); // empty obj_buffer
    spritememindex = 0;
    objs = 0;
    pals = 0;
    memcpy32(&se_mem[ENV_SB], MPDATA[gs->map].map, MPDATA[gs->map].maplen / 4);
    loadTileset(MPDATA[gs->map].ts, ENV_CB, ENV_PAL);

    memcpy32(&pal_obj_mem[PLR_PAL1 * 16], palplr1Pal, 8); // load player unit palette #1
    memcpy32(&pal_obj_mem[PLR_PAL2 * 16], palplr2Pal, 8); // load player unit palette #2
    memcpy32(&pal_obj_mem[PLR_PAL3 * 16], palplr3Pal, 8); // load player unit palette #3
    memcpy32(&pal_obj_mem[ENEMY_PAL1 * 16], palenemy1Pal, 8); // load enemy unit palette #1

    if(gs->map != mpmainmenu) {
        // load cursor
        cursormemindex = spritememindex;
        cursor = loadSprite(awaw, 0, 0);
        obj_set_pos(cursor, 0, 0);

        // load player and enemy unit sprites
        for(int i = 0; i < MAX_PLR_UNITS && gs->units_plr[i].hp != 0; i++) {
            loadSprite(gs->units_plr[i].unit_attr->sprite, 1, i);
        }
        for(int i = 0; i < MAX_ENEMY_UNITS && i < MPENEMYDATA_LEN[gs->map]; i++) {
            loadSprite(gs->units_enemy[i].unit_attr->sprite, 2, i);
        }
    }
}

void updateForecast(GAMESTATE* gs, bool flip_x, bool flip_y, bool erase) {
    int bg_x = flip_x ? UIDATA[uifight].x : 30 - UIDATA[uifight].x - UIDATA[uifight].width;
    int bg_y = flip_y ? UIDATA[uifight].y : 20 - UIDATA[uifight].y - UIDATA[uifight].height;

    if(erase) {
        tte_erase_rect(bg_x * 8, bg_y * 8, bg_x * 8 + UIDATA[uifight].width * 8, bg_y * 8 + UIDATA[uifight].height * 8);
    }

    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 2);
    tte_write(gs->forecast_attacker_name);
    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 12);
    tte_write(gs->forecast_attacker_weapon);
    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 56);
    tte_write(gs->forecast_defender_name);
    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 66);
    tte_write(gs->forecast_defender_weapon);

    char buffer[4];

    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 23);
    sprintf(buffer, "%d", gs->forecast_defender_hp);
    tte_write(buffer);

    tte_set_pos((bg_x * 8) + 25, (bg_y * 8) + 23);
    tte_write("HP");

    tte_set_pos((bg_x * 8) + 42, (bg_y * 8) + 23);
    sprintf(buffer, "%d", gs->forecast_attacker_hp);
    tte_write(buffer);


    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 34);
    sprintf(buffer, "%d", gs->forecast_defender_damage);
    tte_write(buffer);

    tte_set_pos((bg_x * 8) + 25, (bg_y * 8) + 34);
    tte_write("Str");

    tte_set_pos((bg_x * 8) + 42, (bg_y * 8) + 34);
    sprintf(buffer, "%d", gs->forecast_attacker_damage);
    tte_write(buffer);


    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 45);
    sprintf(buffer, "%d", gs->forecast_defender_chance);
    tte_write(buffer);

    tte_set_pos((bg_x * 8) + 25, (bg_y * 8) + 45);
    tte_write("Hit");

    tte_set_pos((bg_x * 8) + 42, (bg_y * 8) + 45);
    sprintf(buffer, "%d", gs->forecast_attacker_chance);
    tte_write(buffer);

    loaded_forecast = gs->targetindex;
}

/**
 * loads the ui object into the ui screenblock at the given 
 * @param focus_cursor sets the cursor to the position of the UI element
 * @param flip_x moves the ui element from the left side to the right, or vice-versa
 * @param flip_y moves the ui element from the top to the bottom, or vice-versa
*/
void loadUI(GAMESTATE *gs, UI ui, bool flip_x, bool flip_y, bool set_cursor) {
    int bg_x = flip_x ? UIDATA[ui].x : 30 - UIDATA[ui].x - UIDATA[ui].width;
    int bg_y = flip_y ? UIDATA[ui].y : 20 - UIDATA[ui].y - UIDATA[ui].height;
    switch(ui) {
        case NOUI:
            break;
        case uifight:
            updateForecast(gs, flip_x, flip_y, false);
        default:
            for(int y = 0; y < UIDATA[ui].height; y++) { // load each row of tiles
                for(int x = 0; x < UIDATA[ui].width; x++) {
                    memcpy16(&se_mem[UI_SB][(bg_y + y) * 32 + bg_x], &UIDATA[ui].ui[y * UIDATA[ui].width], UIDATA[ui].width);
                }
            }
            for(int i = 0; i < UIDATA[ui].lines; i++) {
                tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + (i * 16) + 2); // MAGIC NUMBER ALERT
                tte_write(UIDATA[ui].text[i]);
            }
    }
    if(set_cursor) {
        cursor_menu_x = (bg_x - 2) * 8;
        cursor_menu_y = bg_y * 8;
        obj_set_pos(cursor, cursor_menu_x, cursor_menu_y);
    }
}
/**
 * clears UI element from the screen by filling the ui screenblock with 0
 * @param flip_x MUST be the same as when loadUI was called
 * @param flip_y MUST be the same as when loadUI was called
*/
void unloadUI(UI ui, bool flip_x, bool flip_y) {
    int bg_x = flip_x ? UIDATA[ui].x : 30 - UIDATA[ui].x - UIDATA[ui].width;
    int bg_y = flip_y ? UIDATA[ui].y : 20 - UIDATA[ui].y - UIDATA[ui].height;
    if(ui != NOUI) {
        for(int y = 0; y < UIDATA[ui].height; y++) { // load each row of tiles
            for(int x = 0; x < UIDATA[ui].width; x++) {
                memset16(&se_mem[UI_SB][(bg_y + y) * 32 + bg_x], 0, UIDATA[ui].width);
            }
        }
        tte_erase_rect(bg_x * 8, bg_y * 8, bg_x * 8 + UIDATA[ui].width * 8, bg_y * 8 + UIDATA[ui].height * 8);
    }
}

/**
 * 
*/
int renderinit(RENDERSTATE* rs) {
    // copied from load_sprite

    //memcpy32(&tile_mem[4][0], testTiles, testTilesLen / 4);
    //memcpy32(pal_obj_mem, testPal, testPalLen / 4);

    oam_init(obj_buffer, 128);

    /*
    renderstate->metroid = &obj_buffer[0];
    obj_set_attr(renderstate->metroid,
        ATTR0_SQUARE || ATTR0_8BPP,  // Square, regular sprite, 8 bits per pixel
        ATTR1_SIZE_64, // 64x64 pixels,
        ATTR2_PALBANK(0) | 0); // palette index 0, tile index 0
    */

    //obj_set_pos(loadSprite(test), 0, 0);

	//oam_copy(oam_mem, obj_buffer, 1); // Update first OAM object
    
    // set background control registers
    REG_BGCNT[ENV_BG] = env_bgcnt;
    REG_BGCNT[UI_BG] = ui_bgcnt;

    loadTileset(tsui, UI_CB, UI_PAL); // load UI tileset

    // copied from main
    irq_init(NULL);
	irq_enable(II_VBLANK);

    // initialize text engine
	tte_init_chr4c_default(TEXT_BG, text_bgcnt);
    tte_set_color(0, 0xFFFF);
	//tte_set_pos(92, 68);
	//tte_write(">-<");

    // set display control register
    REG_DISPCNT = dcnt;

    return 0;
}

//int map_x, map_y, bg_x, bg_y, screen_x, screen_y;
int render(unsigned int frame, INPUTSTATE* is, GAMESTATE* gs, RENDERSTATE* rs) { // TODO: might not need frame parameter
    // pre-vsync code
    /*
    map_x = inputstate->cursor_map_x;
    map_y = inputstate->cursor_map_y;
    bg_x = map_x * 2;
    bg_y = map_y * 2;
    screen_x = bg_x * 8;
    screen_y = bg_y * 8;
    */

    if(gs->map != loaded_map) { // change map
        loadMap(gs);
        loaded_map = gs->map;
        switch(loaded_map) {
            case mpmainmenu:
                break;
            default:
                cursor_setmode(true);
        }
    }

    if(gs->menu != loaded_menu) {
        unloadUI(loaded_menu, is->cursor_map_x > 7, is->cursor_map_y < 5);
        loadUI(gs, gs->menu, is->cursor_map_x > 7, is->cursor_map_y < 5, true);
        loaded_menu = gs->menu;
        if(loaded_menu == NOUI) {
            obj_set_pos(cursor, is->cursor_map_x * 16, is->cursor_map_y * 16);
            cursor_setmode(true);
        } else {
            cursor_setmode(false);
        }
    } else if(gs->menu == uifight && loaded_forecast != gs->targetindex) {
        updateForecast(gs, is->cursor_map_x > 7, is->cursor_map_y < 5, true);
    }

    // update unit sprites
    OBJ_ATTR *obj;
    for(int y = 0; y < 10; y++) {
        for(int x = 0; x < 15; x++) {
            if(gs->map_units[y][x] != 0) {
                obj = &obj_buffer[(gs->map_units[y][x] + 127 - MAX_ENEMY_UNITS - MAX_PLR_UNITS)];
                //if(gs->map_units[y][x] == 10) {
                //    char buffer[4];
                //    sprintf(buffer, "%d", (obj->attr2 & ATTR2_PALBANK_MASK) >> ATTR2_PALBANK_SHIFT);
                //    tte_write(buffer);
                //}
                obj_set_pos(obj, x * 16, y * 16);
                if(gs->map_units[y][x] <= MAX_PLR_UNITS) {
                    obj->attr2 = (obj->attr2 & ~ATTR2_PALBANK_MASK) | ATTR2_PALBANK(gs->units_plr[gs->map_units[y][x] - 1].can_act ? PLR_PAL1 : PLR_PAL2); // make sprites that cant act grey
                }
            }
        }
    }
    if(gs->selected_unit != 0) {
        obj = &obj_buffer[gs->selected_unit + 127 - MAX_ENEMY_UNITS - MAX_PLR_UNITS];
        obj_set_pos(obj, is->cursor_map_x * 16, is->cursor_map_y * 16);
        obj->attr2 = (obj->attr2 & ~ATTR2_PALBANK_MASK) | ATTR2_PALBANK(((gs->map_units[is->cursor_map_y][is->cursor_map_x] != 0 && gs->map_units[is->cursor_map_y][is->cursor_map_x] != gs->selected_unit) || (*MPTERRAINDATA[gs->map])[is->cursor_map_y][is->cursor_map_x] == impassable) ? PLR_PAL3 : PLR_PAL1); // change palette if tile is impassable
    }

    // update cursor sprite (will replace unit sprite update for selected unit)
    if(is->mapmode) {
        switch(is->input) {
            case UP:
                obj_set_pos(cursor, is->cursor_map_x * 16, is->cursor_map_y * 16 + is->anim_frame * CURS_SPD);
                if(gs->selected_unit != 0) {
                    obj_set_pos(&obj_buffer[gs->selected_unit + 127 - MAX_ENEMY_UNITS - MAX_PLR_UNITS], is->cursor_map_x * 16, is->cursor_map_y * 16 + is->anim_frame * CURS_SPD);
                }
                break;
            case DOWN:
                obj_set_pos(cursor, is->cursor_map_x * 16, is->cursor_map_y * 16 - is->anim_frame * CURS_SPD);
                if(gs->selected_unit != 0) {
                    obj_set_pos(&obj_buffer[gs->selected_unit + 127 - MAX_ENEMY_UNITS - MAX_PLR_UNITS], is->cursor_map_x * 16, is->cursor_map_y * 16 - is->anim_frame * CURS_SPD);
                }
                break;
            case LEFT:
                obj_set_pos(cursor, is->cursor_map_x * 16 + is->anim_frame * CURS_SPD, is->cursor_map_y * 16);
                if(gs->selected_unit != 0) {
                    obj_set_pos(&obj_buffer[gs->selected_unit + 127 - MAX_ENEMY_UNITS - MAX_PLR_UNITS], is->cursor_map_x * 16 + is->anim_frame * CURS_SPD, is->cursor_map_y * 16);
                }
                break;
            case RIGHT:
                obj_set_pos(cursor, is->cursor_map_x * 16 - is->anim_frame * CURS_SPD, is->cursor_map_y * 16);
                if(gs->selected_unit != 0) {
                    obj_set_pos(&obj_buffer[gs->selected_unit + 127 - MAX_ENEMY_UNITS - MAX_PLR_UNITS], is->cursor_map_x * 16 - is->anim_frame * CURS_SPD, is->cursor_map_y * 16);
                }
                break;
            default:
        }
    } else {
        switch(is->input) {
            case UP:
                obj_set_pos(cursor, cursor_menu_x, cursor_menu_y + is->cursor_menu_pos * 16 + is->anim_frame * CURS_SPD);
                break;
            case DOWN:
                obj_set_pos(cursor, cursor_menu_x, cursor_menu_y + is->cursor_menu_pos * 16 - is->anim_frame * CURS_SPD);
                break;
            default:
        }
    }

    // debug
    
    /*
    //tte_erase_screen();
    //tte_set_pos(92, 68);
    for(int i = 0; i < is->anim_frame; i++) {
        tte_write("o");
    }
     */
    

    vid_vsync(); // VRAM should be updated during VBlank to prevent screen tearing
    // post-vsync code
    
    // tte_write(gamestate->ypos == 0 && gamestate->xpos == 0 ? "#{es, P:92,68}" : "#{es, P:92,68} moved"); // test
    oam_copy(oam_mem, obj_buffer, objs); // transfer back buffer to VRAM
    oam_copy(oam_mem + 128 - MAX_ENEMY_UNITS - MAX_PLR_UNITS, obj_buffer + 128 - MAX_ENEMY_UNITS - MAX_PLR_UNITS, MAX_ENEMY_UNITS + MAX_PLR_UNITS); // transfer back buffer to VRAM

    return 0;
}