#ifndef __HELPERS_H__
#define __HELPERS_H__

#define PROC_FREQ		(XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ)
#define US_FACTOR		(XPAR_PS7_CORTEXA9_0_CPU_CLK_FREQ_HZ/1000000)
#define UART_BASEADDR 	(XPS_UART1_BASEADDR)

#include "xil_io.h"
#include "xuartps_hw.h"

void delay_us(uint64_t us)
{
	uint64_t i = 0;
	// create a threshold to count to.
	// the `/4` is needed because one iteration of the do-while
	// takes 4 clocks. (nop, increment, compare, jump)
	// lowest granularity (`delay(0)`) is (6/PROC_FREQ) seconds.
	uint64_t thresh = (US_FACTOR*us) / 4;
	do
	{
		asm("nop");
		i++;
	} while(i < thresh);
}


void setup_uart()
{
	// setup UART for reading
	u8 inp = 0x00;
	u32 CntrlRegister;
	CntrlRegister = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_CR_OFFSET);

	XUartPs_WriteReg(UART_BASEADDR, XUARTPS_CR_OFFSET,
	  ((CntrlRegister & ~XUARTPS_CR_EN_DIS_MASK) |
	   XUARTPS_CR_TX_EN | XUARTPS_CR_RX_EN));
}

#endif // __HELPERS_H__
