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

#include "xil_types.h"
#include "xstatus.h"
#include "xllfifo.h"
#include "xaxidma.h"
#include "ff.h"

#include "intr.h"
#include "defs.h"
#include "display.h"
#include "textglyphs.h"
#include "helpers.h"
#include "gui.h"
#include "sdif.h"

int status = 0;

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
	Xil_Out32(IPC_MSG_LOC, 0);
	setup_gpio(&xled_o, &xbtn_o);
	setup_sw_timer(&xtimer_o, 0xDC3CB9FF);
	setup_uart();

	xil_printf("IPC_IMG0_LOC:\t0x%x\r\n", IPC_IMG0_LOC);
	xil_printf("IPC_IMG1_LOC:\t0x%x\r\n", IPC_IMG1_LOC);
	xil_printf("KERNEL_VALUES:\t0x%x\r\n", IPC_KERNEL_VALUES);
	xil_printf("KERNEL_SELECT:\t0x%x\r\n", IPC_KERNEL_SELECT);
	xil_printf("IPC_MSG_LOC:\t0x%x\r\n", IPC_MSG_LOC);

	// initialise the interrupt controller
	status = IntcInitFunction(INTC_DEVICE_ID, &xtimer_o, &xbtn_o);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// start timer
//	XTmrCtr_Start(&xtimer_o, 0);

	Xil_Out32(IPC_MSG_LOC, IPC_ARM0_TRIGGER);

	// get some pointers to play around with
	vga_buffer_p = (int *)0x00900000;
	img1D_p = (int *)0x018D2008;

	// reinterpret to 2D pointer
	int (&img_buffer)[HEIGHT][WIDTH] = *reinterpret_cast<int (*)[HEIGHT][WIDTH]>(img1D_p);
	thisImage_p = (image_t *)(0x020BB074+0x8C);//malloc(sizeof(image_t));

	// blank out frame once
	clear_frame(img_buffer);

	// wait for ARM1 init before loading card
	while(Xil_In32(IPC_MSG_LOC) != IPC_ARM1_TRIGGER);

	strcpy(files[0],"Loading Card...");
	bool sd_status = setup_sd_card();

//	Xil_Out32(KERNELS_BASE_ADDR, 0x1);
//	memcpy((void*)DATAIN_BASE_ADDR, (void*)thisImage_p, 320*240);

	blank_pane_file_selector(img_buffer);
	draw_main_page(img_buffer);
	update_frame();

	while(1)
	{
		gui_handler(img_buffer);
//		build_frame(img_buffer, colour_offset, bar_width);
		update_frame();
	}
	return 0;
}
