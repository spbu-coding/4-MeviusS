#ifndef MY_REALISATION_H
#define MY_REALISATION_H

#include <stdlib.h>
#include <stdio.h>

#define BYTE unsigned char
#define WORD unsigned short
#define DWORD unsigned int
#define LONG int

#define FILE_TYPE_ERROR 1
#define RESERVED_NULLS_ERROR 2
#define VERSION_ERROR 3
#define WIDTH_ERROR 4
#define HEIGHT_ERROR 5
#define PLANES_ERROR 6
#define BYTES_READING_ERROR 8
#define MEMORY_ALLOCATION_ERROR 9
#define FILE_OPEN_ERROR 10

typedef struct {
    BYTE r;
    BYTE g;
    BYTE b;
} PIXEL;

#pragma pack(push, 1)
typedef struct {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BMPHEADER;
#pragma pack(pop)

typedef struct {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
    BMPHEADER file_header;
    PIXEL **data;
} BMP_24;

typedef struct {
    BMPHEADER file_header;
    PIXEL *palette;
    BYTE **data;
} BMP_8;

int read_bmpheader(BMPHEADER *bmpheader, FILE *fin);
int read_bitmapinfoheader(BITMAPINFOHEADER *bitmapinfoheader, FILE *fin);
int count_padding(LONG width);
int load_bmp_24(BMP_24 *image, char const *filename);
int write_negative_bmp_24(BMP_24 *image, char const *filename);
int load_bmp_8(BMP_8 *image, char const *filename);
int write_negative_bmp_8(BMP_8 *image, char const *filename);
int return_bits_per_pixel(char const *filename);
int compare_pixels(PIXEL a, PIXEL b);
int compare_bmp8(BMP_8 *image1, BMP_8 *image2);
int compare_bmp24(BMP_24 *image1, BMP_24 *image2);
void free_bmp8(BMP_8 *image);
void free_bmp24(BMP_24 *image);

#endif //MY_REALISATION_H
