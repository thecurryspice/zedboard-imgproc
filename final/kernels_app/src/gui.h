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
uint8_t kernel_pixel_select = 0;
uint8_t kernel_pixel_value;
bool quickupdate = false, do_action = false, custom_kernel_pane_active = false;

#include "panes.h"

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
}

void draw_main_page(int (&img_buffer)[HEIGHT][WIDTH])
{
	if(quickupdate)	// set from gui_handler
	{
		if(active_pane == FILE_SELECT)			// redraw file-selector
		{
			blank_pane_file_selector(img_buffer);
			draw_pane_file_selector(img_buffer, (active_pane == FILE_SELECT));
		}
		else if(active_pane == MAIN_MENU)
		{
			// go to custom-kernel pane if requested by user (do_action) or if already in pane
			if(custom_kernel_pane_active)
			{
				blank_pane_custom_kernel(img_buffer);
				draw_pane_custom_kernel_active(img_buffer);
			} else {							// redraw main_menu
				// if pane is already active, go back
				blank_pane_custom_kernel(img_buffer);
				draw_pane_custom_kernel_passive(img_buffer);
				blank_pane_main_menu(img_buffer);
				draw_pane_main_menu(img_buffer, (active_pane == MAIN_MENU));
			}
		}
		else if(active_pane == KERNEL_SELECT)	// redraw kernel-selector
		{
			blank_pane_kernel_selector(img_buffer);
			draw_pane_kernel_selector(img_buffer, (active_pane == KERNEL_SELECT));
		}
		// unset quick-update after doing a quick update
		quickupdate = false;
	}
	else
	{
		draw_all(img_buffer);
	}
	// reset action
	do_action = false;
}

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

	// Select if center button pressed
	if(btn_value == BT_CR)
	{
		do_action = true;
		if(mainlist == SET_CUSTOM_KERNEL)
		{
			quickupdate = true;
			custom_kernel_pane_active = !custom_kernel_pane_active;
		}
	}

	if(custom_kernel_pane_active)
	{
		quickupdate = true;
		if(btn_value == BT_LT || btn_value == BT_RT)
		{
			if(btn_value == BT_LT) {
				kernel_pixel_select = (kernel_pixel_select-1 < 0) ? (8):(kernel_pixel_select -1);
			} else {
				kernel_pixel_select = (kernel_pixel_select+1 == 9) ? (0):(kernel_pixel_select +1);
			}
		}
//		kernel_pixel_value = Xil_In64(IPC_KERNEL_VALUES) & (0x1F << (kernel_pixel_select*5));
		kernel_pixel_value = Xil_In8(IPC_KERNEL_VALUES+kernel_pixel_select);

		// Change highlight based on button value
		if(btn_value == BT_UP || btn_value == BT_DN)
		{
			quickupdate = true;
			if(btn_value == BT_DN) {
				kernel_pixel_value = MIN(kernel_pixel_value+1, 9);
			} else {
				kernel_pixel_value = MAX(kernel_pixel_value-1, 0);
			}
		}

//		uint64_t buf = Xil_In64(IPC_KERNEL_VALUES);
//		buf = buf & ~(0x1F << (kernel_pixel_select*5));
//		buf = buf | (kernel_pixel_value << (kernel_pixel_select*5));
//		Xil_Out64(IPC_KERNEL_VALUES, buf);
		Xil_Out8(IPC_KERNEL_VALUES+kernel_pixel_select, kernel_pixel_value);
		// return if the custom kernel pane is active
		goto DRAW_PAGE;
	}


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
				// write the new kernel selection
				Xil_Out32(IPC_KERNEL_SELECT, kernlist);
				// and tell the other core about it
				Xil_Out32(IPC_MSG_LOC, IPC_ARM0_TRIGGER);
			}
		}
	}

	DRAW_PAGE:
	// draw main page again
	draw_main_page(img_buffer);
}

#endif // __GUI_H__
