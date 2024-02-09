#ifndef __COLOUR_DEFS__
#define __COLOUR_DEFS__

//typedef enum {
//	RED 	= 0x000000F0,
//	GREEN 	= 0x0000F000,
//	BLUE 	= 0x00F00000,
//	YELLOW 	= 0x0000F0F0,
//	CYAN 	= 0x00F0F000,
//	PINK 	= 0x00F000F0,
//	WHITE	= 0x00F0F0F0,
//	GRAY 	= 0x08008080,
//	ORANGE 	= 0x000080F0,
//	MAGENTA	= 0x00F00080,
//	LIME	= 0x0000F080
//} colour;

typedef enum {
	RED,
	GREEN,
	BLUE,
	YELLOW,
	CYAN,
	PINK,
	ORANGE,
	MAGENTA,
	LIME,
	WHITE,
	GRAY,
	BLACK,
	OLIVE,
	PALETTE_SIZE
} colour;


int palette[PALETTE_SIZE] = {
		0x000000F0,
		0x0000F000,
		0x00F00000,
		0x0000F0F0,
		0x00F0F000,
		0x00F000F0,
		0x000080F0,
		0x00F00080,
		0x0000F080,
		0x00F0F0F0,
		0x00808080,
		0x00000000,
		0x00008080,
};


#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#endif
