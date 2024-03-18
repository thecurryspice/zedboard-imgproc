#ifndef __GUI_H__
#define __GUI_H__

#include "defs.h"
#include "textglyphs.h"
#include "display.h"

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
	RESET_IMAGE,
	SAVE_IMAGE,
	WRAP_MAINLIST_E
} mainlist_e;
mainlist_e mainlist;

typedef enum {
	SHARPEN,
	HORIZONTAL_EMBOSS,
	VERTICAL_EMBOSS,
	WRAP_KERNLIST_E
} kernlist_e;
kernlist_e kernlist;

uint16_t highlight_index = 0;
bool quickupdate = false;

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
	if(mainlist == RESET_IMAGE)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "RESET IMAGE", WHITE);
	ycursor += LINE_SPACING;
	if(mainlist == SAVE_IMAGE)
		fill_rect(img_buffer, 0, ycursor-1, (WIDTH>>2)-1, ycursor+TEXT_HEIGHT+1, OLIVE);
	write_text(img_buffer, 10, ycursor, "SAVE IMAGE", WHITE);
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
		}
	}

	// Select if centre button pressed
	// TODO

	// draw main page again
	draw_main_page(img_buffer);
}

#endif // __GUI_H__
