#ifndef __SDIF_H__
#define __SDIF_H__

#include "defs.h"
#include "sd_card.h"

FATFS FS_instance;

typedef struct {
	char riff[4];
	u32 riffSize;
	char wave[4];
} headerWave_t;

typedef struct __attribute__((__packed__)){
	int pixel[320][240];
} image_t;

typedef struct {
	char ckId[4];
	u32 cksize;
} genericChunk_t;

typedef struct {
	u16 wFormatTag;
	u16 nChannels;
	u32 nSamplesPerSec;
	u32 nAvgBytesPerSec;
	u16 nBlockAlign;
	u16 wBitsPerSample;
	u16 cbSize;
	u16 wValidBitsPerSample;
	u32 dwChannelMask;
	u8 SubFormat[16];
} fmtChunk_t;

void read_image_file(const char *filename) {
    image_t* thisImage_p = (image_t*)malloc(sizeof(image_t));
    FIL file;
    UINT nBytesRead=0;

    FRESULT res = f_open(&file, filename, FA_READ);
    if (res!=0) {
    	xil_printf("File not found");
    }
    xil_printf("Loading %s\r\n",filename);

    // read file into memory buffer
    res = f_read(&file,(void*)thisImage_p,sizeof(image_t),&nBytesRead);
    if (res!=0) {
    	xil_printf("Failed to read file");
    }

    f_close(&file);
}

bool setup_sd_card()
{
    setvbuf(stdin, NULL, _IONBF, 0);

	print("Mounting SD Card\n\r");
	FRESULT result = f_mount(&FS_instance,"0:/", 1);
	if (result != 0) {
		xil_printf("Couldn't mount SD Card!\r\n");
		return false;
	}

//	// Wait for input from UART via the terminal
//	while (true)
//	{
//		while(!XUartPs_IsReceiveData(UART_BASEADDR));
//		inp = XUartPs_ReadReg(UART_BASEADDR, XUARTPS_FIFO_OFFSET);
//		if(inp == '\r')
//			break;
//	}
	// Look for *.bmp files and copy file names to files[]
	DIR dir;
	FRESULT res = f_opendir(&dir, "0:/");
	if (res != FR_OK) {
		print("Couldn't read root directory!\r\n");
	}
	for (;;) {
		FILINFO fno;
		res = f_readdir(&dir, &fno);
		if (res != FR_OK || fno.fname[0] == 0) {
			break;
		}
		if (fno.fattrib & AM_DIR) {
			// Do nothing. This is a directory.
		}
		else if (strstr(fno.fname,".bmp")!=NULL || strstr(fno.fname,".BMP")!=NULL) {
			// This is a BMP file
			xil_printf(fno.fname);
			xil_printf("\r\n");
			strcpy(files[filesNum++],fno.fname);
		}
		else {
			// It's some other uninteresting file
		}
	}
	f_closedir(&dir);

	if (filesNum == 0) {
		xil_printf("No BMP files found!\r\n");
		return false;
	}
	return true;
}


#endif // __SDIF_H__
