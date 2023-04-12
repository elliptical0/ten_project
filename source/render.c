#include "render.h"

const IMGDATA TSDATA[] = { // tileset data
    {tsuiTiles, tsuiTilesLen, tsuiPal, tsuiPalLen},
    {mpfieldTiles, mpfieldTilesLen, mpfieldPal, mpfieldPalLen},
    {mpislandTiles, mpislandTilesLen, mpislandPal, mpislandPalLen},
    {mpMainMenuTiles, mpMainMenuTilesLen, mpMainMenuPal, mpMainMenuPalLen}
};
const MAPDATA MPDATA[] = { // map data
    {mpfieldMap, mpfieldMapLen, tsfield},
    {mpislandMap, mpislandMapLen, tsisland},
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
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_16, ATTR2_PRIO(SPRITE_UNIT_PRIO)},
};

// vram variables
int spritememindex = 0;
u8 pals = 0;
OBJ_ATTR obj_buffer[128];
u8 objs = 0; // number of OBJ_ATTR currently in the obj_buffer
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;
#define unit_obj_attr(n) &obj_buffer[(n + 127 - MAX_ENEMY_UNITS - MAX_PLR_UNITS)]

// cursor variables
OBJ_ATTR *cursor;
int cursormemindex = 0;
#define cursor_setmode(n) cursor->attr2 = cursormemindex + n ? 0 : 4 // true: map mode, false: menu mode
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

    //cursor_setmode(true);
    obj_set_pos(cursor, gs->target_locations[gs->targetindex][0] * 16, gs->target_locations[gs->targetindex][1] * 16);
    
    if(erase) {
        tte_erase_rect(bg_x * 8, bg_y * 8, bg_x * 8 + UIDATA[uifight].width * 8, bg_y * 8 + UIDATA[uifight].height * 8);
    }

    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 2);
    tte_write(gs->units_plr[gs->forecast.attacker_index - 1].unit_attr->name);
    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 12);
    tte_write(gs->forecast.attacker_weapon);
    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 56);
    tte_write(gs->units_plr[gs->forecast.defender_index - 1].unit_attr->name);
    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 66);
    tte_write(gs->forecast.defender_weapon);

    char buffer[4];

    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 23);
    sprintf(buffer, "%d", gs->forecast.defender_hp);
    tte_write(buffer);

    tte_set_pos((bg_x * 8) + 25, (bg_y * 8) + 23);
    tte_write("HP");

    tte_set_pos((bg_x * 8) + 42, (bg_y * 8) + 23);
    sprintf(buffer, "%d", gs->forecast.attacker_hp);
    tte_write(buffer);


    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 34);
    sprintf(buffer, "%d", gs->forecast.defender_damage);
    tte_write(buffer);

    tte_set_pos((bg_x * 8) + 25, (bg_y * 8) + 34);
    tte_write("Str");

    tte_set_pos((bg_x * 8) + 42, (bg_y * 8) + 34);
    sprintf(buffer, "%d", gs->forecast.attacker_damage);
    tte_write(buffer);


    tte_set_pos((bg_x * 8) + 4, (bg_y * 8) + 45);
    sprintf(buffer, "%d", gs->forecast.defender_chance);
    tte_write(buffer);

    tte_set_pos((bg_x * 8) + 25, (bg_y * 8) + 45);
    tte_write("Hit");

    tte_set_pos((bg_x * 8) + 42, (bg_y * 8) + 45);
    sprintf(buffer, "%d", gs->forecast.attacker_chance);
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
    REG_BGCNT[GRID_BG] = grid_bgcnt;
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

    // set display control and blending registers
    REG_DISPCNT = dcnt;
    REG_BLDMOD = bldmod;
    REG_COLEV = colev;
    //REG_COLEY = coley;

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
        loadUI(gs, gs->menu, is->cursor_map_x > 7, is->cursor_map_y < 5, gs->menu != uifight);
        loaded_menu = gs->menu;
        if(loaded_menu == NOUI) {
            obj_set_pos(cursor, is->cursor_map_x * 16, is->cursor_map_y * 16);
            cursor_setmode(true);
        } else if(loaded_menu == uifight) {
            cursor_setmode(true);
        } else {
            cursor_setmode(false);
        }
    } else if(gs->menu == uifight && loaded_forecast != gs->targetindex) {
        updateForecast(gs, is->cursor_map_x > 7, is->cursor_map_y < 5, true);
    }

    OBJ_ATTR *obj;

    if(gs->map != mpmainmenu) {
    // update unit sprites

        // update unit sprites and grid overlay
        for(int y = 0; y < 10; y++) {
            for(int x = 0; x < 15; x++) {
                if(gs->map_units[y][x] != 0) { // unit sprites
                    obj = unit_obj_attr(gs->map_units[y][x]);
                    if(gs->units_plr[gs->map_units[y][x] - 1].hp <= 0 && gs->map_units[y][x] != gs->forecast.attacker_index && gs->map_units[y][x] != gs->forecast.defender_index) { // dead and not in combat animation
                        gs->map_units[y][x] = 0; // tbh this belongs in gamestate but im short on time
                        obj_set_attr(obj, ATTR0_HIDE, 0, 0);
                    }
                    //if(gs->map_units[y][x] == 10) {
                    //    char buffer[4];
                    //    sprintf(buffer, "%d", (obj->attr2 & ATTR2_PALBANK_MASK) >> ATTR2_PALBANK_SHIFT);
                    //    tte_write(buffer);
                    //}
                    obj_set_pos(obj, x * 16, y * 16);
                    if(!gs->enemy_phase) {
                        if(gs->map_units[y][x] <= MAX_PLR_UNITS) {
                            obj->attr2 = (obj->attr2 & ~ATTR2_PALBANK_MASK) | ATTR2_PALBANK(gs->units_plr[gs->map_units[y][x] - 1].can_act ? PLR_PAL1 : PLR_PAL2); // make sprites that cant act grey
                        }
                    }
                }
                if(gs->map_canmove_stale || gs->map_threatened_stale) { // grid overlay
                    se_mem[GRID_SB][y * 64 + x * 2] = gs->map_canmove[y][x] ? (gs->map_threatened[y][x] ? 0xe010 : 0xe011) : (gs->map_threatened[y][x] ? 0xe021 : 0); // same tileset as UI! check "ui.c"
                    se_mem[GRID_SB][y * 64 + x * 2 + 1] = gs->map_canmove[y][x] ? (gs->map_threatened[y][x] ? 0xe010 : 0xe011) : (gs->map_threatened[y][x] ? 0xe021 : 0); // same tileset as UI! check "ui.c"
                    se_mem[GRID_SB][y * 64 + 32 + x * 2] = gs->map_canmove[y][x] ? (gs->map_threatened[y][x] ? 0xe010 : 0xe011) : (gs->map_threatened[y][x] ? 0xe021 : 0); // same tileset as UI! check "ui.c"
                    se_mem[GRID_SB][y * 64 + 32 + x * 2 + 1] = gs->map_canmove[y][x] ? (gs->map_threatened[y][x] ? 0xe010 : 0xe011) : (gs->map_threatened[y][x] ? 0xe021 : 0); // same tileset as UI! check "ui.c"
                }
            }
        }
        gs->map_canmove_stale = false;
        gs->map_threatened_stale = false;

        if(!gs->enemy_phase && gs->selected_unit != 0) {
            obj = unit_obj_attr(gs->selected_unit);
            obj_set_pos(obj, is->cursor_map_x * 16, is->cursor_map_y * 16);
            obj->attr2 = (obj->attr2 & ~ATTR2_PALBANK_MASK) | ATTR2_PALBANK(gs->map_canmove[is->cursor_map_y][is->cursor_map_x] != 0 ? PLR_PAL1 : PLR_PAL3); // change palette if tile is impassable
        }
    }

    // update cursor sprite (will replace unit sprite update for selected unit)
    if(!gs->enemy_phase) {
        if(is->mapmode) {
            switch(is->input) {
                case UP:
                    obj_set_pos(cursor, is->cursor_map_x * 16, is->cursor_map_y * 16 + is->anim_frame * CURS_SPD);
                    if(gs->selected_unit != 0) {
                        obj_set_pos(unit_obj_attr(gs->selected_unit), is->cursor_map_x * 16, is->cursor_map_y * 16 + is->anim_frame * CURS_SPD);
                    }
                    break;
                case DOWN:
                    obj_set_pos(cursor, is->cursor_map_x * 16, is->cursor_map_y * 16 - is->anim_frame * CURS_SPD);
                    if(gs->selected_unit != 0) {
                        obj_set_pos(unit_obj_attr(gs->selected_unit), is->cursor_map_x * 16, is->cursor_map_y * 16 - is->anim_frame * CURS_SPD);
                    }
                    break;
                case LEFT:
                    obj_set_pos(cursor, is->cursor_map_x * 16 + is->anim_frame * CURS_SPD, is->cursor_map_y * 16);
                    if(gs->selected_unit != 0) {
                        obj_set_pos(unit_obj_attr(gs->selected_unit), is->cursor_map_x * 16 + is->anim_frame * CURS_SPD, is->cursor_map_y * 16);
                    }
                    break;
                case RIGHT:
                    obj_set_pos(cursor, is->cursor_map_x * 16 - is->anim_frame * CURS_SPD, is->cursor_map_y * 16);
                    if(gs->selected_unit != 0) {
                        obj_set_pos(unit_obj_attr(gs->selected_unit), is->cursor_map_x * 16 - is->anim_frame * CURS_SPD, is->cursor_map_y * 16);
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
    }

    // damage animation
    if(gs->map != mpmainmenu) {
        if(gs->attacker_combat_anim != 0 || gs->defender_combat_anim != 0) {
            int shake_x = rng_shake();
            int shake_y = rng_shake();
            obj = gs->attacker_combat_anim != 0 ? unit_obj_attr(gs->forecast.defender_index) : unit_obj_attr(gs->forecast.attacker_index);
            
            tte_erase_rect((obj->attr1 & ATTR1_X_MASK) - 2, obj->attr0 & ATTR0_Y_MASK, (obj->attr1 & ATTR1_X_MASK) + 17, (obj->attr0 & ATTR0_Y_MASK) + 17);
            if(gs->attacker_combat_anim != 0 ? gs->attacker_combat_anim == 1 : gs->defender_combat_anim == 1) { // hit
                if(is->anim_frame > 0) {
                    char buffer[4];
                    sprintf(buffer, "%d", gs->attacker_combat_anim != 0 ? gs->forecast.attacker_damage : gs->forecast.defender_damage);
                    tte_set_pos((obj->attr1 & ATTR1_X_MASK) + 5, (obj->attr0 & ATTR0_Y_MASK) + (is->anim_frame / 4) - 1);
                    tte_write(buffer);
                }
                obj_set_pos(obj, (obj->attr1 & ATTR1_X_MASK) + RANGE(shake_x, -(is->anim_frame / 4), (is->anim_frame / 4)), (obj->attr0 & ATTR0_Y_MASK) + RANGE(shake_y, -(is->anim_frame / 4), (is->anim_frame / 4)));
                //obj_set_attr(obj, obj->attr0, obj->attr1, (obj->attr2 & ~ATTR2_PRIO_MASK) + (is->anim_frame == 0 ? ATTR2_PRIO(3) : ATTR2_PRIO(1))); // increase prio bc the shaking anim will overlap with adjacent sprites
            } else { // miss
                if(is->anim_frame > 0) {
                    tte_set_pos((obj->attr1 & ATTR1_X_MASK) - 2, (obj->attr0 & ATTR0_Y_MASK) + (is->anim_frame / 4) - 1);
                    tte_write("miss");
                }
            }
            if(is->anim_frame == 0) {
                if(gs->attacker_combat_anim != 0) {
                    // part of death code
                    gs->attacker_combat_anim = 0;
                    if(gs->units_plr[gs->forecast.defender_index - 1].hp > 0) { // defender alive
                        is->anim_frame = HIT_ANIM_FRAMES - 1;
                    } else {
                        //gs->forecast.defender_index = 0;
                        //gs->forecast.attacker_index = 0; // part of death code
                    }
                }
                else if(gs->defender_combat_anim != 0) {
                    //gs->forecast.defender_index = 0;
                    //gs->forecast.attacker_index = 0; // part of death code
                    gs->defender_combat_anim = 0;
                }
            }
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