#include "render.h"

IMGDATA TSDATA[] = { // tileset data
    {&tsfieldTiles, tsfieldTilesLen, &tsfieldPal, tsfieldPalLen}
};
IMGDATA SPDATA[] = { // sprite data
    {&cursTiles, cursTilesLen, &cursPal, cursPalLen},
    {&metrTiles, metrTilesLen, &metrPal, metrPalLen},
    {&testTiles, testTilesLen, &testPal, testPalLen}
};
u16 SPATTR[][3] = { // sprite attributes. a0, a1, a2 WITHOUT PALBANK OR TILE INDEX!!!
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_16, ATTR2_PRIO(0)},
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_64, ATTR2_PRIO(0)},
    {ATTR0_SQUARE || ATTR0_4BPP, ATTR1_SIZE_64, ATTR2_PRIO(0)}
};
MAPDATA MAPSDATA[] = { // map data
    {map0, map0Length, field}
};

int spritememindex = 0;
u8 pals = 0;
OBJ_ATTR obj_buffer[128];
u8 objs = 0; // number of OBJ_ATTR currently in the obj_buffer
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

OBJ_ATTR *cursor;
OBJ_ATTR *metroid;
    
/**
 * loads the tileset into memory
*/
void loadTileset(tileset ts) {
    memcpy32(&tile_mem[ENV_CB][0], TSDATA[ts].tiles, TSDATA[ts].tileslen / 4);
    memcpy32(&pal_bg_mem[0], TSDATA[ts].pal, TSDATA[ts].pallen / 4);
}

/**
 * loads the map into memory
*/
void loadMap(map m) {
    memcpy32(&se_mem[ENV_SB], MAPSDATA[m].map, MAPSDATA[m].maplen / 4);
    loadTileset(MAPSDATA[m].ts);
}

/**
 * loads the sprite into memory
 * @return the OBJ_ATTR for the sprite (should it return the index in obj_buffer instead? hmmm,,,)
*/
OBJ_ATTR* loadSprite(enum sprite s) {
    memcpy32(&tile_mem[4][spritememindex], SPDATA[s].tiles, SPDATA[s].tileslen / 4);
    memcpy32(&pal_obj_mem[pals * 16], SPDATA[s].pal, 8);
    OBJ_ATTR *out = &obj_buffer[objs];
    obj_set_attr(out, SPATTR[s][0], SPATTR[s][1], SPATTR[s][2] | ATTR2_PALBANK(pals) | spritememindex);
    objs++;
    spritememindex += SPDATA[s].tileslen / 32;
    pals++;
    return out;
}

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

    metroid = loadSprite(metr);
    //obj_set_pos(loadSprite(metr), 0, 0);
    obj_set_pos(metroid, 0, 0);

	//oam_copy(oam_mem, obj_buffer, 1); // Update first OAM object

    loadMap(testmap);

    // set background control registers
    REG_BGCNT[ENV_BG] = env_bgcnt;
    REG_BGCNT[UI_BG] = ui_bgcnt;

    // copied from main
    irq_init(NULL);
	irq_enable(II_VBLANK);

    // initialize text engine
	tte_init_chr4c_default(TEXT_BG, text_bgcnt);
	tte_set_pos(92, 68);
	//tte_write(">-<");

    // set display control register
    REG_DISPCNT = dcnt;

    return 0;
}

int render(INPUTSTATE* inputstate, GAMESTATE* gamestate, RENDERSTATE* renderstate, int s) {
    // pre-vsync code
    
    if(s == 0){
    obj_set_pos(cursor, inputstate->cursor_map_x * 16, inputstate->cursor_map_y * 16); // set position
    }

    if(s == 1){
    obj_set_pos(metroid, inputstate->metr_map_x * 16, inputstate->metr_map_y * 16);
    }

    vid_vsync(); // VRAM should be updated during VBlank to prevent screen tearing
    // post-vsync code
    
    // tte_write(gamestate->ypos == 0 && gamestate->xpos == 0 ? "#{es, P:92,68}" : "#{es, P:92,68} moved"); // test
    oam_copy(oam_mem, obj_buffer, objs); // transfer back buffer to VRAM

    return 0;
}