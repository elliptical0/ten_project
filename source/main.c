#include <tonc.h>

// This file is autogenerated from the file in the graphics folder
#include "metr.h"

#include <tonc_types.h>
#include <tonc_input.h>

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

#define RANGE(a, x, y) if(a < 0) { a = 0; } else if(a > y) { a = y; }

OBJ_ATTR * load_sprite(void)
{
	// Load tiles and palette of sprite into video and palete RAM
	memcpy32(&tile_mem[4][0], metrTiles, metrTilesLen / 4);
	memcpy32(pal_obj_mem, metrPal, metrPalLen / 4);

	oam_init(obj_buffer, 128);

	OBJ_ATTR *metr = &obj_buffer[0];
	obj_set_attr(metr,
		ATTR0_SQUARE,  // Square, regular sprite
		ATTR1_SIZE_64, // 64x64 pixels,
		ATTR2_PALBANK(0) | 0); // palette index 0, tile index 0

	// Set position
	obj_set_pos(metr, 176, 96);

	oam_copy(oam_mem, obj_buffer, 1); // Update first OAM object

	return metr;
}

int main(void)
{
	irq_init(NULL);
	irq_enable(II_VBLANK);

	tte_init_chr4c_default(0, BG_CBB(0) | BG_SBB(31));
	tte_set_pos(92, 68);
	tte_write(">_<");

	OBJ_ATTR *metroid = load_sprite();
	int xpos = 176;
	int ypos = 96;

	REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_OBJ | DCNT_OBJ_1D;

	while (1) {
		// input
		key_poll();
		xpos += key_tri_horz();
		RANGE(xpos, 0, 176);
		ypos += key_tri_vert();
		RANGE(ypos, 0, 96);

		vid_vsync(); // sprite should be updated during VBlank to prevent screen tearing
		//VBlankIntrWait();

		// set position
		obj_set_pos(metroid, xpos, ypos);
		oam_copy(oam_mem, obj_buffer, 1); // transfer back buffer to VRAM
	}
}
