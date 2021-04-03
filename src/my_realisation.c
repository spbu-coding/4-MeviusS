#include "my_realisation.h"

#define error(...) (fprintf(stderr, __VA_ARGS__))

#define MAX_DEVIATION_COUNT 100

int read_bmpheader(BMPHEADER *bmpheader, FILE *fin) {
    if (fread(bmpheader, 54, 1, fin) != 1) {
        error("Some bytes of BMPHEADER were not read");
        return BYTES_READING_ERROR;
    }
    if (bmpheader->bfType != 0x4d42) {
        error("Error in type of bmp file");
        return FILE_TYPE_ERROR;
    }
    if (bmpheader->bfReserved1 != 0x00 || bmpheader->bfReserved2 != 0x00) {
        error("Something was written to reserved nulls at 06-0A");
        return RESERVED_NULLS_ERROR;
    }
//    if (fread(bmpheader, 40, 1, fin) != 1) {
//        error("Some bytes of BITMAPINFOHEADER were not read");
//        return BYTES_READING_ERROR;
//    }
    if (bmpheader->biSize != 0x28) {
        error("Wrong version of file (must be 3)");
        return VERSION_ERROR;
    }
    if (bmpheader->biWidth <= 0) {
        error("Error in width (position 12, must be greater 0)");
        return WIDTH_ERROR;
    }
    if (bmpheader->biHeight == 0) {
        error("Error in height (must not be equal 0)");
        return HEIGHT_ERROR;
    }
    if (bmpheader->biPlanes != 0x01) {
        error("Field biPlane is not equal 1");
        return PLANES_ERROR;
    }
    return 0;
}

int count_padding(LONG width) {
    width *= 3;
    if (width % 4 == 0) {
        return 0;
    } else if (width % 4 == 1) {
        return 3;
    } else if (width % 4 == 2) {
        return 2;
    } else {
        return 1;
    }
}

int load_bmp_24(BMP_24 *image, char const *filename) {
    FILE *input = fopen(filename, "rb");
    if (input == NULL) {
        error("Cannot open file %s\n", filename);
        return FILE_OPEN_ERROR;
    }
    BMPHEADER file_header;
    int correctness_file_header = read_bmpheader(&file_header, input);
    if (correctness_file_header != 0) {
        return correctness_file_header;
    }
    PIXEL **data = (PIXEL **) malloc(sizeof(PIXEL *) * abs(file_header.biHeight));
    if (data == NULL) {
        error("Cannot allocate memory for pixels data");
        return MEMORY_ALLOCATION_ERROR;
    }
    for (int i = 0; i < file_header.biHeight; ++i) {
        data[i] = (PIXEL *) malloc(sizeof(PIXEL) * file_header.biWidth);
        if (data[i] == NULL) {
            error("Cannot allocate memory for %d-th pixel row", i);
            return MEMORY_ALLOCATION_ERROR;
        }
    }
    size_t padding = count_padding(file_header.biWidth);
    BYTE padding_zero;
    for (int i = 0; i < file_header.biHeight; ++i) {
        for (int j = 0; j < file_header.biWidth; ++j) {
            if (fread(&data[i][j], sizeof(PIXEL), 1, input) != 1) {
                error("Cannot read pixel");
                return BYTES_READING_ERROR;
            }
        }
        if (fread(&padding_zero, 1, padding, input) != padding) {
            error("Cannot read pixel");
            return BYTES_READING_ERROR;
        }
    }
    image->file_header = file_header;
    image->data = data;
    fclose(input);
    return 0;
}

int write_negative_bmp_24(BMP_24 *image, char const *filename) {
    FILE *output = fopen(filename, "wb");
    if (output == NULL) {
        error("Cannot open file %s", filename);
        return FILE_OPEN_ERROR;
    }
    if (fwrite(&image->file_header, sizeof(BMPHEADER), 1, output) != 1) {
        error("Some bytes of BMPHEADER were not written");
        exit(BYTES_READING_ERROR);
    }
    size_t padding = count_padding(image->file_header.biWidth);
    BYTE padding_zero = 0x00;
    PIXEL negative_pixel;
    for (int i = 0; i < image->file_header.biHeight; ++i) {
        for (int j = 0; j < image->file_header.biWidth; ++j) {
            negative_pixel.r = ~image->data[i][j].r;
            negative_pixel.g = ~image->data[i][j].g;
            negative_pixel.b = ~image->data[i][j].b;
            if (fwrite(&negative_pixel, sizeof(BYTE), 3, output) != 3) {
                error("Some bytes of pixels data were not written");
                return BYTES_READING_ERROR;
            }
        }
        if (fwrite(&padding_zero, 1, padding, output) != padding) {
            error("Zero bytes were not written");
            return BYTES_READING_ERROR;
        }
    }
    fclose(output);
    return 0;
}

int load_bmp_8(BMP_8 *image, char const *filename) {
    FILE *input = fopen(filename, "rb");
    if (input == NULL) {
        error("Cannot open file %s", filename);
        return FILE_OPEN_ERROR;
    }
    if (image == NULL) {
        error("Cannot allocate memory for loading image");
        return MEMORY_ALLOCATION_ERROR;
    }
    int correctness_file_header = read_bmpheader(&image->file_header, input);
    if (correctness_file_header != 0) {
        return correctness_file_header;
    }
    image->palette = malloc(sizeof(PIXEL) * image->file_header.biClrUsed);
    if (image->palette == NULL) {
        error("Cannot allocate memory for palette");
        return MEMORY_ALLOCATION_ERROR;
    }
    image->data = malloc(sizeof(PIXEL *) * abs(image->file_header.biHeight));
    if (image->data == NULL) {
        error("Cannot allocate memory for pixels data");
        return MEMORY_ALLOCATION_ERROR;
    }
    for (int i = 0; i < image->file_header.biHeight; ++i) {
        image->data[i] = malloc(sizeof(PIXEL) * image->file_header.biWidth);
        if (image->data[i] == NULL) {
            error("Cannon allocate memory for %d-th pixel row", i);
            return MEMORY_ALLOCATION_ERROR;
        }
    }
    BYTE zero_byte;
    for (size_t i = 0; i < image->file_header.biClrUsed; ++i) {
        if (fread(&image->palette[i], sizeof(BYTE), 3, input) != 3) {
            error("Some bytes of palette were not read");
            return BYTES_READING_ERROR;
        }
        if (fread(&zero_byte, sizeof(BYTE), 1, input) != 1) {
            error("0x00 byte after %zu-th node in palette was nor read", i);
            return BYTES_READING_ERROR;
        }
    }
    for (int i = 0; i < image->file_header.biHeight; ++i) {
        for (int j = 0; j < image->file_header.biWidth; ++j) {
            if (fread(&image->data[i][j], sizeof(BYTE), 1, input) != 1) {
                error("Some bytes of pixels data were not read");
                return BYTES_READING_ERROR;
            }
        }
    }
    fclose(input);
    return 0;
}

int write_negative_bmp_8(BMP_8 *image, char const *filename) {
    BYTE zero_byte = 0x00;
    FILE *output = fopen(filename, "wb");
    if (output == NULL) {
        error("Cannot open file %s", filename);
        return FILE_OPEN_ERROR;
    }
    if (fwrite(&image->file_header, sizeof(BMPHEADER), 1, output) != 1) {
        error("Some bytes of BMPHEADER were not written");
        return BYTES_READING_ERROR;
    }
    for (size_t i = 0; i < image->file_header.biClrUsed; ++i) {
        PIXEL color = {~image->palette[i].r, ~image->palette[i].g, ~image->palette[i].b};
        if (fwrite(&color, sizeof(PIXEL), 1, output) != 1) {
            error("Some bytes of palette were not written");
            return BYTES_READING_ERROR;
        }
        if (fwrite(&zero_byte, sizeof(BYTE), 1, output) != 1) {
            error("0x00 byte after %lu-th node in palette was nor written", i);
            return BYTES_READING_ERROR;
        }
    }
    for (int i = 0; i < image->file_header.biHeight; ++i) {
        for (int j = 0; j < image->file_header.biWidth; ++j) {
            if (fwrite(&image->data[i][j], sizeof(BYTE), 1, output) != 1) {
                error("Some bytes of pixels data were not written");
                return BYTES_READING_ERROR;
            }
        }
    }
    fclose(output);
    return 0;
}

int return_bits_per_pixel(char const *filename) {
    FILE *input = fopen(filename, "rb");
    if (input == NULL) {
        error("Can not open file: %s", filename);
        return FILE_OPEN_ERROR;
    }
    fseek(input, 28, SEEK_SET);
    int bits = 0;
    fread(&bits, 1, 2, input);
    fseek(input, 0, SEEK_SET);
    fclose(input);
    return bits;
}

int compare_pixels(PIXEL a, PIXEL b) {
    return (a.r - b.r || a.b - b.b || a.g - b.g);
}

int compare_bmp8(BMP_8 *image1, BMP_8 *image2) {
    LONG width1 = image1->file_header.biWidth;
    LONG height1 = image1->file_header.biHeight;
    LONG width2 = image2->file_header.biWidth;
    LONG height2 = image2->file_header.biHeight;
    if (width1 != width2) {
        error("Images have different width");
        return -1;
    }
    if (abs(height1) != abs(height2)) {
        error("Images have different height");
        return -1;
    }
    BYTE **data1 = image1->data;
    BYTE **data2 = image2->data;
    int deviation_counter = 0;
    int abs_height = abs(height1);
    for (int i = 0; i < abs_height && deviation_counter < MAX_DEVIATION_COUNT; ++i) {
        for (int j = 0; j < width1 && deviation_counter < MAX_DEVIATION_COUNT; ++j) {
            int y_coord_1 = height1 > 0 ? i : abs_height - i - 1;
            int y_coord_2 = height2 > 0 ? i : abs_height - i - 1;
            if (compare_pixels(image1->palette[data1[y_coord_1][j]], image2->palette[data2[y_coord_2][j]]) != 0) {
                deviation_counter++;
                printf("%d %d\n", i, j);
            }
        }
    }
    return 0;
}

int compare_bmp24(BMP_24 *image1, BMP_24 *image2) {
    LONG width1 = image1->file_header.biWidth;
    LONG height1 = image1->file_header.biHeight;
    LONG width2 = image2->file_header.biWidth;
    LONG height2 = image2->file_header.biHeight;
    if (width1 != width2) {
        error("Images have different width");
        return -1;
    }
    if (abs(height1) != abs(height2)) {
        error("Images have different height");
        return -1;
    }
    PIXEL **data1 = image1->data;
    PIXEL **data2 = image2->data;
    int deviation_counter = 0;
    int abs_height = abs(height1);
    for (int i = 0; i < abs_height && deviation_counter < MAX_DEVIATION_COUNT; ++i) {
        for (int j = 0; j < width1 && deviation_counter < MAX_DEVIATION_COUNT; ++j) {
            int y_coord_1 = height1 > 0 ? i : abs_height - i - 1;
            int y_coord_2 = height2 > 0 ? i : abs_height - i - 1;
            if (compare_pixels(data1[y_coord_1][j], data2[y_coord_2][j]) != 0) {
                deviation_counter++;
                printf("%d %d\n", i, j);
            }
        }
    }
    return 0;
}

void free_bmp8(BMP_8 *image) {
    free(image->palette);
    for (int i = 0; i < image->file_header.biHeight; ++i) {
        free(image->data[i]);
    }
    free(image->data);
}

void free_bmp24(BMP_24 *image) {
    for (int i = 0; i < image->file_header.biHeight; ++i) {
        free(image->data[i]);
    }
    free(image->data);
}