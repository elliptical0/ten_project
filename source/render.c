#include "render.h"

#include "tonc_memmap.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;
    
/**
 * loads the tileset into memory
*/
void loadTileset(enum tileset ts) {
    switch(ts) {
        case field:
            memcpy32(&tile_mem[ENV_CB][0], tsfieldTiles, tsfieldTilesLen / 4);
            memcpy32(&pal_bg_mem[0], tsfieldPal, tsfieldPalLen / 4);
            break;
    }
}

void loadMap(enum map m) {
    switch(m) {
        case test:
            memcpy32(&se_mem[ENV_SB], map0, map0Length / 4);
            loadTileset(field);
            break;
    }
}

int renderinit(RENDERSTATE* renderstate) {
    // copied from load_sprite
    memcpy32(&tile_mem[4][0], metrTiles, metrTilesLen / 4);
	memcpy32(pal_obj_mem, metrPal, metrPalLen / 4);
    //memcpy32(&tile_mem[4][0], testTiles, testTilesLen / 4);
	//memcpy32(pal_obj_mem, testPal, testPalLen / 4);

	oam_init(obj_buffer, 128);

	renderstate->metroid = &obj_buffer[0];
	obj_set_attr(renderstate->metroid,
		ATTR0_SQUARE || ATTR0_8BPP,  // Square, regular sprite, 8 bits per pixel
		ATTR1_SIZE_64, // 64x64 pixels,
		ATTR2_PALBANK(0) | 0); // palette index 0, tile index 0

	// Set position
	obj_set_pos(renderstate->metroid, 176, 96);

	oam_copy(oam_mem, obj_buffer, 1); // Update first OAM object

    // copied from main
    irq_init(NULL);
	irq_enable(II_VBLANK);

    loadMap(test);

    // set background control registers
    REG_BGCNT[ENV_BG] = env_bgcnt;
    REG_BGCNT[UI_BG] = ui_bgcnt;

    // initialize text engine
	tte_init_chr4c_default(TEXT_BG, text_bgcnt);
	tte_set_pos(92, 68);
	tte_write(">-<");

    // set display control register
    REG_DISPCNT = dcnt;

    return 0;
}

int render(INPUTSTATE* inputstate, GAMESTATE* gamestate, RENDERSTATE* renderstate) {
    // pre-vsync code

    vid_vsync(); // VRAM should be updated during VBlank to prevent screen tearing
    // post-vsync code
    
    // tte_write(gamestate->ypos == 0 && gamestate->xpos == 0 ? "#{es, P:92,68}" : "#{es, P:92,68} moved"); // test

    obj_set_pos(renderstate->metroid, gamestate->xpos, gamestate->ypos); // set position
    oam_copy(oam_mem, obj_buffer, 1); // transfer back buffer to VRAM

    return 0;
}