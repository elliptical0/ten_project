
//{{BLOCK(mpfield)

//======================================================================
//
//	mpfield, 256x256@4, 
//	+ palette 16 entries, not compressed
//	+ 58 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 32x32 
//	Total size: 32 + 1856 + 2048 = 3936
//
//	Time-stamp: 2023-04-07, 14:47:39
//	Exported by Cearn's GBA Image Transmogrifier, v0.9.2
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_MPFIELD_H
#define GRIT_MPFIELD_H

#define mpfieldTilesLen 1856
extern const unsigned int mpfieldTiles[464];

#define mpfieldMapLen 2048
extern const unsigned short mpfieldMap[1024];

#define mpfieldPalLen 32
extern const unsigned short mpfieldPal[16];

#endif // GRIT_MPFIELD_H

//}}BLOCK(mpfield)
