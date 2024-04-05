#ifndef __KERNELS_H__
#define __KERNELS_H__

typedef enum {
	SHARPEN,
	HORIZONTAL_EMBOSS,
	VERTICAL_EMBOSS,
	ALL_EMBOSS,
	CUSTOM,
	WRAP_KERNLIST_E
} kernlist_e;

typedef struct
{
	int kern_pix[3][3] = {0};
}kernel_3x3_t;

kernel_3x3_t kernels[WRAP_KERNLIST_E] = {
		{{{0,-1,0},	{-1,5,-1}, {0,-1,0}}},
		{{{0,0,0}, 	{1,0,-1},  {0,0,0}}},
		{{{0,1,0}, 	{0,0,0},   {0,-1,0}}},
		{{{-2,-1,0}, {-1,1,1},  {0,1,2}}},
		{{{0,0,0}, 	{0,0,0},   {0,0,0}}},
};

#endif // __KERNELS_H__
