
//Empty C++ Application
#include <stdio.h>
#include "xil_types.h"
#include "xtmrctr.h"
#include "xparameters.h"

#include "xil_io.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xscugic.h"
#include <cstdlib>

#include "colour_defs.h"
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define HEIGHT (1024)
#define WIDTH  (1280)
#define BAR_WIDTH (200)

volatile bool TIMER_INTR_FLG;
XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of thecontroller */
int NUM_BYTES_BUFFER = 5242880;

void Timer_InterruptHandler(XTmrCtr *data, u8 TmrCtrNumber)
{
	XTmrCtr_Stop(data,TmrCtrNumber);
	XTmrCtr_Reset(data,TmrCtrNumber);
	//Update Stuff
	TIMER_INTR_FLG = true;
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

int main()
{
	XTmrCtr TimerInstancePtr;
	int xStatus;
	//-----------Setup Timer Interrupt---------------------------------------

	xStatus = XTmrCtr_Initialize(&TimerInstancePtr,XPAR_AXI_TIMER_0_DEVICE_ID);

	XTmrCtr_SetHandler(&TimerInstancePtr,
	(XTmrCtr_Handler)Timer_InterruptHandler,
	&TimerInstancePtr);

	//Reset Values
	XTmrCtr_SetResetValue(&TimerInstancePtr,
	0, //Change with generic value
	0xFFF0BDC0);
	//0x23C34600);
//	0xDC3CB9FF);
	//Interrupt Mode and Auto reload
	XTmrCtr_SetOptions(&TimerInstancePtr,
	XPAR_AXI_TIMER_0_DEVICE_ID,
	(XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION ));

	xStatus=ScuGicInterrupt_Init(XPAR_PS7_SCUGIC_0_DEVICE_ID,&TimerInstancePtr);


	/*Enable the interrupt for the device and then cause (simulate) an interrupt so the handlers will be called*/
	XScuGic_Enable(&InterruptController, 61);
	XScuGic_SetPriorityTriggerType(&InterruptController, 61, 0xa0, 3);
	int loop = 0;
	int * vga_buffer_pointer = (int *)0x00900000;
	int * image1_pointer = (int *)0x018D2008;
	int * image2_pointer = (int *)0x020BB00C;
	int * image3_pointer = (int *)0x028A4010;
	int * image4_pointer = (int *)0x0308D014;
	int * image5_pointer = (int *)0x03876018;

	int (&img_buffer)[HEIGHT][WIDTH] = *reinterpret_cast<int (*)[HEIGHT][WIDTH]>(image1_pointer);
	for(int row = 0; row < HEIGHT; row++)
	{
		for(int col = 0; col < WIDTH; col++)
			img_buffer[row][col] = 0x00000000;
	}

	int bar_idx = 0;
	int prevCol = 0;
	for(int row = 0; row < HEIGHT; row++)
	{
		bar_idx = 0;
		prevCol = 0;
		for(int col = 0; col < WIDTH; col++)
		{
			img_buffer[row][col] = palette[bar_idx];
			if(col-prevCol > BAR_WIDTH)
			{
				bar_idx++;
				prevCol = col;
			}
		}
	}

	Xil_DCacheFlush();

	while(1)
	{
		XTmrCtr_Start(&TimerInstancePtr,0);
		while(TIMER_INTR_FLG == false){
		}

		TIMER_INTR_FLG = false;

		memcpy(vga_buffer_pointer, image1_pointer, NUM_BYTES_BUFFER);
//		if(loop == PALETTE_SIZE) loop = 0;

	}
	return 0;
}


//dow -data U:/ENSC894AdvDig/vga_tutorial_students/stage1.data 0x018D2008
//dow -data U:/ENSC894AdvDig/vga_tutorial_students/stage2.data 0x020BB00C
//dow -data U:/ENSC894AdvDig/vga_tutorial_students/stage3.data 0x028A4010
//dow -data U:/ENSC894AdvDig/vga_tutorial_students/stage4.data 0x0308D014
//dow -data U:/ENSC894AdvDig/vga_tutorial_students/stage5.data 0x03876018
//dow -data C:/Users/arb26/Pictures/red.data 0x018D2008
