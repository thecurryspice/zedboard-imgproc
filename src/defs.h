#ifndef __DEFS_H__
#define __DEFS_H__

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define HEIGHT (1024)
#define WIDTH  (1280)
#define NUM_BYTES_BUFFER (HEIGHT*WIDTH*4)


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
} colour_e;


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

#define BT_CR (1)
#define BT_UP (2)
#define BT_LT (4)
#define BT_RT (8)
#define BT_DN (16)


// display pointers
int * vga_buffer_p;
int * img1D_p;

// sd card stuff
#define MAX_FILES (32)
char files[MAX_FILES][32] = {0};
int filesNum = 0, file_select_index = 0;

// interrupts
volatile int btn_value;
volatile int last_btn_value;
volatile bool TIMER_INTR_FLG, pause_buttons=true, horizontal = false, skip_interrupt=false;
volatile uint32_t timer_counter = 0, btn_counter = 0;

#endif
