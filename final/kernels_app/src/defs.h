#ifndef __DEFS_H__
#define __DEFS_H__

#include "xparameters.h"

#define MIN(a,b) 	(((a)<(b))?(a):(b))
#define MAX(a,b) 	(((a)>(b))?(a):(b))
#define SWAP32(num)	(((num)&0xff00ff) | ((num>>16)&0xff00) | ((num<<16)&0xff000000))
#define SWAP32b(num)	(((num>>16)&0xffff) | ((num<<16)&0xffff0000))
#define SWAP32c(num)	(((num)&0xffff0000) | ((num<<8)&0xff00) | ((num>>8)&0xff))

#define HEIGHT 				(1024)
#define WIDTH  				(1280)
#define IMG_HEIGHT 			(240)
#define IMG_WIDTH 			(320)
#define IMG_SIZE			(IMG_WIDTH * IMG_HEIGHT * 4)		// bytes
#define NUM_BYTES_BUFFER 	(HEIGHT*WIDTH*4)
#define IMG_STORE_OFFSET 	(35)

#define DATAIN_BASE_ADDR	(XPAR_AXI_FIFO_MM_S_DINA_BASEADDR)
#define KERNELS_BASE_ADDR	(XPAR_AXI_FIFO_MM_S_KERNELS_BASEADDR)
#define DATAOUT_BASE_ADDR	(0x02400000)

#define IPC_ARM1_TRIGGER	(0xDEADBEEF)
#define IPC_ARM0_TRIGGER	(0xBADCAFFE)

#define ARM1_BASE_ADDR		(0x17F40000)
// draw addressing layout for shared memory
#define IPC_IMG0_LOC		(ARM1_BASE_ADDR + 0x6F00000)
#define IPC_IMG1_LOC		(IPC_IMG0_LOC + IMG_SIZE)
#define IPC_KERNEL_VALUES	(IPC_IMG1_LOC + IMG_SIZE + 0x10)
#define IPC_KERNEL_SELECT	(IPC_KERNEL_VALUES + 0x30)
#define IPC_MSG_LOC			(IPC_KERNEL_SELECT + 0x10)


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


typedef struct __attribute__((__packed__)){
	int pixel[IMG_HEIGHT][IMG_WIDTH];
} image_t;
image_t* thisImage_p;

typedef struct {
	uint32_t header[35];
	image_t thisImage_p;
}filebuffer_t;
filebuffer_t* filebuffer_p;

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
volatile uint32_t timer_counter = 0, btn_counter = 0, kernel_counter = 0;

#endif
