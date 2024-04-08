#ifndef __PANES_H__
#define __PANES_H__

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

void blank_pane_custom_kernel(int (&img_buffer)[HEIGHT][WIDTH])
{
	fill_rect(img_buffer, (WIDTH>>2), 3*HEIGHT>>2, (WIDTH>>2)+200, HEIGHT, BLACK);
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
	write_text(img_buffer, (xcursor+10), ycursor, "CUSTOM KERNEL", BLACK);

	// kernel grid
	draw_hline(img_buffer, xcursor, ycursor + (1*kern_window_height/3), kern_window_width, WHITE);
	draw_hline(img_buffer, xcursor, ycursor + (2*kern_window_height/3), kern_window_width, WHITE);
	draw_vline(img_buffer, xcursor + (1*kern_window_width/3), ycursor, kern_window_height, WHITE);
	draw_vline(img_buffer, xcursor + (2*kern_window_width/3), ycursor, kern_window_height, WHITE);

	// pixel selection rectangle
//	uint8_t krow = kernel_pixel_select/3;
//	uint8_t kcol = kernel_pixel_select%3;
	for (uint8_t krow = 0; krow < 3; krow++)
	{
		for (uint8_t kcol = 0; kcol < 3; kcol++)
		{
			uint32_t kpxcursor = xcursor + kcol*(kern_window_width/3);
			uint32_t kpycursor = ycursor + krow*(kern_window_height/3);
			uint8_t pixel_index = krow*3 + kcol;
//			if(pixel_index == kernel_pixel_select)
//			{
//				fill_rect(img_buffer, kpxcursor, kpycursor, (kpxcursor)+(kern_window_width/3), (kpycursor)+(kern_window_height/3), MAGENTA);
//			}

//			uint8_t pixel_value = (Xil_In64(IPC_KERNEL_VALUES) & (0x1F << (pixel_index*5)));
			uint8_t pixel_value = Xil_In8(IPC_KERNEL_VALUES+pixel_index);
			char x[2] = {0,0};
			x[0] = (char)(pixel_value + 48);
			write_text(img_buffer, kpxcursor+padding, kpycursor+LINE_SPACING, x, WHITE);
		}
	}

//	if(do_action && mainlist == CUSTOM_KERNEL)
//	{
//		Xil_Out32(IPC_KERNEL_SELECT, kernlist);
//		Xil_Out32(IPC_MSG_LOC, IPC_ARM0_TRIGGER);
//		}
//	}
}

void draw_pane_custom_kernel_active(int (&img_buffer)[HEIGHT][WIDTH])
{
	uint32_t ycursor =  (3*HEIGHT>>2);
	uint32_t xcursor = ((WIDTH>>2) + 10);
	uint32_t padding = LINE_SPACING + ((TEXT_HEIGHT-TEXT_WIDTH)>>1);
	uint32_t kern_window_width  = (TEXT_WIDTH + 2*(padding)) * 3;
	uint32_t kern_window_height = (TEXT_HEIGHT + 2*LINE_SPACING) * 3;

	// caption
	fill_rect(img_buffer, xcursor-1, ycursor, (xcursor-1)+(kern_window_width+1), ycursor+2*LINE_SPACING, GREEN);
	ycursor += LINE_SPACING;
	write_text(img_buffer, (xcursor+10), ycursor, "CUSTOM KERNEL", BLACK);

	ycursor += LINE_SPACING+1;
	// boundary
	draw_rect(img_buffer, xcursor-1, ycursor-1, (xcursor-1)+(kern_window_width), (ycursor-1)+(kern_window_height), MAGENTA);

	// kernel grid
	draw_hline(img_buffer, xcursor, ycursor + (1*kern_window_height/3), kern_window_width, WHITE);
	draw_hline(img_buffer, xcursor, ycursor + (2*kern_window_height/3), kern_window_width, WHITE);
	draw_vline(img_buffer, xcursor + (1*kern_window_width/3), ycursor, kern_window_height, WHITE);
	draw_vline(img_buffer, xcursor + (2*kern_window_width/3), ycursor, kern_window_height, WHITE);

	// pixel selection rectangle
//	uint8_t krow = kernel_pixel_select/3;
//	uint8_t kcol = kernel_pixel_select%3;
	for (uint8_t krow = 0; krow < 3; krow++)
	{
		for (uint8_t kcol = 0; kcol < 3; kcol++)
		{
			uint32_t kpxcursor = xcursor + kcol*(kern_window_width/3);
			uint32_t kpycursor = ycursor + krow*(kern_window_height/3);
			uint8_t pixel_index = krow*3 + kcol;
			if(pixel_index == kernel_pixel_select)
			{
				fill_rect(img_buffer, kpxcursor, kpycursor, (kpxcursor)+(kern_window_width/3), (kpycursor)+(kern_window_height/3), MAGENTA);
			}

//			uint8_t pixel_value = (Xil_In64(IPC_KERNEL_VALUES) & (0x1F << (pixel_index*5)));
			uint8_t pixel_value = Xil_In8(IPC_KERNEL_VALUES+pixel_index);
			char x[2] = {0,0};
			x[0] = (char)(pixel_value + 48);
			write_text(img_buffer, kpxcursor+padding, kpycursor+LINE_SPACING, x, RED);
		}
	}
//	uint32_t kpxcursor = xcursor + kcol*(kern_window_width/3);
//	uint32_t kpycursor = ycursor + krow*(kern_window_height/3);
//	fill_rect(img_buffer, kpxcursor, kpycursor, (kpxcursor)+(kern_window_width/3), (kpycursor)+(kern_window_height/3), MAGENTA);

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
		else	// send next trigger
		{
			// write the new kernel selection
			Xil_Out32(IPC_KERNEL_SELECT, kernlist);
			// and tell the other core about it
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
		char kernName[4] = {0};
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
		for (int i = 0; i < 32; i++) {
			x = files[file_select_index][i];
			if(x == '.') {
				length = i; break;
			} else {
				name[i] = x;
			}
		}
		memcpy((void*)(&(filebuffer_p->thisImage_p)), (void*)IPC_IMG1_LOC, sizeof(image_t));
		strcat(name, kernName);
		strcat(name, ".BMP");
		write_image_file(name, sizeof(filebuffer_t), u32(filebuffer_p));
//		write_image_file(name, IMG_SIZE, u32(IPC_IMG1_LOC));
//		write_image_file(files[file_select_index], IMG_SIZE, u32(IPC_IMG1_LOC));
	}

	if(do_action && mainlist == REFRESH_FILE_LIST)
	{
		file_select_index = 0;
		filesNum = 0;
		read_bmp_files();
	}
}

#endif // __PANES_H__
