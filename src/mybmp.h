#ifndef _MYBMP_INCLUDED
#define _MYBMP_INCLUDED

#include <stdio.h>

#define HEADER_SIZE 54
#define PALETTE_SIZE_8_BIT 1024

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define LONG int

typedef enum
{
	NO_ERROR = 0,
	INPUT_ERROR,
	OUTPUT_ERROR,
	OUT_OF_MEMORY_ERROR,
	OFFSET_ERROR,
	INCORRECT_HEIGHT,
	INCORRECT_WIDTH,
	NOT_RESERVED,
	INCORRECT_SIGNATURE,
	PLANES_NOT_EQ_1,
	INCORRECT_BITS_PER_PIXEL
} error_t;

typedef struct
{
	WORD signature;
	DWORD file_size;
	WORD reserved1;
	WORD reserved2;
	DWORD offset_bits;

	DWORD dheader_size;
	LONG width;
	LONG height;
	WORD planes;
	WORD bits_per_pixel;
	DWORD compression;
	DWORD image_size;
	LONG horizontal_resolution;
	LONG vertical_resolution;
	DWORD palette_colors;
	DWORD used_colors;

	BYTE *palette;
	BYTE *data;
} bmp_t;

void init_bmp_t(bmp_t *bmp);

void read_byte_from_buffer(BYTE *buffer, BYTE *write_in, unsigned int *read_from);
void read_word_from_buffer(BYTE *buffer, WORD *write_in, unsigned int *read_from);
void read_dword_from_buffer(BYTE *buffer, DWORD *write_in, unsigned int *read_from);
void read_long_from_buffer(BYTE *buffer, LONG *write_in, unsigned int *read_from);

void write_byte_to_buffer(BYTE *buffer, BYTE read, unsigned int *write_to);
void write_word_to_buffer(BYTE *buffer, WORD read, unsigned int *write_to);
void write_dword_to_buffer(BYTE *buffer, DWORD read, unsigned int *write_to);
void write_long_to_buffer(BYTE *buffer, LONG read, unsigned int *write_to);

int read_header(FILE *input, bmp_t *bmp);
int check_header(bmp_t *bmp);
void free_bmp_t(bmp_t *bmp);
int read_bmp(FILE *input, bmp_t *bmp);
int write_header(FILE *output, bmp_t *bmp);
int write_bmp(FILE *output, bmp_t *bmp);

#include "mybmp.c"
#endif