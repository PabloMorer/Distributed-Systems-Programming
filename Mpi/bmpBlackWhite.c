#include "bmpBlackWhite.h"


int showError (char* msg){

	printf ("ERROR:%s\n", msg);
	exit (1);
}

void readHeaders (char* fileName, tBitmapFileHeader *bmHeader, tBitmapInfoHeader *bmInfoHeader){

	int inputFile;

		// Open image file
		if((inputFile = open(fileName, O_RDONLY)) < 0){
			printf("ERROR: Source file cannot be opened: %s\n", fileName);
			exit (1);
		}

		// Read BMP file header (input)
		read(inputFile, &(bmHeader->bfType), sizeof (unsigned short));
		read(inputFile, &(bmHeader->bfSize), sizeof (unsigned int));
		read(inputFile, &(bmHeader->bfReserved1), sizeof (unsigned short));
		read(inputFile, &(bmHeader->bfReserved2), sizeof (unsigned short));
		read(inputFile, &(bmHeader->bfOffBits), sizeof (unsigned int));

		// Read BMP info header (input)
		read(inputFile, &(bmInfoHeader->biSize), sizeof(unsigned int));
		read(inputFile, &(bmInfoHeader->biWidth), sizeof(unsigned int));
		read(inputFile, &(bmInfoHeader->biHeight), sizeof(unsigned int));
		read(inputFile, &(bmInfoHeader->biPlanes), sizeof(unsigned short));
		read(inputFile, &(bmInfoHeader->biBitCount), sizeof(unsigned short));
		read(inputFile, &(bmInfoHeader->biCompression), sizeof(unsigned int));
		read(inputFile, &(bmInfoHeader->biSizeImage), sizeof(unsigned int));
		read(inputFile, &(bmInfoHeader->biXPelsPerMeter), sizeof(unsigned int));
		read(inputFile, &(bmInfoHeader->biYPelsPerMeter), sizeof(unsigned int));
		read(inputFile, &(bmInfoHeader->biClrUsed), sizeof(unsigned int));
		read(inputFile, &(bmInfoHeader->biClrImportant), sizeof(unsigned int));

		// Close file
		close (inputFile);
}

void writeHeaders (char* fileName, tBitmapFileHeader *bmHeader, tBitmapInfoHeader *bmInfoHeader){

	int outputFile;

		// Create a new image file
		if((outputFile = open(fileName, O_WRONLY | O_TRUNC | O_CREAT, 0777)) < 0){
			printf("ERROR: Target file cannot be created: %s\n", fileName);
			exit (1);
		}

		// Read BMP file header (input)
		write(outputFile, &(bmHeader->bfType), sizeof (unsigned short));
		write(outputFile, &(bmHeader->bfSize), sizeof (unsigned int));
		write(outputFile, &(bmHeader->bfReserved1), sizeof (unsigned short));
		write(outputFile, &(bmHeader->bfReserved2), sizeof (unsigned short));
		write(outputFile, &(bmHeader->bfOffBits), sizeof (unsigned int));

		// Read BMP info header (input)
		write(outputFile, &(bmInfoHeader->biSize), sizeof(unsigned int));
		write(outputFile, &(bmInfoHeader->biWidth), sizeof(unsigned int));
		write(outputFile, &(bmInfoHeader->biHeight), sizeof(unsigned int));
		write(outputFile, &(bmInfoHeader->biPlanes), sizeof(unsigned short));
		write(outputFile, &(bmInfoHeader->biBitCount), sizeof(unsigned short));
		write(outputFile, &(bmInfoHeader->biCompression), sizeof(unsigned int));
		write(outputFile, &(bmInfoHeader->biSizeImage), sizeof(unsigned int));
		write(outputFile, &(bmInfoHeader->biXPelsPerMeter), sizeof(unsigned int));
		write(outputFile, &(bmInfoHeader->biYPelsPerMeter), sizeof(unsigned int));
		write(outputFile, &(bmInfoHeader->biClrUsed), sizeof(unsigned int));
		write(outputFile, &(bmInfoHeader->biClrImportant), sizeof(unsigned int));

		// Close file
		close (outputFile);
}

void printBitmapHeaders (tBitmapFileHeader *bmHeader, tBitmapInfoHeader *bmInfoHeader){

	printf (" tBitmapFileHeader:\n");
	printf ("\t- bfType :%hu\n", bmHeader->bfType);
	printf ("\t- bfSize :%d\n", bmHeader->bfSize);
	printf ("\t- bfReserved1 :%hu\n", bmHeader->bfReserved1);
	printf ("\t- bfReserved2 :%hu\n", bmHeader->bfReserved2);
	printf ("\t- bfOffBits :%d\n", bmHeader->bfOffBits);

	printf (" tBitmapInfoHeader:\n");
	printf ("\t- biSize :%d\n", bmInfoHeader->biSize);
	printf ("\t- biWidth :%d\n", bmInfoHeader->biWidth);
	printf ("\t- biHeight :%d\n", bmInfoHeader->biHeight);
	printf ("\t- biPlanes :%hu\n", bmInfoHeader->biPlanes);
	printf ("\t- biBitCount :%hu\n", bmInfoHeader->biBitCount);
	printf ("\t- biCompression :%d\n", bmInfoHeader->biCompression);
	printf ("\t- biSizeImage :%d\n", bmInfoHeader->biSizeImage);
	printf ("\t- biXPelsPerMeter :%d\n", bmInfoHeader->biXPelsPerMeter);
	printf ("\t- biYPelsPerMeter :%d\n", bmInfoHeader->biYPelsPerMeter);
	printf ("\t- biClrUsed :%d\n", bmInfoHeader->biClrUsed);
	printf ("\t- biClrImportant :%d\n", bmInfoHeader->biClrImportant);

	printf ("\n");
}

unsigned char calculatePixelValue (tPixelVector vector, unsigned int numPixels, unsigned int threshold, int debug){

	unsigned int mean;
	unsigned char pixelValue;
	int randomNumber;
	float result;
	int i;

		// Init...
		pixelValue = mean = 0;

		if (numPixels > VECTOR_SIZE){
			printf ("Error in function calculatePixelValue. Parameter numPixels=%d. It must be <= %d\n", numPixels, VECTOR_SIZE);
			exit (1);
		}

		// Random processing
		randomNumber = rand() % 300;
		for (i=0; i<randomNumber; i++){
			result = ((float)rand()/(float)(RAND_MAX)) * 0.42354435;
		}

		// Calculate Ac. mean
		for (i=0; i<numPixels; i++){

			if (debug)
				printf ("[%d]  -  ", vector[i]);

			mean += (unsigned int) vector[i];
		}

		// Calculate mean
		mean = mean/numPixels;

		// Calculate pixel value
		if (mean>threshold)
			pixelValue = WHITE_COLOR;
		else
			pixelValue = BLACK_COLOR;

		if (debug)
			printf ("Mean:%d  -  Pixel value:%d\n", mean, pixelValue);

	return pixelValue;
}
