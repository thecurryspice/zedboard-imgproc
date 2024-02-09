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

volatile bool TIMER_INTR_FLG, trigger=true, horizontal = true;
volatile int colour_offset = 0, bar_width = BAR_WIDTH;
volatile uint32_t timer_counter = 0;
XScuGic InterruptController; /* Instance of the Interrupt Controller */
static XScuGic_Config *GicConfig;/* The configuration parameters of thecontroller */

XGpio xled_o, xbtn_o;
XScuGic xscugic_o;
XTmrCtr xtimer_o;

static int led_data;
static int btn_value;

//----------------------------------------------------
// PROTOTYPE FUNCTIONS
//----------------------------------------------------
static void Button_InterruptHandler(void *baseaddr_p);
static void Timer_InterruptHandler(void *baseaddr_p);
static int InterruptSystemSetup(XScuGic *XScuGicInstancePtr);
static int IntcInitFunction(u16 DeviceId, XTmrCtr *TmrInstancePtr, XGpio *GpioInstancePtr);

//----------------------------------------------------
// INTERRUPT HANDLER FUNCTIONS
// - called by the timer, button interrupt, performs
// - LED flashing
//----------------------------------------------------

void Button_InterruptHandler(void *InstancePtr)
{
	// Disable GPIO interrupts
	XGpio_InterruptDisable(&xbtn_o, BTN_INT);
	// Ignore additional button presses
	if ((XGpio_InterruptGetStatus(&xbtn_o) & BTN_INT) !=
			BTN_INT) {
			return;
		}
	btn_value = XGpio_DiscreteRead(&xbtn_o, 1);
	// Increment counter based on button value
	// Reset if centre button pressed
	if(btn_value != 1) {
		if(btn_value == 16)	{ 			// increase bar width
			bar_width += 2;
		} else if (btn_value == 2) {	// decrease bar width
			bar_width = MAX(bar_width-2, 2);
		} else if (btn_value == 4) {	// rotate colours left
			colour_offset++;
		} else if (btn_value == 8) {	// rotate colours right
			colour_offset = MAX(colour_offset-1, 0);
		}
	} else {
		colour_offset = 0;
		bar_width = BAR_WIDTH;
		horizontal = !horizontal;
	}
	trigger = true;
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
// MAIN FUNCTION
//----------------------------------------------------
int main2 (void)
{
//  int status;
//  //----------------------------------------------------
//  // INITIALIZE THE PERIPHERALS & SET DIRECTIONS OF GPIO
//  //----------------------------------------------------
//  // Initialise LEDs
//  status = XGpio_Initialize(&LEDInst, LEDS_DEVICE_ID);
//  if(status != XST_SUCCESS) return XST_FAILURE;
//  // Initialise Push Buttons
//  status = XGpio_Initialize(&xbtn_o, BTNS_DEVICE_ID);
//  if(status != XST_SUCCESS) return XST_FAILURE;
//  // Set LEDs direction to outputs
//  XGpio_SetDataDirection(&LEDInst, 1, 0x00);
//  // Set all buttons direction to inputs
//  XGpio_SetDataDirection(&xbtn_o, 1, 0xFF);
//
//  //----------------------------------------------------
//  // SETUP THE TIMER
//  //----------------------------------------------------
//  status = XTmrCtr_Initialize(&TMRInst, TMR_DEVICE_ID);
//  if(status != XST_SUCCESS) return XST_FAILURE;
//  XTmrCtr_SetHandler(&TMRInst, TMR_Intr_Handler, &TMRInst);
//  XTmrCtr_SetResetValue(&TMRInst, 0, TMR_LOAD);
//  XTmrCtr_SetOptions(&TMRInst, 0, XTC_INT_MODE_OPTION | XTC_AUTO_RELOAD_OPTION);
//
//  // Initialize interrupt controller
//  status = IntcInitFunction(INTC_DEVICE_ID, &TMRInst, &xbtn_o);
//  if(status != XST_SUCCESS) return XST_FAILURE;
//
//  XTmrCtr_Start(&TMRInst, 0);
//
//  while(1)
//  {
//  	  xil_printf("%d\r\n", led_data);
//  }
//
  return 0;
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
	status = XScuGic_Connect(&xscugic_o,
					  	  	 INTC_GPIO_INTERRUPT_ID,
					  	  	 (Xil_ExceptionHandler)Button_InterruptHandler,
					  	  	 (void *)GpioInstancePtr);
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
