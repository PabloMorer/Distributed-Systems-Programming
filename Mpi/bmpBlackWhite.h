#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

/** Vector size for calculating the final pixel value */
#define VECTOR_SIZE 3

/** Header size */
#define BIMAP_HEADERS_SIZE 54

/** White color */
#define WHITE_COLOR 255

/** Black color */
#define BLACK_COLOR 0


// BMP file header
typedef struct{
	unsigned short bfType;
	unsigned int bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned int bfOffBits;
} tBitmapFileHeader;


// BMP info header
typedef struct{
	unsigned int  biSize;
	unsigned int  biWidth;
	unsigned int  biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
    unsigned int  biCompression;
    unsigned int  biSizeImage;
    unsigned int  biXPelsPerMeter;
    unsigned int  biYPelsPerMeter;
    unsigned int  biClrUsed;
    unsigned int biClrImportant;
} tBitmapInfoHeader;

/** Vector of neighbour pixels */
typedef unsigned char tPixelVector[VECTOR_SIZE];


/**
 * Show an error message
 * @param msg Error message
 */
int showError (char* msg);

/**
 * Read bitmap headers from file
 *
 * @param fileName Bmp file
 * @param bmHeader Bmp file header
 * @param bmInfoHeader Bmp info header
 */
void readHeaders (char* fileName, tBitmapFileHeader *bmHeader, tBitmapInfoHeader *bmInfoHeader);

/**
 * Write bitmap headers to file
 *
 * @param fileName Bmp file
 * @param bmHeader Bmp file header
 * @param bmInfoHeader Bmp info header
 */
void writeHeaders (char* fileName, tBitmapFileHeader *bmHeader, tBitmapInfoHeader *bmInfoHeader);

/**
 * Show the information of bitmap headers
 *
 * @param bmHeader Bmp file header
 * @param bmInfoHeader Bmp info header
 */
void printBitmapHeaders (tBitmapFileHeader *bmHeader, tBitmapInfoHeader *bmInfoHeader);

/**
 * Calculate the pixel value (black of white) given its neighbours and a threshold.
 *
 * @param vector Vector that contains the neighbour pixels.
 * @param numPixels Number of involved neighbour pixels.
 * @param threshold Threshold value.
 * @param debug Show information about the filtering process.
 * @return WHITE_COLOR if the resulting pixel es white, or BLACK_COLOR if the filtered pixel is black
 */
unsigned char calculatePixelValue (tPixelVector vector, unsigned int numPixels, unsigned int threshold, int debug);
