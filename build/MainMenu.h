
//{{BLOCK(MainMenu)

//======================================================================
//
//	MainMenu, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 65 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 32 + 2080 + 2048 = 4160
//
//	Time-stamp: 2023-04-07, 15:52:20
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MAINMENU_H
#define GRIT_MAINMENU_H

#define MainMenuTilesLen 2080
extern const unsigned int MainMenuTiles[520];

#define MainMenuMapLen 2048
extern const unsigned short MainMenuMap[1024];

#define MainMenuPalLen 32
extern const unsigned short MainMenuPal[16];

#endif // GRIT_MAINMENU_H

//}}BLOCK(MainMenu)
