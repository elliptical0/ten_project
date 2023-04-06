#include "render.h"

const IMGDATA TSDATA[] = { // tileset data
    {tsuiTiles, tsuiTilesLen, tsuiPal, tsuiPalLen},
    {mpfieldTiles, mpfieldTilesLen, mpfieldPal, mpfieldPalLen}
};
const MAPDATA MPDATA[] = { // map data
    {mpfieldMap, mpfieldMapLen, tsfield}
};
const UIELEMENT UIDATA[] = {
    {ui_act, ui_act_x, ui_act_y}
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

int spritememindex = 0;
u8 pals = 0;
OBJ_ATTR obj_buffer[128];
u8 objs = 0; // number of OBJ_ATTR currently in the obj_buffer
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

OBJ_ATTR *cursor;
    
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
 * loads the ui object into the ui screenblock at the given position
*/
void loadUI(UI ui, int bg_x, int bg_y) {
    for(int y = 0; y < UIDATA[ui].height; y++) { // load each row of tiles
        for(int x = 0; x < UIDATA[ui].width; x++) {
            memcpy16(&se_mem[UI_SB][(bg_y + y) * 32 + bg_x], &UIDATA[ui].ui[y * UIDATA[ui].width], UIDATA[ui].width);
        }
    }
}
/**
 * clears UI element from the screen by filling the ui screenblock with 0
*/
void unloadUI(UI ui, int bg_x, int bg_y) {
    for(int y = 0; y < UIDATA[ui].height; y++) { // load each row of tiles
        for(int x = 0; x < UIDATA[ui].width; x++) {
            memset16(&se_mem[UI_SB][(bg_y + y) * 32 + bg_x], 0, UIDATA[ui].width);
        }
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

int renderinit(RENDERSTATE* renderstate) {
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
    cursor = loadSprite(awaw);
    obj_set_pos(cursor, 0, 0);

	//oam_copy(oam_mem, obj_buffer, 1); // Update first OAM object


    // set background control registers
    REG_BGCNT[ENV_BG] = env_bgcnt;
    REG_BGCNT[UI_BG] = ui_bgcnt;

    loadMap(mpfield);
    loadTileset(tsui, UI_CB, UI_PAL);
        

    // copied from main
    irq_init(NULL);
	irq_enable(II_VBLANK);

    // initialize text engine
	tte_init_chr4c_default(TEXT_BG, text_bgcnt);
	//tte_set_pos(92, 68);
	//tte_write(">-<");

    // set display control register
    REG_DISPCNT = dcnt;

    return 0;
}

int render(unsigned int frame, INPUTSTATE* inputstate, GAMESTATE* gamestate, RENDERSTATE* renderstate) { // TODO: might not need frame parameter
    // pre-vsync code

    // cursor animation
    if(inputstate->mapmode) {
        switch(inputstate->input) {
            case A:
                loadUI(act, inputstate->cursor_map_x * 2 + 2, inputstate->cursor_map_y * 2 + 2);
                tte_set_color(0, 0xFFFF);
                tte_set_pos(inputstate->cursor_map_x * 16 + 20, inputstate->cursor_map_y * 16 + 18);
                tte_write("Fight");
                tte_set_pos(inputstate->cursor_map_x * 16 + 20, inputstate->cursor_map_y * 16 + 34);
                tte_write("Item");
                tte_set_pos(inputstate->cursor_map_x * 16 + 20, inputstate->cursor_map_y * 16 + 50);
                tte_write("Wait");
                inputstate->mapmode = false;
                break;
            case UP:
                obj_set_pos(cursor, inputstate->cursor_map_x * 16, inputstate->cursor_map_y * 16 + inputstate->anim_frame * CURS_SPD);
                break;
            case DOWN:
                obj_set_pos(cursor, inputstate->cursor_map_x * 16, inputstate->cursor_map_y * 16 - inputstate->anim_frame * CURS_SPD);
                break;
            case LEFT:
                obj_set_pos(cursor, inputstate->cursor_map_x * 16 + inputstate->anim_frame * CURS_SPD, inputstate->cursor_map_y * 16);
                break;
            case RIGHT:
                obj_set_pos(cursor, inputstate->cursor_map_x * 16 - inputstate->anim_frame * CURS_SPD, inputstate->cursor_map_y * 16);
                break;
            default:
        }
    } else {
        switch(inputstate->input) {
            case B:
                unloadUI(act, inputstate->cursor_map_x * 2 + 2, inputstate->cursor_map_y * 2 + 2);
                tte_erase_rect(inputstate->cursor_map_x * 16, inputstate->cursor_map_y * 16, inputstate->cursor_map_x * 16 + 64, inputstate->cursor_map_y * 16 + 96);
                inputstate->mapmode = true;
                break;
            default:
        }
    }

    // debug
    /*
    tte_erase_screen();
    tte_set_pos(92, 68);
    for(int i = 0; i < inputstate->anim_frame; i++) {
        tte_write("o");
    }
     */

    vid_vsync(); // VRAM should be updated during VBlank to prevent screen tearing
    // post-vsync code
    
    // tte_write(gamestate->ypos == 0 && gamestate->xpos == 0 ? "#{es, P:92,68}" : "#{es, P:92,68} moved"); // test
    oam_copy(oam_mem, obj_buffer, objs); // transfer back buffer to VRAM

    return 0;
}