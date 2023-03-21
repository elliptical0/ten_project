#include <tonc.h>

#include <stdlib.h>

#include <tonc_types.h>

//#include "input.h"
//#include "game.h"
#include "render.h"

int main(void)
{
	irq_init(NULL);
	irq_enable(II_VBLANK);

	tte_init_chr4c_default(0, BG_CBB(0) | BG_SBB(31));
	tte_set_pos(92, 68);
	tte_write(">-<");

	// allocate the input and output structs for input, game, and render, and run init
	INPUTSTATE* inputstate = malloc(sizeof(INPUTSTATE));
	int inputerror = inputinit(inputstate); // TODO: possible error handling
	GAMESTATE* gamestate = malloc(sizeof(GAMESTATE));
	int gameerror = gameinit(gamestate);
	RENDERSTATE* renderstate = malloc(sizeof(RENDERSTATE));
	int rendererror = renderinit(renderstate);

	while (1) {
		// TODO: add error handling code (for nonzero output);
		inputerror = input(inputstate);
		// switch(inputerror) {
		gameerror = game(inputstate, gamestate);
		rendererror = render(inputstate, gamestate, renderstate); // vsync happens within here		
	}

	free(inputstate); // cleanup memory. probably not necessary but good housekeeping >_<
	free(gamestate);
	free(renderstate);
}
