#include <tonc.h>

#include <stdlib.h>

#include "input.h"
#include "game.h"
#include "render.h"

int main(void)
{
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
