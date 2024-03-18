#ifndef __INTR_H__
#define __INTR_H__

#include "xparameters.h"
#include "xgpio.h"
#include "xtmrctr.h"
#include "xscugic.h"
#include "xil_exception.h"
#include "xil_printf.h"
#include "defs.h"

// Parameter definitions
#define INTC_DEVICE_ID 			XPAR_PS7_SCUGIC_0_DEVICE_ID
#define TMR_DEVICE_ID			XPAR_AXI_TIMER_0_DEVICE_ID/*XPAR_TMRCTR_0_DEVICE_ID*/
#define BTNS_DEVICE_ID			XPAR_AXI_GPIO_0_DEVICE_ID
#define LEDS_DEVICE_ID			XPAR_AXI_GPIO_0_DEVICE_ID
#define INTC_GPIO_INTERRUPT_ID 	XPAR_FABRIC_AXI_GPIO_0_IP2INTC_IRPT_INTR
#define INTC_TMR_INTERRUPT_ID 	XPAR_FABRIC_AXI_TIMER_0_INTERRUPT_INTR

#define BTN_INT 				XGPIO_IR_CH1_MASK
#define BAR_WIDTH				(100)

XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of thecontroller */

XGpio xled_o, xbtn_o;
XScuGic xscugic_o;
XTmrCtr xtimer_o;

static int led_data;

//----------------------------------------------------
// PROTOTYPE FUNCTIONS
//----------------------------------------------------
static void Button_InterruptHandler(void *baseaddr_p);
static void Timer_InterruptHandler(void *baseaddr_p);
static int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int IntcInitFunction(u16 DeviceId, XTmrCtr *TmrInstancePtr, XGpio *GpioInstancePtr);

void Button_InterruptHandler(void *InstancePtr)
{
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&xbtn_o, BTN_INT);
	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(&xbtn_o) & BTN_INT) !=
			BTN_INT) {
			return;
		}
	if(!pause_buttons)
	{
		btn_value = XGpio_DiscreteRead(&xbtn_o, 1);
	}
	pause_buttons = true;	// this lock is released from the GUI handler
//    XGpio_DiscreteWrite(&LEDInst, 1, led_data);
    (void)XGpio_InterruptClear(&xbtn_o, BTN_INT);
    // Enable GPIO interrupts
    XGpio_InterruptEnable(&xbtn_o, BTN_INT);
}

void Timer_InterruptHandler(void *InstancePtr)
{
	XTmrCtr_Stop(&xtimer_o,0);
	XTmrCtr_Reset(&xtimer_o,0);
	TIMER_INTR_FLG = true;
	timer_counter++;
//	colour_offset++;
//	trigger = true;
	XTmrCtr_Start(&xtimer_o,0);
}

//----------------------------------------------------
// INITIAL SETUP FUNCTIONS
//----------------------------------------------------
int InterruptSystemSetup(XScuGic *XScuGicInstancePtr)
{
	// Enable interrupt
	XGpio_InterruptEnable(&xbtn_o, BTN_INT);
	XGpio_InterruptGlobalEnable(&xbtn_o);
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			 	 	 	 	 	 (Xil_ExceptionHandler)XScuGic_InterruptHandler,
			 	 	 	 	 	 XScuGicInstancePtr);
	Xil_ExceptionEnable();
	return XST_SUCCESS;
}

int IntcInitFunction(u16 DeviceId, XTmrCtr *TmrInstancePtr, XGpio *GpioInstancePtr)
{
	XScuGic_Config *IntcConfig;
	int status;

	// Interrupt controller initialisation
	IntcConfig = XScuGic_LookupConfig(DeviceId);
	status = XScuGic_CfgInitialize(&xscugic_o, IntcConfig, IntcConfig->CpuBaseAddress);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Call to interrupt setup
	status = InterruptSystemSetup(&xscugic_o);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Connect GPIO interrupt to handler
#ifdef MILESTONE_1
	status = XScuGic_Connect(&xscugic_o,
					  	  	 INTC_GPIO_INTERRUPT_ID,
					  	  	 (Xil_ExceptionHandler)ButtonM1_InterruptHandler,
					  	  	 (void *)GpioInstancePtr);
#else
	// Connect GPIO interrupt to handler
	status = XScuGic_Connect(&xscugic_o,
							 INTC_GPIO_INTERRUPT_ID,
							 (Xil_ExceptionHandler)Button_InterruptHandler,
							 (void *)GpioInstancePtr);
#endif
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Connect timer interrupt to handler
	status = XScuGic_Connect(&xscugic_o,
							 INTC_TMR_INTERRUPT_ID,
							 (Xil_ExceptionHandler)Timer_InterruptHandler                                                                                                                                   ,
							 (void *)TmrInstancePtr);
	if(status != XST_SUCCESS) return XST_FAILURE;

	// Enable GPIO interrupts interrupt
	XGpio_InterruptEnable(GpioInstancePtr, 1);
	XGpio_InterruptGlobalEnable(GpioInstancePtr);

	// Enable GPIO and timer interrupts in the controller
	XScuGic_Enable(&xscugic_o, INTC_GPIO_INTERRUPT_ID);
	XScuGic_Enable(&xscugic_o, INTC_TMR_INTERRUPT_ID);
	return XST_SUCCESS;
}

#endif //__INTR_H__
