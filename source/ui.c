#include "ui.h"

// screenblocks for UI objects

/* tileset: (add 0xe000 to use color palette 14)
 * 0: transparent
 * 1: middle
 * 2: upper-left corner
 * 3: upper edge
 * 4: upper-right corner
 * 5: left edge
 * 6: right edge
 * 7: bottom-left corner
 * 8: bottom edge
 * 9: bottom-right corner
 */

// sizes of UI elements MUST HAVE EVEN WIDTH AND HEIGHT!!! because of memcpy16 (i think)

const unsigned short ui_act[24] = {
    0xe002,0xe003,0xe003,0xe004,
    0xe005,0xe001,0xe001,0xe006,
    0xe005,0xe001,0xe001,0xe006,
    0xe005,0xe001,0xe001,0xe006,
    0xe005,0xe001,0xe001,0xe006,
    0xe007,0xe008,0xe008,0xe009
};
const char *ui_act_text[3] = {
    "Fight",
    "Item",
    "Wait"
};