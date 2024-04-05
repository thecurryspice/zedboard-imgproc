#ifndef __GUI_H__
#define __GUI_H__

#include "defs.h"
#include "textglyphs.h"
#include "display.h"
#include "sdif.h"
#include "string.h"

typedef enum {
	MAIN_MENU,
	FILE_SELECT,
	KERNEL_SELECT,
	CUSTOM_KERNEL,
	WRAP_ACTIVE_E
} active_e;
active_e active_pane;

typedef enum {
	LOAD_IMAGE,
	APPLY_KERNEL,
	SET_CUSTOM_KERNEL,
	SAVE_IMAGE,
	REFRESH_FILE_LIST,
	WRAP_MAINLIST_E
} mainlist_e;
mainlist_e mainlist;

typedef enum {
	SHARPEN,
	HORIZONTAL_EMBOSS,
	VERTICAL_EMBOSS,
	ALL_EMBOSS,
	CUSTOM,
	WRAP_KERNLIST_E
} kernlist_e;
kernlist_e kernlist;

uint16_t highlight_index = 0;
bool quickupdate = false, do_action = false;

void blank_pane_file_selector(int (&img_buffer)[HEIGHT][WIDTH])
{
	fill_rect(img_buffer, 0, 0, WIDTH>>2, HEIGHT>>1, BLACK);
}

void blank_pane_kernel_selector(int (&img_buffer)[HEIGHT][WIDTH])
{
	fill_rect(img_buffer, 0, HEIGHT>>1, WIDTH>>2, 3*HEIGHT>>2, BLACK);
}

void blank_pane_main_menu(int (&img_buffer)[HEIGHT][WIDTH])
{
	fill_rect(img_buffer, 0, 3*HEIGHT>>2, WIDTH>>2, HEIGHT, BLACK);
}

void draw_pane_custom_kernel_passive(int (&img_buffer)[HEIGHT][WIDTH])
{
	uint32_t ycursor =  (3*HEIGHT>>2);
	uint32_t xcursor = ((WIDTH>>2) + 10);
	uint32_t padding = LINE_SPACING + ((TEXT_HEIGHT-TEXT_WIDTH)>>1);
	uint32_t kern_window_width  = (TEXT_WIDTH + 2*(padding)) * 3;
	uint32_t kern_window_height = (TEXT_HEIGHT + 2*LINE_SPACING) * 3;

	// caption
	fill_rect(img_buffer, xcursor-1, ycursor, (xcursor-1)+(kern_window_width+1), ycursor+2*LINE_SPACING, WHITE);
	ycursor += LINE_SPACING;
	write_text(img_buffer, (xcursor+10), ycursor, "CUSTOM KERNEL", BLACK);

	ycursor += LINE_SPACING+1;
	// boundary
	draw_rect(img_buffer, xcursor-1, ycursor-1, (xcursor-1)+(kern_window_width), (ycursor-1)+(kern_window_height), MAGENTA);

	// kernel values
	draw_hline(img_buffer, xcursor, ycursor + (1*kern_window_height/3), kern_window_width, WHITE);
	draw_hline(img_buffer, xcursor, ycursor + (2*kern_window_height/3), kern_window_width, WHITE);
	draw_vline(img_buffer, xcursor + (1*kern_window_width/3), ycursor, kern_window_height, WHITE);
	draw_vline(img_buffer, xcursor + (2*kern_window_width/3), ycursor, kern_window_height, WHITE);

	write_text(img_buffer, (xcursor+10), ycursor, "CUSTOM KERNEL", BLACK);
//	if(do_action && mainlist == CUSTOM_KERNEL)
//	{
//		Xil_Out32(IPC_KERNEL_SELECT, kernlist);
//		Xil_Out32(IPC_MSG_LOC, IPC_ARM0_TRIGGER);
//		}
//	}
}

void draw_pane_src_img(int (&img_buffer)[HEIGHT][WIDTH])
{
	uint32_t ycursor = (HEIGHT>>1)-(IMG_HEIGHT>>1);
	uint32_t xcursor = 400;
	// boundary
	draw_rect(img_buffer, xcursor-1, ycursor-1, (xcursor-1)+(IMG_WIDTH+1), (ycursor-1)+(IMG_HEIGHT+1), WHITE);

	// image
	if(do_action && mainlist == LOAD_IMAGE)
	{
		read_image_file(files[file_select_index]);
		draw_image(img_buffer, thisImage_p, xcursor, ycursor);
		// wait for trigger from ARM1
		if(Xil_In32(IPC_MSG_LOC) != IPC_ARM1_TRIGGER)
		{
			xil_printf("ARM0: ARM1 is still busy. Try again!\r\n");
		}
		else
		{
			Xil_Out32(IPC_KERNEL_SELECT, kernlist);
			Xil_Out32(IPC_MSG_LOC, IPC_ARM0_TRIGGER);
		}
	}
	ycursor += IMG_HEIGHT+1;
	// caption
	fill_rect(img_buffer, xcursor-1, ycursor, (xcursor-1)+(IMG_WIDTH+1)+1, ycursor+2*LINE_SPACING, WHITE);
	ycursor += LINE_SPACING;
	write_text(img_buffer, (xcursor+10), ycursor, "ORIGINAL IMAGE", BLACK);
}

void draw_pane_dst_img(int (&img_buffer)[HEIGHT][WIDTH])
{
	uint32_t ycursor = (HEIGHT>>1)-(IMG_HEIGHT>>1);
	uint32_t xcursor = 800;
	// boundary
	draw_rect(img_buffer, xcursor-1, ycursor-1, (xcursor-1)+(IMG_WIDTH+1), (ycursor-1)+(IMG_HEIGHT+1), WHITE);

	// image
	if(do_action && mainlist == APPLY_KERNEL)
	{
//		read_image_file(files[file_select_index]);
		draw_image(img_buffer, (image_t *)(IPC_IMG1_LOC), xcursor, ycursor);
	}
	ycursor += IMG_HEIGHT+1;
	// caption
	fill_rect(img_buffer, xcursor-1, ycursor, (xcursor-1)+(IMG_WIDTH+1)+1, ycursor+2*LINE_SPACING, WHITE);
	ycursor += LINE_SPACING;
	write_text(img_buffer, (xcursor+10), ycursor, "PROCESSED IMAGE", BLACK);
}

void draw_pane_file_selector(int (&img_buffer)[HEIGHT][WIDTH], bool highlight)
{
	colour_e colour = (highlight) ? GREEN : WHITE;
	uint32_t ycursor = 0;

	// boundary
	draw_rect(img_buffer, 0, ycursor, WIDTH>>2, HEIGHT>>1, colour);

	// header
	fill_rect(img_buffer, 2, ycursor+1, (WIDTH>>2)-1, 2*LINE_SPACING, colour);
	ycursor += LINE_SPACING;
	write_text(img_buffer, 10, ycursor, "SELECT IMAGE", BLACK);
	ycursor += (2*LINE_SPACING);

	// contents
	for (int i = 0; i < 8; i++)
	{
		if(i == file_select_index) {
			fill_rect(img_buffer, 0, ycursor-1, WIDTH>>2, ycursor+TEXT_HEIGHT+1, OLIVE);
			write_text(img_buffer, 10, ycursor, files[i], WHITE);
		} else {
			write_text(img_buffer, 10, ycursor, files[i], WHITE);
		}
		ycursor += LINE_SPACING;
	}
}

void draw_pane_kernel_selector(int (&img_buffer)[HEIGHT][WIDTH], bool highlight)
{
	colour_e colour = (highlight) ? GREEN : WHITE;
	uint32_t ycursor = HEIGHT>>1;
	// boundary
	draw_rect(img_buffer, 0, ycursor, WIDTH>>2, 3*HEIGHT>>2, colour);

	// header
	fill_rect(img_buffer, 2, ycursor+1, (WIDTH>>2)-1, ycursor+2*LINE_SPACING, colour);
	ycursor += LINE_SPACING;
	write_text(img_buffer, 10, ycursor, "SELECT KERNEL", BLACK);
	ycursor += LINE_SPACING;

	// contents
	ycursor += LINE_SPACING;
	if(kernlist == SHARPEN)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "SHARPEN", WHITE);
	ycursor += LINE_SPACING;
	if(kernlist == HORIZONTAL_EMBOSS)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "HORIZONTAL EMBOSS", WHITE);
	ycursor += LINE_SPACING;
	if(kernlist == VERTICAL_EMBOSS)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "VERTICAL EMBOSS", WHITE);
	ycursor += LINE_SPACING;
	if(kernlist == ALL_EMBOSS)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "ALL EMBOSS", WHITE);
	ycursor += LINE_SPACING;
	if(kernlist == CUSTOM)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "CUSTOM", WHITE);
}

void draw_pane_main_menu(int (&img_buffer)[HEIGHT][WIDTH], bool highlight)
{
	colour_e colour = (highlight) ? GREEN : WHITE;
	uint32_t ycursor = 3*HEIGHT>>2;
	// boundary
	draw_rect(img_buffer, 0, ycursor, WIDTH>>2, HEIGHT-1, colour);

	// header
	fill_rect(img_buffer, 2, ycursor+1, (WIDTH>>2)-1, ycursor+2*LINE_SPACING, colour);
	ycursor += LINE_SPACING;
	write_text(img_buffer, 10, ycursor, "MAIN MENU", BLACK);
	ycursor += LINE_SPACING;

	// contents
	ycursor += LINE_SPACING;
	if(mainlist == LOAD_IMAGE)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "LOAD IMAGE", WHITE);
	ycursor += LINE_SPACING;
	if(mainlist == APPLY_KERNEL)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "APPLY KERNEL", WHITE);
	ycursor += LINE_SPACING;
	if(mainlist == SET_CUSTOM_KERNEL)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "SET CUSTOM KERNEL", WHITE);
	ycursor += LINE_SPACING;
	if(mainlist == SAVE_IMAGE)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "SAVE IMAGE", WHITE);
	ycursor += LINE_SPACING;
	if(mainlist == REFRESH_FILE_LIST)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "REFRESH FILE LIST", WHITE);

	if(do_action && mainlist == SAVE_IMAGE)
	{
		char name[32] = {0};
		char kernName[8] = {0};
		if(kernlist == SHARPEN) {
			strcpy(kernName, "SH");
		}
		else if (kernlist == HORIZONTAL_EMBOSS) {
			strcpy(kernName, "HE");
		}
		else if (kernlist == VERTICAL_EMBOSS) {
			strcpy(kernName, "VE");
		}
		else if (kernlist == ALL_EMBOSS) {
			strcpy(kernName, "AE");
		}
		else if (kernlist == CUSTOM) {
			strcpy(kernName, "_C");
		}
		char x;
		int length = 0;
		for (int i = 0; i < 32; i++)
		{
			x = files[file_select_index][i];
			if(x == '.')
			{
				length = i;
				break;
			}
			else
			{
				name[i] = x;
			}
		}

		strcat(name, kernName);
		strcat(name, ".BMP");
		write_image_file(name, IMG_SIZE, u32(IPC_IMG1_LOC));
//		write_image_file(files[file_select_index], IMG_SIZE, u32(IPC_IMG1_LOC));
	}

	if(do_action && mainlist == REFRESH_FILE_LIST)
	{
		file_select_index = 0;
		filesNum = 0;
		read_bmp_files();
	}


}

void draw_all(int (&img_buffer)[HEIGHT][WIDTH])
{
	// blank panes
	blank_pane_file_selector(img_buffer);
	blank_pane_kernel_selector(img_buffer);
	blank_pane_main_menu(img_buffer);

	// draw panes
	draw_pane_file_selector(img_buffer, (active_pane == FILE_SELECT));
	draw_pane_kernel_selector(img_buffer, (active_pane == KERNEL_SELECT));
	draw_pane_main_menu(img_buffer, (active_pane == MAIN_MENU));
	draw_pane_custom_kernel_passive(img_buffer);
	draw_pane_src_img(img_buffer);
	draw_pane_dst_img(img_buffer);
	do_action = false;
}

void draw_main_page(int (&img_buffer)[HEIGHT][WIDTH])
{
	if(quickupdate)
	{
		if(active_pane == FILE_SELECT)
		{
			blank_pane_file_selector(img_buffer);
			draw_pane_file_selector(img_buffer, (active_pane == FILE_SELECT));
		}
		else if(active_pane == MAIN_MENU)
		{
			blank_pane_main_menu(img_buffer);
			draw_pane_main_menu(img_buffer, (active_pane == MAIN_MENU));
		}
		else if(active_pane == KERNEL_SELECT)
		{
			blank_pane_kernel_selector(img_buffer);
			draw_pane_kernel_selector(img_buffer, (active_pane == KERNEL_SELECT));
		}
		quickupdate = false;
	}
	else
	{
		draw_all(img_buffer);
	}
}


//void draw_image(int index)
//{
//	read_image_file(files[index]);
//
//}

void gui_handler(int (&img_buffer)[HEIGHT][WIDTH])
{
	// check if button has been released
	if(last_btn_value == btn_value)
	{
		// ready to accept more inputs
		pause_buttons = false;
		return;
	}
	last_btn_value = btn_value;

	// Change active pane based on left/right
	if(btn_value == BT_LT || btn_value == BT_RT) {
		int active_pane_ = active_pane;
		if(btn_value == BT_LT) {
			active_pane_ = (active_pane_-1 < 0) ? (WRAP_ACTIVE_E-1):(active_pane_-1);
		} else {
			active_pane_ = (active_pane_+1 == WRAP_ACTIVE_E) ? (0):(active_pane_+1);
		}
		active_pane = static_cast<active_e>(active_pane_);
	}

	// Change highlight based on button value
	if(btn_value == BT_UP || btn_value == BT_DN)
	{
		quickupdate = true;
		if(active_pane == MAIN_MENU)
		{
			highlight_index = mainlist;
			if(btn_value == BT_UP) {
				highlight_index = MIN(highlight_index+1, WRAP_MAINLIST_E-1);
			} else {
				highlight_index = MAX(highlight_index-1, 0);
			}
			mainlist = static_cast<mainlist_e>(highlight_index);
		}
		else if(active_pane == FILE_SELECT)
		{
			highlight_index = file_select_index;
			if(btn_value == BT_UP) {
				highlight_index = MIN(highlight_index+1, filesNum-1);
			} else {
				highlight_index = MAX(highlight_index-1, 0);
			}
			file_select_index = highlight_index;
		}
		else if(active_pane == KERNEL_SELECT)
		{
			highlight_index = kernlist;
			if(btn_value == BT_UP) {
				highlight_index = MIN(highlight_index+1, WRAP_KERNLIST_E-1);
			} else {
				highlight_index = MAX(highlight_index-1, 0);
			}
			kernlist = static_cast<kernlist_e>(highlight_index);
			if(Xil_In32(IPC_KERNEL_SELECT) == kernlist)
			{
				// kernel already selected, no point rewriting
			}
			else
			{
				Xil_Out32(IPC_KERNEL_SELECT, kernlist);
				Xil_Out32(IPC_MSG_LOC, IPC_ARM0_TRIGGER);
			}
		}
	}

	// Select if center button pressed
	if(btn_value == BT_CR)
	{
		do_action = true;
	}

	// draw main page again
	draw_main_page(img_buffer);
}

#endif // __GUI_H__
