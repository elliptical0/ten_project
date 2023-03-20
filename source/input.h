#include <tonc_input.h> 

// the input and output of the input() function
typedef struct INPUTSTATE {
    int x;
    int y;
} INPUTSTATE;

int input(INPUTSTATE* inputstate);