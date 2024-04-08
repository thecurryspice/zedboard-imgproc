#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "defs.h"
#include "textglyphs.h"

void clear_frame(int (&img_buffer)[HEIGHT][WIDTH])
{
	for(int row = 0; row < HEIGHT; row++)
	{
		for(int col = 0; col < WIDTH; col++)
			img_buffer[row][col] = palette[BLACK];
	}
}

void build_frame(int (&img_buffer)[HEIGHT][WIDTH], int _colour_offset, int _bar_width)
{
	int bar_idx = 0;
	int current_line = 0;
	if(horizontal)
	{
		int prevRow = 0, bar_idx = 0;;
		// for each row, walk through the available palette based on the bar_idx
		int colour = (bar_idx+_colour_offset)%PALETTE_SIZE;
		for(int row = 0; row < HEIGHT; row++)
		{
			current_line = row-prevRow;
			if(current_line >= _bar_width)
			{
				// update bar_idx and colour when #columns for a bar are drawn
				bar_idx++;
				prevRow = row;
				colour = (bar_idx+_colour_offset)%PALETTE_SIZE;
			}
			for(int col = 0; col < WIDTH; col++)
			{
				// assign colour to this pixel
				img_buffer[row][col] = palette[colour];
			}
		}
	}
	else
	{
		int prevCol = 0;
		for(int row = 0; row < HEIGHT; row++)
		{
			// for each row, walk through the available palette based on the bar_idx
			bar_idx = 0;
			prevCol = 0;
			int colour = (bar_idx+_colour_offset)%PALETTE_SIZE;
			for(int col = 0; col < WIDTH; col++)
			{
				// assign colour to this pixel
				img_buffer[row][col] = palette[colour];
				current_line = col-prevCol;
				if(current_line >= _bar_width)
				{
					// update bar_idx and colour when #columns for a bar are drawn
					bar_idx++;
					prevCol = col;
					colour = (bar_idx+_colour_offset)%PALETTE_SIZE;
				}
			}
		}
	}
}

void draw_line(int (&img_buffer)[HEIGHT][WIDTH], uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, colour_e colour)
{
	int16_t steep = (y1 - y0) > (x1 - x0);

	int16_t dx, dy;
	dx = x1 - x0;
	dy = (y1 - y0);

	int16_t err = dx / 2;
	int16_t ystep;

	if (y0 < y1) {
		ystep = 1;
	} else {
		ystep = -1;
	}

	for (; x0 <= x1; x0++)
	{
		if (steep) {
			img_buffer[y0][x0] = palette[colour];
		} else {
			img_buffer[y0][x0] = palette[colour];
		}
		err -= dy;
		if (err < 0) {
			y0 += ystep;
			err += dx;
		}
	}
}

bool draw_hline(int (&img_buffer)[HEIGHT][WIDTH], uint32_t x0, uint32_t y0, uint32_t length, colour_e colour)
{
	if(x0 + length >= WIDTH)
		return false;
	for(uint32_t i = 0; i < length; i++)
	{
		img_buffer[y0][x0+i] = palette[colour];
	}
	return true;
}

bool draw_vline(int (&img_buffer)[HEIGHT][WIDTH], uint32_t x0, uint32_t y0, uint32_t length, colour_e colour)
{
	if(y0 + length >= HEIGHT)
		return false;
	for(uint32_t i = 0; i < length; i++)
	{
		img_buffer[y0+i][x0] = palette[colour];
	}
	return true;
}

bool draw_rect(int (&img_buffer)[HEIGHT][WIDTH], uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, colour_e colour)
{
	if(y1 - y0 >= HEIGHT || y1 < y0)
		return false;
	if(x1 - x0 >= WIDTH || x1 < x0)
		return false;
	draw_hline(img_buffer, x0, y0, (x1-x0), colour);
	draw_hline(img_buffer, x0, y1, (x1-x0), colour);
	draw_vline(img_buffer, x0, y0, (y1-y0), colour);
	draw_vline(img_buffer, x1, y0, (y1-y0), colour);
	return true;
}

bool fill_rect(int (&img_buffer)[HEIGHT][WIDTH], uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1, colour_e colour)
{
	if(y1 - y0 >= HEIGHT || y1 < y0)
		return false;
	if(x1 - x0 >= WIDTH || x1 < x0)
		return false;
	for(uint32_t i = 0; i < (y1-y0); i++)
	{
		draw_hline(img_buffer, x0, y0+i, (x1-x0), colour);
	}
	return true;
}

void write_text_rainbow(int (&img_buffer)[HEIGHT][WIDTH], int offsetX, int offsetY, char* str)
{
	int charIndex = 0;
	uint32_t startX = offsetX;
	uint32_t startY = offsetY;
	uint8_t ascii_val, glyphIndex;
	bitmap_12x8_t character;
	uint32_t colour = 0;
	while(str[charIndex] != '\0')
	{
		ascii_val = str[charIndex];
		glyphIndex = asciiToGlyphIndex(ascii_val);
		loadTextGlyph(&character, glyphIndex);
		for(uint32_t row = 0; row < TEXT_HEIGHT; row++)
		{
			uint8_t drawMask = character.byte[row];
			for(uint32_t col = 0; col < TEXT_WIDTH; col++)
			{
				if(drawMask & (1<<7))
				{
					img_buffer[startY+row][startX+col] = palette[(colour++)%PALETTE_SIZE];
				}
				drawMask = drawMask<<1;
			}
		}

		// move to next character
		startX += (TEXT_WIDTH+TEXT_SPACING);
		charIndex++;
	}
}

void write_text(int (&img_buffer)[HEIGHT][WIDTH], int offsetX, int offsetY, char* str, colour_e colour)
{
	int charIndex = 0;
	uint32_t startX = offsetX;
	uint32_t startY = offsetY;
	uint8_t ascii_val, glyphIndex;
	bitmap_12x8_t character;
	while(str[charIndex] != '\0')
	{
		ascii_val = str[charIndex];
		glyphIndex = asciiToGlyphIndex(ascii_val);
		if(glyphIndex == GLYPHINDEX_WHITESPACE)
		{
			// nothing to draw, just move to next character
			goto END_LOOP_FOR_NEXT_CHAR;
		}
		loadTextGlyph(&character, glyphIndex);
//		xil_printf("glyphIndex: %d\r\n", glyphIndex);
//		for(uint8_t byte=0; byte<TEXT_HEIGHT; byte++)
//		{
//			xil_printf("0x%x,",character.byte[byte]);
//		}
//		xil_printf("\r\n");
		for(uint32_t row = 0; row < TEXT_HEIGHT; row++)
		{
			uint8_t drawMask = character.byte[row];
			for(uint32_t col = 0; col < TEXT_WIDTH; col++)
			{
				if(drawMask & (1<<7))
				{
					img_buffer[startY+row][startX+col] = palette[colour];
				}
				drawMask = drawMask<<1;
			}
		}

		// move to next character
		END_LOOP_FOR_NEXT_CHAR:
		startX += (TEXT_WIDTH+TEXT_SPACING);
		charIndex++;
	}
}

void write_text_bg(int (&img_buffer)[HEIGHT][WIDTH], int offsetX, int offsetY, char* str, colour_e colour_fg, colour_e colour_bg)
{
	int charIndex = 0;
	uint32_t startX = offsetX;
	uint32_t startY = offsetY;
	uint8_t ascii_val, glyphIndex;
	bitmap_12x8_t character;
	while(str[charIndex] != '\0')
	{
		ascii_val = str[charIndex];
		glyphIndex = asciiToGlyphIndex(ascii_val);
		if(glyphIndex == GLYPHINDEX_WHITESPACE)
		{
			// fill bg_colour and move to next character
			for(uint32_t row = 0; row < TEXT_HEIGHT; row++)
			{
				for(uint32_t col = 0; col < TEXT_WIDTH; col++)
				{
					img_buffer[startY+row][startX+col] = palette[colour_bg];
				}
			}
			goto END_LOOP_FOR_NEXT_CHAR;
		}
		loadTextGlyph(&character, glyphIndex);
		for(uint32_t row = 0; row < TEXT_HEIGHT; row++)
		{
			uint8_t drawMask = character.byte[row];
			for(uint32_t col = 0; col < TEXT_WIDTH; col++)
			{
				if(drawMask & (1<<7))
				{
					img_buffer[startY+row][startX+col] = palette[colour_fg];
				}
				else
				{
					img_buffer[startY+row][startX+col] = palette[colour_bg];
				}
				drawMask = drawMask<<1;
			}
		}

		// move to next character
		END_LOOP_FOR_NEXT_CHAR:
		startX += TEXT_WIDTH;
		// fill bg_colour and move to next character
		for(uint32_t row = 0; row < TEXT_HEIGHT; row++)
		{
			for(uint32_t col = 0; col < TEXT_SPACING; col++)
			{
				img_buffer[startY+row][startX+col] = palette[colour_bg];
			}
		}
		startX += TEXT_SPACING;
		charIndex++;
	}
}

void draw_image(int (&img_buffer)[HEIGHT][WIDTH], image_t* image_p, int offsetX, int offsetY)
{
	for(uint32_t row = 0; row < IMG_HEIGHT; row++)
	{
		for(uint32_t col = 0; col < IMG_WIDTH; col++)
		{
			// image is read inverted. So invert the row index
			img_buffer[offsetY+row][offsetX+col] = SWAP32(image_p->pixel[IMG_HEIGHT-1-row][col]);
		}
	}
}

void update_frame()
{
	memcpy(vga_buffer_p, img1D_p, NUM_BYTES_BUFFER);
	Xil_DCacheFlush();
}

#endif // __DISPLAY_H__
