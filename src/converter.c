#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qdbmp.h"
#include "my_realisation.h"

#define FIRST_PARAMETER_ERROR 1
#define SECOND_PARAMETER_ERROR 2
#define THIRD_PARAMETER_ERROR 3
#define PARAMETER_COUNT_ERROR 4
#define NORMAL_PARAMETER_COUNT 4

#define error(...) (fprintf(stderr, __VA_ARGS__))

typedef struct {
    char code_owner[10];
    char src_file[100];
    char dst_file[100];
} PARAMETER_STRUCT;

int read_arguments(int argc, char *argv[], PARAMETER_STRUCT *parameters) {
    if (argc != NORMAL_PARAMETER_COUNT) {
        error("Wrong count of parameters");
        return PARAMETER_COUNT_ERROR;
    }
    if (strcmp(argv[1], "--mine") != 0 && strcmp(argv[1], "--theirs") != 0) {
        error("Wrong first parameter");
        return FIRST_PARAMETER_ERROR;
    }
    strncpy(parameters->code_owner, argv[1], strlen(argv[1]));
    if (strstr(argv[2], ".bmp") == NULL && strstr(argv[2], ".BMP") == NULL) {
        error("Wrong second parameter");
        return SECOND_PARAMETER_ERROR;
    }
    strncpy(parameters->src_file, argv[2], strlen(argv[2]));
    if (strstr(argv[3], ".bmp") == NULL) {
        error("Wrong third parameter");
        return THIRD_PARAMETER_ERROR;
    }
    strncpy(parameters->dst_file, argv[3], strlen(argv[3]));
    return 0;
}

int main(int argc, char *argv[]) {
    PARAMETER_STRUCT parameters;
    int reading_result = read_arguments(argc, argv, &parameters);
    if (reading_result != 0) {
        return reading_result;
    }
    char src_path[105];
    sprintf(src_path, "%s%s", "..\\", parameters.src_file);
    char dst_path[105];
    sprintf(dst_path, "%s%s", "..\\", parameters.dst_file);
    int bits_per_pixel = return_bits_per_pixel(parameters.src_file);
    if (bits_per_pixel == -1) {
        error("Cannot open file %s", parameters.src_file);
        return FILE_OPEN_ERROR;
    }
    if (strcmp(parameters.code_owner, "--mine") == 0) {
        if (bits_per_pixel == 24) {
            BMP_24 *image = malloc(sizeof(*image));
            int loading_result = load_bmp_24(image, parameters.src_file);
            if (loading_result != 0) {
                free_bmp24(image);
                return loading_result;
            }
            int writing_result = write_negative_bmp_24(image, parameters.dst_file);
            if (writing_result != 0) {
                free_bmp24(image);
                return writing_result;
            }
            free_bmp24(image);
        } else if (bits_per_pixel == 8) {
            BMP_8 *image = malloc(sizeof(*image));
            int loading_result = load_bmp_8(image, parameters.src_file);
            if (loading_result != 0) {
                free_bmp8(image);
                return loading_result;
            }
            int writing_result = write_negative_bmp_8(image, parameters.dst_file);
            if (writing_result != 0) {
                free_bmp8(image);
                return writing_result;
            }
            free_bmp8(image);
        } else {
            error("Format is not supported");
            return -1;
        }
    } else {
        unsigned char r, g, b;
        BMP *bmp = BMP_ReadFile(parameters.src_file);
        BMP_CHECK_ERROR(stderr, -1)
        unsigned long width = BMP_GetWidth(bmp);
        unsigned long height = BMP_GetWidth(bmp);
        if (bmp->Header.BitsPerPixel == 24) {
            for (unsigned int x = 0; x < width; ++x) {
                for (unsigned int y = 0; y < height; ++y) {
                    BMP_GetPixelRGB(bmp, x, y, &r, &g, &b);
                    BMP_SetPixelRGB(bmp, x, y, ~r, ~g, ~b);
                }
            }
        } else if (bmp->Header.BitsPerPixel == 8) {
            for (long long int i = 0; i < BMP_PALETTE_SIZE_8bpp; i++) {
                if ((i + 1) % 4 != 0) {
                    bmp->Palette[i] = ~bmp->Palette[i];
                }
            }
        }
        BMP_WriteFile(bmp, parameters.dst_file);
        BMP_CHECK_ERROR(stderr, -1)
        BMP_Free(bmp);
    }
    return 0;
}