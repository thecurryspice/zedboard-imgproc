//Empty C++ Application
#include <stdio.h>
#include "xil_types.h"
#include "xtmrctr.h"
#include "xparameters.h"

#include "xil_io.h"
#include "xil_printf.h"
#include "xil_cache.h"
#include <cstdlib>
#include "defs.h"
#include "kernels.h"

#define USE_MASK

#define R_MASK (0x000000FF)
#define G_MASK (0xFF000000)
#define B_MASK (0x00FF0000)
#define R_SHFT (00)
#define G_SHFT (24)
#define B_SHFT (16)

int filter_pixel(image_t* src, uint32_t row, uint32_t col, kernel_3x3_t* kernel)
{
	int rsum=0, gsum=0, bsum=0, sum=0;
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			int mul = kernel->kern_pix[i][j];
#ifdef USE_MASK
			bsum += ((src->pixel[(row-1)+i][(col-1)+j]) & B_MASK) * mul;
			gsum += ((src->pixel[(row-1)+i][(col-1)+j]) & G_MASK) * mul;
			rsum += ((src->pixel[(row-1)+i][(col-1)+j]) & R_MASK) * mul;
#else
			bsum += ((src->pixel[(row-1)+i][(col-1)+j] >> B_SHFT) & 0xFF) * mul;
			gsum += ((src->pixel[(row-1)+i][(col-1)+j] >> G_SHFT) & 0xFF) * mul;
			rsum += ((src->pixel[(row-1)+i][(col-1)+j] >> R_SHFT) & 0xFF) * mul;
#endif
//			sum += src->pixel[(row-1)+i][(col-1)+j] * kernel->kern_pix[i][j];
		}
	}

#ifdef USE_MASK
	sum = (bsum & B_MASK) | (gsum & G_MASK) | (rsum & R_MASK);
#else
	sum = (bsum/9 << B_SHFT) | (gsum/9 << G_SHFT) | ((rsum/9) << R_SHFT);
#endif
	return sum;
}

void apply_filter(image_t* src, image_t* dst, kernel_3x3_t* kernel)
{
	for(int row = 0; row < IMG_HEIGHT; row++)
	{
		for(int col = 0; col < IMG_WIDTH; col++)
		{
			dst->pixel[row][col] = filter_pixel(src, row, col, kernel);
		}
	}
}

void process_image_cpu()
{
	image_t* srcImage_p = (image_t *)(IPC_IMG0_LOC);
	image_t* dstImage_p = (image_t *)(IPC_IMG1_LOC);
	xil_printf("ARM1: Processing Image\r\n");
	kernel_3x3_t* kernel;
	uint32_t kern_sel = Xil_In32(IPC_KERNEL_SELECT);
	kernel = &kernels[kern_sel];

	// load the custom kernel
	if(kern_sel == CUSTOM)
	{
		for(int i = 0; i < 3; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				kernel->kern_pix[i][j] = Xil_In8(IPC_KERNEL_VALUES + i*3 + j);
			}
		}
	}

	xil_printf("%d %d %d\r\n", kernel->kern_pix[0][0], kernel->kern_pix[0][1], kernel->kern_pix[0][2]);
	xil_printf("%d %d %d\r\n", kernel->kern_pix[1][0], kernel->kern_pix[1][1], kernel->kern_pix[1][2]);
	xil_printf("%d %d %d\r\n", kernel->kern_pix[2][0], kernel->kern_pix[2][1], kernel->kern_pix[2][2]);

	apply_filter(srcImage_p, dstImage_p, kernel);
}

int main()
{
	while(Xil_In32(IPC_MSG_LOC) != IPC_ARM0_TRIGGER);
	xil_printf("Starting ARM1\r\n");
	Xil_Out32(IPC_MSG_LOC, IPC_ARM1_TRIGGER);
	xil_printf("ARM1: Ready\r\n");
	while(1)
	{
		// wait for trigger from ARM0
		if(Xil_In32(IPC_MSG_LOC) != IPC_ARM0_TRIGGER)
		{
			continue;
		}
		else
		{
			// process whatever is in buffer
#ifdef HW_KERNEL
			process_image_kernel();
#else
			process_image_cpu();
#endif
			// notify done
			Xil_Out32(IPC_MSG_LOC, IPC_ARM1_TRIGGER);
		}

	}

	return 0;
}
