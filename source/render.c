#include "render.h"

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

int renderinit(RENDERSTATE* renderstate) {
    // copied from load_sprite
    memcpy32(&tile_mem[4][0], metrTiles, metrTilesLen / 4);
	memcpy32(pal_obj_mem, metrPal, metrPalLen / 4);

	oam_init(obj_buffer, 128);

	renderstate->metroid = &obj_buffer[0];
	obj_set_attr(renderstate->metroid,
		ATTR0_SQUARE,  // Square, regular sprite
		ATTR1_SIZE_64, // 64x64 pixels,
		ATTR2_PALBANK(0) | 0); // palette index 0, tile index 0

	// Set position
	obj_set_pos(renderstate->metroid, 176, 96);

	oam_copy(oam_mem, obj_buffer, 1); // Update first OAM object

    // copied from right after load_sprite
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

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