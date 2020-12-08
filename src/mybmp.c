#include "mybmp.h"
#include <stdio.h>
#include <stdlib.h>

void init_bmp_t(bmp_t *bmp)
{
	bmp->data = NULL;
	bmp->palette = NULL;
}

void read_byte_from_buffer(BYTE *buffer, BYTE *write_in, unsigned int *read_from)
{
	*write_in = buffer[*read_from];
	*read_from += sizeof(BYTE);
}

void read_word_from_buffer(BYTE *buffer, WORD *write_in, unsigned int *read_from)
{
	*write_in = buffer[*read_from];
	*write_in |= buffer[*read_from + 1] << 8;
	*read_from += sizeof(WORD);
}

void read_dword_from_buffer(BYTE *buffer, DWORD *write_in, unsigned int *read_from)
{
	*write_in = buffer[*read_from];
	*write_in |= buffer[*read_from + 1] << 8;
	*write_in |= buffer[*read_from + 2] << 16;
	*write_in |= buffer[*read_from + 3] << 24;
	*read_from += sizeof(DWORD);
}

void read_long_from_buffer(BYTE *buffer, LONG *write_in, unsigned int *read_from)
{
	*write_in = buffer[*read_from];
	*write_in |= buffer[*read_from + 1] << 8;
	*write_in |= buffer[*read_from + 2] << 16;
	*write_in |= buffer[*read_from + 3] << 24;
	*read_from += sizeof(LONG);
}

void write_byte_to_buffer(BYTE *buffer, BYTE read, unsigned int *write_to)
{
	buffer[*write_to + 0] = read;
	*write_to += sizeof(BYTE);
}

void write_word_to_buffer(BYTE *buffer, WORD read, unsigned int *write_to)
{
	buffer[*write_to + 0] = read & 0x00FF;
	buffer[*write_to + 1] = (read & 0xFF00) >> 8;
	*write_to += sizeof(WORD);
}

void write_dword_to_buffer(BYTE *buffer, DWORD read, unsigned int *write_to)
{
	buffer[*write_to + 0] = read & 0x000000FF;
	buffer[*write_to + 1] = (read & 0x0000FF00) >> 8;
	buffer[*write_to + 2] = (read & 0x00FF0000) >> 16;
	buffer[*write_to + 3] = (read & 0xFF000000) >> 24;
	*write_to += sizeof(DWORD);
}

void write_long_to_buffer(BYTE *buffer, LONG read, unsigned int *write_to)
{
	buffer[*write_to + 0] = read & 0x000000FF;
	buffer[*write_to + 1] = (read & 0x0000FF00) >> 8;
	buffer[*write_to + 2] = (read & 0x00FF0000) >> 16;
	buffer[*write_to + 3] = (read & 0xFF000000) >> 24;
	*write_to += sizeof(LONG);
}

int read_header(FILE *input, bmp_t *bmp)
{
	BYTE input_buffer[HEADER_SIZE];

	if (fread(input_buffer, sizeof(BYTE), HEADER_SIZE, input) != HEADER_SIZE)
	{
		return INPUT_ERROR;
	}

	unsigned int read_from = 0;

	read_word_from_buffer(input_buffer, &bmp->signature, &read_from);
	read_dword_from_buffer(input_buffer, &bmp->file_size, &read_from);
	read_word_from_buffer(input_buffer, &bmp->reserved1, &read_from);
	read_word_from_buffer(input_buffer, &bmp->reserved2, &read_from);
	read_dword_from_buffer(input_buffer, &bmp->offset_bits, &read_from);

	read_dword_from_buffer(input_buffer, &bmp->dheader_size, &read_from);
	read_long_from_buffer(input_buffer, &bmp->width, &read_from);
	read_long_from_buffer(input_buffer, &bmp->height, &read_from);
	read_word_from_buffer(input_buffer, &bmp->planes, &read_from);
	read_word_from_buffer(input_buffer, &bmp->bits_per_pixel, &read_from);
	read_dword_from_buffer(input_buffer, &bmp->compression, &read_from);
	read_dword_from_buffer(input_buffer, &bmp->image_size, &read_from);
	read_long_from_buffer(input_buffer, &bmp->horizontal_resolution, &read_from);
	read_long_from_buffer(input_buffer, &bmp->vertical_resolution, &read_from);
	read_dword_from_buffer(input_buffer, &bmp->palette_colors, &read_from);
	read_dword_from_buffer(input_buffer, &bmp->used_colors, &read_from);

	return NO_ERROR;
}

int check_header(bmp_t *bmp)
{
	if (bmp->offset_bits < HEADER_SIZE)
		return OFFSET_ERROR;
	if (bmp->width <= 0)
		return INCORRECT_WIDTH;
	if (bmp->height == 0)
		return INCORRECT_HEIGHT;
	if (bmp->reserved1 != 0 || bmp->reserved2 != 0)
		return NOT_RESERVED;
	if (bmp->signature != 0x4D42)
		return INCORRECT_SIGNATURE;
	if (bmp->planes != 1)
		return PLANES_NOT_EQ_1;
	if (bmp->bits_per_pixel != 8 && bmp->bits_per_pixel != 24)
		return PLANES_NOT_EQ_1;

	return NO_ERROR;
}

void free_bmp_t(bmp_t *bmp)
{
	free(bmp->palette);
	free(bmp->data);
}

int read_bmp(FILE *input, bmp_t *bmp)
{
	int return_code;

    free_bmp_t(bmp);

	return_code = read_header(input, bmp);

	if (return_code != NO_ERROR)
		return return_code;

	return_code = check_header(bmp);

	if (return_code != NO_ERROR)
		return return_code;

	if (bmp->bits_per_pixel == 8)
	{
		bmp->palette = (BYTE *)malloc(PALETTE_SIZE_8_BIT * sizeof(BYTE));

		if (bmp->palette == NULL)
			return OUT_OF_MEMORY_ERROR;

		if (fread(bmp->palette, sizeof(BYTE), PALETTE_SIZE_8_BIT, input) != PALETTE_SIZE_8_BIT)
			return INPUT_ERROR;
	}
	else
		bmp->palette = NULL;

	bmp->data = (BYTE *)malloc(bmp->image_size * sizeof(BYTE));

	if (bmp->data == NULL)
		return OUT_OF_MEMORY_ERROR;

	if (fread(bmp->data, sizeof(BYTE), bmp->image_size, input) != bmp->image_size)
		return INPUT_ERROR;

	return NO_ERROR;
}

int write_header(FILE *output, bmp_t *bmp)
{
	BYTE output_buffer[HEADER_SIZE];
	unsigned int write_to = 0;

	write_word_to_buffer(output_buffer, bmp->signature, &write_to);
	write_dword_to_buffer(output_buffer, bmp->file_size, &write_to);
	write_word_to_buffer(output_buffer, bmp->reserved1, &write_to);
	write_word_to_buffer(output_buffer, bmp->reserved2, &write_to);
	write_dword_to_buffer(output_buffer, bmp->offset_bits, &write_to);

	write_dword_to_buffer(output_buffer, bmp->dheader_size, &write_to);
	write_long_to_buffer(output_buffer, bmp->width, &write_to);
	write_long_to_buffer(output_buffer, bmp->height, &write_to);
	write_word_to_buffer(output_buffer, bmp->planes, &write_to);
	write_word_to_buffer(output_buffer, bmp->bits_per_pixel, &write_to);
	write_dword_to_buffer(output_buffer, bmp->compression, &write_to);
	write_dword_to_buffer(output_buffer, bmp->image_size, &write_to);
	write_long_to_buffer(output_buffer, bmp->horizontal_resolution, &write_to);
	write_long_to_buffer(output_buffer, bmp->vertical_resolution, &write_to);
	write_dword_to_buffer(output_buffer, bmp->palette_colors, &write_to);
	write_dword_to_buffer(output_buffer, bmp->used_colors, &write_to);

	if (fwrite(output_buffer, sizeof(BYTE), HEADER_SIZE, output) != HEADER_SIZE)
		return OUTPUT_ERROR;

	return NO_ERROR;
}

int write_bmp(FILE *output, bmp_t *bmp)
{
	int return_code;

	return_code = write_header(output, bmp);

	if (return_code != NO_ERROR)
		return return_code;

	if (bmp->palette != NULL)
	{
		if (fwrite(bmp->palette, sizeof(BYTE), PALETTE_SIZE_8_BIT, output) != PALETTE_SIZE_8_BIT)
			return OUTPUT_ERROR;
	}

	if (fwrite(bmp->data, sizeof(BYTE), bmp->image_size, output) != bmp->image_size)
		return OUTPUT_ERROR;

	return NO_ERROR;
}

void get_pixel(bmp_t *bmp, unsigned int x, unsigned int y, BYTE *r, BYTE *g, BYTE *b)
{
	BYTE *pixel;
	unsigned int bytes_per_pixel = bmp->bits_per_pixel >> 3;
	unsigned int bytes_per_row = bmp->image_size / bmp->height;

	pixel = bmp->data + (bmp->height - y - 1) * bytes_per_row + x * bytes_per_pixel;

	if (bmp->palette != NULL)
		pixel = bmp->palette + (*pixel << 2);

	*r = *(pixel + 2);
	*g = *(pixel + 1);
	*b = *pixel;
}

void set_pixel(bmp_t *bmp, unsigned int x, unsigned int y, BYTE r, BYTE g, BYTE b)
{
	BYTE *pixel;
	unsigned int bytes_per_pixel = bmp->bits_per_pixel >> 3;
	unsigned int bytes_per_row = bmp->image_size / bmp->height;

	pixel = bmp->data + (bmp->height - y - 1) * bytes_per_row + x * bytes_per_pixel;

	*(pixel + 2) = r;
	*(pixel + 1) = g;
	*pixel = b;
}

void get_palette_color(bmp_t *bmp, BYTE index, BYTE *r, BYTE *g, BYTE *b)
{
    *g = *(bmp->palette + (index << 2));
    *b = *(bmp->palette + (index << 2) + 1);
    *r = *(bmp->palette + (index << 2) + 2);
}

void set_palette_color(bmp_t *bmp, BYTE index, BYTE r, BYTE g, BYTE b)
{
    *(bmp->palette + (index << 2)) = g;
    *(bmp->palette + (index << 2) + 1) = b;
    *(bmp->palette + (index << 2) + 2) = r;
}