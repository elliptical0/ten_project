
//{{BLOCK(mpMainMenu)

//======================================================================
//
//	mpMainMenu, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 65 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 32 + 2080 + 2048 = 4160
//
//	Time-stamp: 2023-04-07, 16:20:31
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MPMAINMENU_H
#define GRIT_MPMAINMENU_H

#define mpMainMenuTilesLen 2080
extern const unsigned int mpMainMenuTiles[520];

#define mpMainMenuMapLen 2048
extern const unsigned short mpMainMenuMap[1024];

#define mpMainMenuPalLen 32
extern const unsigned short mpMainMenuPal[16];

#endif // GRIT_MPMAINMENU_H

//}}BLOCK(mpMainMenu)
