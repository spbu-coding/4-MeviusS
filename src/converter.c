#include "mybmp.h"
#include "qdbmp.h"

#define PAR_MINE_SIZE 6
#define PAR_THEIRS_SIZE 8

int write_error(int error_code)
{
	switch (error_code)
	{
		case INPUT_ERROR:
		{
			fprintf(stderr, "Input error!");
			return -1;
		}
		
		case OUTPUT_ERROR:
		{
			fprintf(stderr, "Output error!");
			return -1;
		}
		
		case OUT_OF_MEMORY_ERROR:
		{
			fprintf(stderr, "Out of memory!");
			return -1;
		}
		
		case OFFSET_ERROR:
		{
			fprintf(stderr, "File structure error! (Bit offset too small)");
			return -2;
		}
		
		case INCORRECT_HEIGHT:
		{
			fprintf(stderr, "File structure error! (Incorrect height of image)");
			return -2;
		}
		
		case INCORRECT_WIDTH:
		{
			fprintf(stderr, "File structure error! (Incorrect witdh of image)");
			return -2;
		}
		
		case NOT_RESERVED:
		{
			fprintf(stderr, "File structure error! (Reserved bits not equal to zero)");
			return -2;
		}
		
		case PLANES_NOT_EQ_1:
		{
			fprintf(stderr, "File structure error! (Incorrect number of planes)");
			return -2;
		}
		
		case INCORRECT_SIGNATURE:
		{
			fprintf(stderr, "File is not a BMP!");
			return -2;
		}
		
		case INCORRECT_BITS_PER_PIXEL:
		{
			fprintf(stderr, "Bits per pixel not equal to 8 or 24!");
			return -2;
		}
	}
	
	return 0;
}

void to_negative_mine(bmp_t *bmp)
{
	BYTE r, g, b;
	
	if (bmp->bits_per_pixel == 8)
	{
		for (unsigned int i = 0; i < 256; i++)
    	{
        	get_palette_color(bmp, i, &r, &g, &b);
        	set_palette_color(bmp, i, ~r, ~g, ~b);
    	}
	}
	else
	{
		for (unsigned int i = 0; i < bmp->width; i++)
			for(unsigned int j = 0; j < bmp->height; j++)
			{
				get_pixel(bmp, i, j, &r, &g, &b);

				set_pixel(bmp, i, j, ~r, ~g, ~b);
			}
	}
}

void to_negative_theirs(BMP *bmp)
{
	BYTE r, g, b;
	
	if (BMP_GetDepth(bmp) == 8)
	{
		for (unsigned int i = 0; i < 256; i++)
    	{
        	BMP_GetPaletteColor(bmp, i, &r, &g, &b);
        	BMP_SetPaletteColor(bmp, i, ~r, ~g, ~b);
    	}
	}
	else
	{
		unsigned int width = BMP_GetWidth(bmp), height = BMP_GetHeight(bmp);
		
		for (unsigned int i = 0; i < width; i++)
			for (unsigned int j = 0; j < height; j++)
			{
				BMP_GetPixelRGB(bmp, i, j, &r, &g, &b);
				BMP_SetPixelRGB(bmp, i, j, ~r, ~g, ~b);
			}
	}
}

int main(int argc, char **argv)
{	
	if (argc != 4 || ((strlen(argv[1]) != PAR_MINE_SIZE || strncmp(argv[1], "--mine", PAR_MINE_SIZE)) && (strlen(argv[1]) != PAR_THEIRS_SIZE || strncmp(argv[1], "--theirs", PAR_THEIRS_SIZE))))
	{
		fprintf(stderr, "Incorrect parameters!");
	
		return -1;
	}
	
	int error_code = 0;
	
	if (strlen(argv[1]) == PAR_MINE_SIZE)
	{
		FILE *f = fopen(argv[2], "rb");
	
		if (f == NULL)
			return write_error(INPUT_ERROR);

		bmp_t bmp;
		init_bmp_t(&bmp);
		
		error_code = read_bmp(f, &bmp);
		
		if (error_code)
			return write_error(error_code);
		
		fclose(f);
		
		bmp.height = abs(bmp.height);
		to_negative_mine(&bmp);
		f = fopen(argv[3], "wb");
	
		if (f == NULL)
			return write_error(OUTPUT_ERROR);

		error_code = write_bmp(f, &bmp);
		
		if (error_code)
			return write_error(error_code);
			
		free_bmp_t(&bmp);
	}
	else
	{
		UCHAR	r, g, b;
		UINT	width, height;
		UINT	x, y;
		BMP*	bmp;
		
		bmp = BMP_ReadFile(argv[2]);
		BMP_CHECK_ERROR(stdout, -3);

		width = BMP_GetWidth(bmp);
		height = BMP_GetHeight(bmp);
		
		to_negative_theirs(bmp);

		BMP_WriteFile(bmp, argv[3]);
		BMP_CHECK_ERROR(stderr, -3);

		BMP_Free(bmp);
	}
	
	return 0;
}