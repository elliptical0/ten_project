#include <stdio.h>
#include <tonc.h>
#include "sound.h"



// Plays given note at given octave
void note_play(int note, int octave)
{

    // Play the actual note
    REG_SND1FREQ = SFREQ_RESET | SND_RATE(note, octave);
    
}

int soundinit(SOUNDSTATE* ss){

REG_SNDSTAT= SSTAT_ENABLE;
// snd1 on left/right ; both full volume
REG_SNDDMGCNT = SDMG_BUILD_LR(SDMG_SQR1, 7);
// DMG ratio to 100%
REG_SNDDSCNT= SDS_DMG100;

// no sweep
REG_SND1SWEEP= SSW_OFF;

// envelope: vol=12, decay, max step time (1) ; 50% duty
REG_SND1CNT= SSQR_ENV_BUILD(12, 0, 1) | SSQR_DUTY1_2;

REG_SND1FREQ= 2;

return 0;
}