//Empty C++ Application
#include <stdio.h>
#include "xil_types.h"
#include "xtmrctr.h"
#include "xparameters.h"

#include "xil_io.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xgpio.h"
#include "xscugic.h"
#include <cstdlib>

#include "intr.h"
#include "defs.h"

#define HEIGHT (1024)
#define WIDTH  (1280)
#define NUM_BYTES_BUFFER (HEIGHT*WIDTH*4)

int status = 0;

void clear_frame(int (&img_buffer)[HEIGHT][WIDTH])
{
	for(int row = 0; row < HEIGHT; row++)
	{
		for(int col = 0; col < WIDTH; col++)
			img_buffer[row][col] = palette[BLUE];
	}

}

void build_frame(int (&img_buffer)[HEIGHT][WIDTH], int _colour_offset, int _bar_width)
{
	int bar_idx = 0;
	if(horizontal)
	{
		int prevRow = 0, bar_idx = 0;;
		// for each row, walk through the available palette based on the bar_idx
		int colour = (bar_idx+_colour_offset)%PALETTE_SIZE;
		for(int row = 0; row < HEIGHT; row++)
		{
			if(row-prevRow >= _bar_width)
			{
				// update bar_idx and colour when #columns for a bar are drawn
				bar_idx++;
				prevRow = row;
				colour = (bar_idx+_colour_offset)%PALETTE_SIZE;
			}
			for(int col = 0; col < WIDTH; col++)
			{
				// assign colour to this pixel
				img_buffer[row][col] = palette[colour];
			}
		}
	}
	else
	{
		int prevCol = 0;
		for(int row = 0; row < HEIGHT; row++)
		{
			// for each row, walk through the available palette based on the bar_idx
			bar_idx = 0;
			prevCol = 0;
			int colour = (bar_idx+_colour_offset)%PALETTE_SIZE;
			for(int col = 0; col < WIDTH; col++)
			{
				// assign colour to this pixel
				img_buffer[row][col] = palette[colour];
				if(col-prevCol >= _bar_width)
				{
					// update bar_idx and colour when #columns for a bar are drawn
					bar_idx++;
					prevCol = col;
					colour = (bar_idx+_colour_offset)%PALETTE_SIZE;
				}
			}
		}
	}
}

void update_frame(int* dst, int* src, size_t)
{
	memcpy(dst, src, NUM_BYTES_BUFFER);
	Xil_DCacheFlush();
}


int SetUpInterruptSystem(XScuGic *XScuGicInstancePtr){
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
	(Xil_ExceptionHandler) XScuGic_InterruptHandler,
	XScuGicInstancePtr);
	Xil_ExceptionEnable();
	return XST_SUCCESS;
}

/*Setup all interrupts of the system*/
int ScuGicInterrupt_Init(u16 DeviceId,XTmrCtr *TimerInstancePtr)
{
	int Status;
	GicConfig = XScuGic_LookupConfig(DeviceId);
	if (NULL == GicConfig) {
		return XST_FAILURE;
	}
	Status = XScuGic_CfgInitialize(&InterruptController, GicConfig,
	GicConfig->CpuBaseAddress);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	Status = SetUpInterruptSystem(&InterruptController);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}
	/*Connect a device driver handler that will be called when an interrupt for the device occurs, the device driver handler performs the specific interrupt processing for the device*/
	Status = XScuGic_Connect(&InterruptController,
	61,
	(Xil_ExceptionHandler)XTmrCtr_InterruptHandler,
	(void *)TimerInstancePtr);
	if (Status != XST_SUCCESS) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}


void setup_sw_timer(XTmrCtr* TimerInstancePtr, int timer_reset_value)
{
	//-----------Setup Timer Interrupt---------------------------------------
	status = XTmrCtr_Initialize(TimerInstancePtr,TMR_DEVICE_ID);
	XTmrCtr_SetHandler(TimerInstancePtr,(XTmrCtr_Handler)Timer_InterruptHandler,TimerInstancePtr);

	//Reset Values
	XTmrCtr_SetResetValue(TimerInstancePtr,0,timer_reset_value);
	//Interrupt Mode and Auto reload
	XTmrCtr_SetOptions(TimerInstancePtr,TMR_DEVICE_ID, (XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION ));
}

void setup_gpio(XGpio* xled_p, XGpio* xbtn_p)
{
//	status = XGpio_Initialize(xled_p, LEDS_DEVICE_ID);
//	if(status != XST_SUCCESS) return XST_FAILURE;
	status = XGpio_Initialize(xbtn_p, BTNS_DEVICE_ID);
//	if(status != XST_SUCCESS) return XST_FAILURE;

//	XGpio_SetDataDirection(xled_p, 1, 0x00);
	XGpio_SetDataDirection(xbtn_p, 1, 0xFF);
}

int main()
{
	setup_gpio(&xled_o, &xbtn_o);
	setup_sw_timer(&xtimer_o, 0xDC3CB9FF);

	// initialise the interrupt controller
	status = IntcInitFunction(INTC_DEVICE_ID, &xtimer_o, &xbtn_o);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// start timer
	XTmrCtr_Start(&xtimer_o, 0);

	// get some pointers to play around with
	int * vga_buffer_p = (int *)0x00900000;
	int * img1D_p = (int *)0x018D2008;

	// reinterpret to 2D pointer
	int (&img_buffer)[HEIGHT][WIDTH] = *reinterpret_cast<int (*)[HEIGHT][WIDTH]>(img1D_p);

	// blank out frame once
	clear_frame(img_buffer);
	update_frame(vga_buffer_p, img1D_p, NUM_BYTES_BUFFER);

	while(1)
	{
		if(trigger)
		{
			build_frame(img_buffer, colour_offset, bar_width);
//			Xil_DCacheFlush();
			trigger = false;
		}
		update_frame(vga_buffer_p, img1D_p, NUM_BYTES_BUFFER);
	}
	return 0;
}
