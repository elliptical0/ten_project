#include "render.h"

const IMGDATA TSDATA[] = { // tileset data
    {tsuiTiles, tsuiTilesLen, tsuiPal, tsuiPalLen},
    {mpfieldTiles, mpfieldTilesLen, mpfieldPal, mpfieldPalLen},
    {mpMainMenuTiles, mpMainMenuTilesLen, mpMainMenuPal, mpMainMenuPalLen}
};
const MAPDATA MPDATA[] = { // map data
    {mpfieldMap, mpfieldMapLen, tsfield},
    {mpMainMenuMap, mpMainMenuMapLen, tsMainMenu}
};
const UIELEMENT UIDATA[] = {
    {ui_act, ui_act_width, ui_act_height, ui_act_x, ui_act_y, ui_act_text, ui_act_lines}
};
const IMGDATA SPDATA[] = { // sprite data
    {cursTiles, cursTilesLen, cursPal, cursPalLen},
    {metrTiles, metrTilesLen, metrPal, metrPalLen},
    {testTiles, testTilesLen, testPal, testPalLen}
};
const u16 SPATTR[][3] = { // sprite attributes. a0, a1, a2 WITHOUT PALBANK OR TILE INDEX!!!
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_16, ATTR2_PRIO(0)},
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_64, ATTR2_PRIO(0)},
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_64, ATTR2_PRIO(0)}
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
UI loaded_menu = NOUI;
    
/**
 * loads the tileset into the provided background charblock (ENV_CB or UI_CB) and palette index
*/
void loadTileset(TILESET ts, int cb, int pal) {
    memcpy32(&tile_mem[cb][0], TSDATA[ts].tiles, TSDATA[ts].tileslen / 4);
    memcpy32(&pal_bg_mem[pal * 16], TSDATA[ts].pal, TSDATA[ts].pallen / 4);
}

/**
 * loads the map into memory
*/
void loadMap(MAP m) {
    memcpy32(&se_mem[ENV_SB], MPDATA[m].map, MPDATA[m].maplen / 4);
    loadTileset(MPDATA[m].ts, ENV_CB, ENV_PAL);
}

/**
 * loads the ui object into the ui screenblock at the given 
 * @param focus_cursor sets the cursor to the position of the UI element
 * @param flip_x moves the ui element from the left side to the right, or vice-versa
 * @param flip_y moves the ui element from the top to the bottom, or vice-versa
*/
void loadUI(UI ui, bool flip_x, bool flip_y, bool set_cursor) {
    int bg_x = flip_x ? UIDATA[ui].x : 30 - UIDATA[ui].x - UIDATA[ui].width;
    int bg_y = flip_y ? UIDATA[ui].y : 20 - UIDATA[ui].y - UIDATA[ui].height;
    if(ui != NOUI) {
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
 * loads the sprite into memory
 * @return the OBJ_ATTR for the sprite (should it return the index in obj_buffer instead? hmmm,,,)
*/
OBJ_ATTR* loadSprite(enum SPRITE s) {
    memcpy32(&tile_mem[4][spritememindex], SPDATA[s].tiles, SPDATA[s].tileslen / 4);
    memcpy32(&pal_obj_mem[pals * 16], SPDATA[s].pal, 8);
    OBJ_ATTR *out = &obj_buffer[objs];
    obj_set_attr(out, SPATTR[s][0], SPATTR[s][1], SPATTR[s][2] | ATTR2_PALBANK(pals) | spritememindex);
    objs++;
    spritememindex += SPDATA[s].tileslen / 32;
    pals++;
    return out;
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
    
    // initialize cursor
    cursormemindex = spritememindex;
    cursor = loadSprite(awaw);
    cursor_setmode(true); // set cursor to map mode
    obj_set_pos(cursor, 0, 0);

	//oam_copy(oam_mem, obj_buffer, 1); // Update first OAM object


    // set background control registers
    REG_BGCNT[ENV_BG] = env_bgcnt;
    REG_BGCNT[UI_BG] = ui_bgcnt;

    loadMap(mpMainMenu);
    loadTileset(tsui, UI_CB, UI_PAL);
        

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

   if(gs->menu != loaded_menu) {
        unloadUI(loaded_menu, is->cursor_map_x > 7, is->cursor_map_y > 5);
        loadUI(gs->menu, is->cursor_map_x > 7, is->cursor_map_y < 5, true);
        loaded_menu = gs->menu;
        if(loaded_menu == NOUI) {
            obj_set_pos(cursor, is->cursor_map_x * 16, is->cursor_map_y * 16);
            cursor_setmode(true);
        } else {
            cursor_setmode(false);
        }
    }
    if(is->mapmode) {
        switch(is->input) {
            case UP:
                obj_set_pos(cursor, is->cursor_map_x * 16, is->cursor_map_y * 16 + is->anim_frame * CURS_SPD);
                break;
            case DOWN:
                obj_set_pos(cursor, is->cursor_map_x * 16, is->cursor_map_y * 16 - is->anim_frame * CURS_SPD);
                break;
            case LEFT:
                obj_set_pos(cursor, is->cursor_map_x * 16 + is->anim_frame * CURS_SPD, is->cursor_map_y * 16);
                break;
            case RIGHT:
                obj_set_pos(cursor, is->cursor_map_x * 16 - is->anim_frame * CURS_SPD, is->cursor_map_y * 16);
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

    return 0;
}