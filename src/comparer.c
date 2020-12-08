#include "mybmp.h"

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
			fprintf(stderr, "File structure error! (Reserved bits ot equal to zero)");
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

int is_uncomparable(bmp_t *bmp1, bmp_t *bmp2)
{
	if (bmp1->width != bmp2->width || bmp1->height != bmp2->height)
	{
		fprintf(stderr, "Different size of images!");
		
		return -1;
	}
	
	return 0;
}

int is_equal(bmp_t *bmp1, bmp_t *bmp2)
{
	BYTE r1, g1, b1, r2, g2, b2;
	
	for (unsigned int i = 0; i < bmp1->width; i++)
			for(unsigned int j = 0; j < bmp1->height; j++)
			{
				get_pixel(bmp1, i, j, &r1, &g1, &b1);
				get_pixel(bmp2, i, j, &r2, &g2, &b2);
				
				if (r1 != r2 || g1 != g2 || b1 != b2)
					return 0;
			}
	
	return 1;
}

int main(int argc, char **argv)
{	
	if (argc != 3)
	{
		fprintf(stderr, "Incorrect parameters!");
	
		return -1;
	}
	
	int error_code = 0;
	
	FILE *f = fopen(argv[1], "rb");
	if (f == NULL)
		return write_error(INPUT_ERROR);
	bmp_t bmp1;
	init_bmp_t(&bmp1);
	error_code = read_bmp(f, &bmp1);
	if (error_code)
		return -1;
	fclose(f);
	
	f = fopen(argv[2], "rb");
	if (f == NULL)
		return write_error(INPUT_ERROR);
	bmp_t bmp2;
	init_bmp_t(&bmp2);
	error_code = read_bmp(f, &bmp2);
	if (error_code)
		return -1;
	fclose(f);
	
	if (is_uncomparable(&bmp1, &bmp2))
		return -1;
		
	bmp1.height = abs(bmp1.height);
	bmp2.height = abs(bmp2.height);
	
	if (is_equal(&bmp1, &bmp2))
	{
		printf("Equal!\n");
	}
	else
	{
		printf("Unequal\n");
		
		const unsigned int max_iterations = 100;
		
		int number_of_iterations = 0;
		
		BYTE r1, g1, b1, r2, g2, b2;
	
		for (unsigned int i = 0; i < bmp1.width; i++)
			for(unsigned int j = 0; j < bmp1.height; j++)
			{		
				get_pixel(&bmp1, i, j, &r1, &g1, &b1);
				get_pixel(&bmp2, i, j, &r2, &g2, &b2);
				
				if (r1 != r2 || g1 != g2 || b1 != b2)
				{	
					printf("%d %d\n", i, j);
					
					number_of_iterations++;
					
					if (number_of_iterations >= max_iterations)
						break;
				}
			}
	}
	
	free_bmp_t(&bmp1);
	free_bmp_t(&bmp2);
		
	return 0;
}