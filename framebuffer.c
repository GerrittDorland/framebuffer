/************************/
/*	FrameBuffer	*/
/*			*/
/*  @Gerritt Dorland	*/
/************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>	//for Windows......
#include "generateImg.h"
#include "oglwindow.h"

#define NUMBARS 8

//Function definitions
void fillBuffer(void *fb_buf, int fb_h, int fb_w, int fb_stride, int32_t *fb_pixval);

void printBuffer(void *fb_buf, int fb_width, int fb_height, int stride, FILE *inFile);
/********************/

int main(int argc, char **argv)
{
	FILE *imageFile;
	int32_t pixVal[8] = {1,2,3,4,5,6,7,8};
	int width = 1080;
	int height = 720;
	int stride = 1090;
	void *frameBuffer = NULL;

	if(argc > 1)
	{
		if(argc < 4)
		{
			fprintf(stderr, "\n\nNot enough arguments provided. You need to enter three: width, height, and stride.\n\tExample: ./framebuffer 1080 720 1090\n\n");
			return 0;
		}

		//Set values for image size/stride from command line
		width = atoi(argv[1]);
		height = atoi(argv[2]);
		stride = atoi(argv[3]);

		if(stride < width)
		{
			fprintf(stderr, "\n\nWidth: %d\nHeight: %d\nStride: %d\n\n", width, height, stride);
			fprintf(stderr, "\n\nStride must be equal or less than width.\n\n");
			return 0;
		}

		else if(width < 8)
		{
			fprintf(stderr, "\n\nWidth: %d\nHeight: %d\nStride: %d\n\n", width, height, stride);
			fprintf(stderr, "\n\nWidth must be greater than 8.\n\n");
			return 0;
		}

		else if(height < 1)
		{
			fprintf(stderr, "\n\nWidth: %d\nHeight: %d\nStride: %d\n\n", width, height, stride);
			fprintf(stderr, "\n\nHeight must be greater than 0.\n\n");
			return 0;
		}
	}

	//Open image file
	imageFile = fopen("out.ppm", "wb");
	(void) fprintf(imageFile, "P6\n%d %d\n255\n", width, height);	//print header for image.

	/*
		-- setup nonsense --
	*/
	int count;
	
	fprintf(stderr, "\nWidth: %d\nHeight: %d\nStride: %d\n\n", width, height, stride);

	//Multiplying by sizeof(uint32_t) unnecessary - calloc takes care of the address size. Only need a pixel count!
	count = height * stride;

	frameBuffer = (int32_t*)calloc(count, sizeof(int32_t));		//Allocate enough memory for every pixel, including the stride

	fillBuffer(frameBuffer, height, width, stride, pixVal);
	printBuffer(frameBuffer, width, height, stride, imageFile);

	free(frameBuffer);

	fprintf(stderr, "\n\n>> Job's Done <<\n\n");
	
	(void)fclose(imageFile);
	return 0;
}

//			Interview Section
//-------------------------------------------------------------------
void fillBuffer(void *fb_buf, int fb_h, int fb_w, int fb_stride, int32_t *fb_pixval)
{
	//Variables
	int i = 0, q, z;
	int barWidth, barRemainder, barRemainderSaveSpace;
	int32_t reducedStride;
	int32_t *pBuf = fb_buf;
	int32_t currentColor;

	reducedStride = fb_stride - fb_w;			//Cut the value of the stride down to the difference between stride and viewport width
	barWidth = fb_w / NUMBARS;				//Get the width of each color bar
	barRemainderSaveSpace = fb_w % NUMBARS;			//Get how many extra pixels we may have per line

	for(z = 0; z < fb_h; z++)				// For (the number of vertical lines on the viewport...)
	{
		barRemainder = barRemainderSaveSpace;		//Reset the remainder value. Need to do this every vertical line, else we'd only
								//	correct the pixels on one horizontal line.

		for(q = 0; q < NUMBARS; q++)			//For (the number of color bars...)
		{
			currentColor = fb_pixval[q];

			for(i = 0; i < barWidth; i++)		//For (the width of each color bar...)
			{
				*pBuf++ = currentColor;		//Store the current color in the buffer and move to the next address.
			}

			if(barRemainder > 0)			//If there are some remaining pixels, print up to one extra pixel per-line to account for this.
			{					//	As there can only be up to 7 extra pixels, this method works well.
				*pBuf++ = currentColor;
				barRemainder--;
			}
		}
		
		pBuf+= reducedStride;				//Jump over the junk space that is indicated by the stride value.
	}
}
//-------------------------------------------------------------------

void printBuffer(void *fb_buf, int fb_width, int fb_height, int stride, FILE* inFile)
{
	//Variables
	int i = 0, q, z;
	int barWidth, barRemainder, barRemainderSaveSpace;
	int32_t reducedStride;
	int32_t *pBuf = fb_buf;

	reducedStride = stride - fb_width;
	barWidth = fb_width / NUMBARS;
	barRemainderSaveSpace = fb_width % NUMBARS;

	for(z = 0; z < fb_height; z++)				//This uses the same algorithm as 'fillBuffer()', so I'm going to skip commenting it
	{							//	and go back to work, haha!
		barRemainder = barRemainderSaveSpace;

		for(q = 0; q < NUMBARS; q++)
		{

			for(i = 0; i < barWidth; i++)
			{
				generateImg(*pBuf, inFile);	//Print pixel to image file
				printf("%d ", *pBuf++);
			}

			if(barRemainder > 0)
			{
				generateImg(*pBuf, inFile);	//Print pixel to image file
				printf("%d ", *pBuf++);
				barRemainder--;
			}
		}

		pBuf += reducedStride;		

		printf("\n");
	}
   openWindow();
}
