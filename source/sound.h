#include <tonc.h>
#include <tonc_types.h>
#include <tonc_memmap.h>
#include <stdio.h>



typedef struct SOUNDSTATE {

    int octave;

} SOUNDSTATE;

int soundinit(SOUNDSTATE* SOUNDSTATE);

void note_play(int note, int octave);