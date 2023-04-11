#include "ui.h"

// screenblocks for UI objects

/* tileset: (add 0xe000 to use color palette 14)
 * 0: transparent
 * 1: middle                neutral
 * 2: upper-left corner     neutral
 * 3: upper edge            neutral
 * 4: upper-right corner    neutral
 * 5: left edge             neutral
 * 6: right edge            neutral
 * 7: bottom-left corner    neutral
 * 8: bottom edge           neutral
 * 9: bottom-right corner   neutral
 *10: middle                player + enemy
 *11: middle                player
 *12: upper-left corner     player
 *13: upper edge            player
 *14: upper-right corner    player
 *15: left edge             player
 *16: right edge            player
 *17: bottom-left corner    player
 *18: bottom edge           player
 *19: bottom-right corner   player
 *
 *21: middle                enemy
 *22: upper-left corner     enemy
 *23: upper edge            enemy
 *24: upper-right corner    enemy
 *25: left edge             enemy
 *26: right edge            enemy
 *27: bottom-left corner    enemy
 *28: bottom edge           enemy
 *29: bottom-right corner   enemy
 */

// sizes of UI elements MUST HAVE EVEN WIDTH AND HEIGHT!!! because of memcpy16 (i think)

const unsigned short ui_act[] = {
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

const unsigned short ui_fight[] = {
    0xe012,0xe013,0xe013,0xe013,0xe013,0xe013,0xe013,0xe014,
    0xe015,0xe011,0xe011,0xe011,0xe011,0xe011,0xe011,0xe016,
    0xe015,0xe011,0xe011,0xe011,0xe011,0xe011,0xe011,0xe016,
    0xe025,0xe021,0xe021,0xe001,0xe001,0xe011,0xe011,0xe016,
    0xe025,0xe021,0xe021,0xe001,0xe001,0xe011,0xe011,0xe016,
    0xe025,0xe021,0xe021,0xe001,0xe001,0xe011,0xe011,0xe016,
    0xe025,0xe021,0xe021,0xe001,0xe001,0xe011,0xe011,0xe016,
    0xe025,0xe021,0xe021,0xe021,0xe021,0xe021,0xe021,0xe026,
    0xe025,0xe021,0xe021,0xe021,0xe021,0xe021,0xe021,0xe026,
    0xe027,0xe028,0xe028,0xe028,0xe028,0xe028,0xe028,0xe029
};