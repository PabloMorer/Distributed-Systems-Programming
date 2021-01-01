#include "bmpBlackWhite.h"
#include "mpi.h"

/** Show log messages */
#define SHOW_LOG_MESSAGES 1

/** Enable output for filtering information */
#define DEBUG_FILTERING 0

/** Show information of input and output bitmap headers */
#define SHOW_BMP_HEADERS 0


int main(int argc, char** argv){

	tBitmapFileHeader imgFileHeaderInput;			/** BMP file header for input image */
	tBitmapInfoHeader imgInfoHeaderInput;			/** BMP info header for input image */
	tBitmapFileHeader imgFileHeaderOutput;			/** BMP file header for output image */
	tBitmapInfoHeader imgInfoHeaderOutput;			/** BMP info header for output image */
	char* sourceFileName;							/** Name of input image file */
	char* destinationFileName;						/** Name of output image file */
	int inputFile, outputFile;						/** File descriptors */
	unsigned char *outputBuffer;					/** Output buffer for filtered pixels */
	unsigned char *inputBuffer;						/** Input buffer to allocate original pixels */
	unsigned char *auxPtr;							/** Auxiliary pointer */
	unsigned int rowSize;							/** Number of pixels per row */
	unsigned int rowsPerProcess;					/** Number of rows to be processed (at most) by each worker */
	unsigned int rowsSentToWorker;					/** Number of rows to be sent to a worker process */
	unsigned int threshold;							/** Threshold */
	unsigned int currentRow;						/** Current row being processed */
	unsigned int currentPixel;						/** Current pixel being processed */
	unsigned int outputPixel;						/** Output pixel */
	unsigned int readBytes;							/** Number of bytes read from input file */
	unsigned int writeBytes;						/** Number of bytes written to output file */
	unsigned int totalBytes;						/** Total number of bytes to send/receive a message */
	unsigned int numPixels;							/** Number of neighbour pixels (including current pixel) */
	unsigned int currentWorker;						/** Current worker process */
	tPixelVector vector;							/** Vector of neighbour pixels */
	int imageDimensions[2];							/** Dimensions of input image */
	double timeStart, timeEnd;						/** Time stamps to calculate the filtering time */
	int size, rank, tag;							/** Number of process, rank and tag */
	MPI_Status status;								/** Status information for received messages */


		// Init
		MPI_Init(&argc, &argv);
		MPI_Comm_size(MPI_COMM_WORLD, &size);
		MPI_Comm_rank(MPI_COMM_WORLD, &rank);
		tag = 1;
		srand(time(NULL));

		// Check the number of processes
		if (size<2){

			if (rank == 0)
				printf ("This program must be launched with (at least) 2 processes\n");

			MPI_Finalize();
			exit(0);
		}

		// Check arguments
		if (argc != 4){

			if (rank == 0)
				printf ("Usage: ./bmpFilterStatic sourceFile destinationFile threshold\n");

			MPI_Finalize();
			exit(0);
		}

		// Get input arguments...
		sourceFileName = argv[1];
		destinationFileName = argv[2];
		threshold = atoi(argv[3]);


		// Master process
		if (rank == 0){

			// Process starts
			timeStart = MPI_Wtime();

			// Read headers from input file
			readHeaders (sourceFileName, &imgFileHeaderInput, &imgInfoHeaderInput);
			readHeaders (sourceFileName, &imgFileHeaderOutput, &imgInfoHeaderOutput);

			// Write header to the output file
			writeHeaders (destinationFileName, &imgFileHeaderOutput, &imgInfoHeaderOutput);

			// Calculate row size for input and output images
			rowSize = (((imgInfoHeaderInput.biBitCount * imgInfoHeaderInput.biWidth) + 31) / 32 ) * 4;
		
			// Show headers...
			if (SHOW_BMP_HEADERS){
				printf ("Source BMP headers:\n");
				printBitmapHeaders (&imgFileHeaderInput, &imgInfoHeaderInput);
				printf ("Destination BMP headers:\n");
				printBitmapHeaders (&imgFileHeaderOutput, &imgInfoHeaderOutput);
			}

			// Open source image
			if((inputFile = open(sourceFileName, O_RDONLY)) < 0){
				printf("ERROR: Source file cannot be opened: %s\n", sourceFileName);
				exit(1);
			}

			// Open target image
			if((outputFile = open(destinationFileName, O_WRONLY | O_APPEND, 0777)) < 0){
				printf("ERROR: Target file cannot be open to append data: %s\n", destinationFileName);
				exit(1);
			}

			// Allocate memory to copy the bytes between the header and the image data
			outputBuffer = (unsigned char*) malloc ((imgFileHeaderInput.bfOffBits-BIMAP_HEADERS_SIZE) * sizeof(unsigned char));

			// Copy bytes between headers and pixels
			lseek (inputFile, BIMAP_HEADERS_SIZE, SEEK_SET);
			read (inputFile, outputBuffer, imgFileHeaderInput.bfOffBits-BIMAP_HEADERS_SIZE);
			write (outputFile, outputBuffer, imgFileHeaderInput.bfOffBits-BIMAP_HEADERS_SIZE);
			
			//volcamos la imagen a procesar 
			unsigned char* imagen ;
			imagen =   (unsigned char*) malloc (rowSize * imgInfoHeaderInput.biHeight * sizeof(unsigned char *));

			read(inputFile, imagen, imgInfoHeaderInput.biHeight * rowSize );
			

			auxPtr = imagen;

			//numero de lineas a procesar por cada worker
			rowsPerProcess = (imgInfoHeaderInput.biHeight) / (size-1);

			//numero de filas extras para el último worker
			int r;
			r = (imgInfoHeaderInput.biSize / rowSize ) % (size-1);
			totalBytes = 0;


			

			currentRow = 0;
			int lastRow = 0;
			for(currentWorker = 1; currentWorker <= size - 1 ; currentWorker++){	
				
				//mandar tamaño de una linea			
				MPI_Send(&rowSize,1,MPI_INT,currentWorker,tag,MPI_COMM_WORLD);

	

				if(currentWorker == size - 1){ //Si es el ultimo worker, puede que gestione más filas
					rowsPerProcess += r;		
				}

					//enviar entre que lineas va a procesar
					MPI_Send(&currentRow, 1, MPI_INT, currentWorker, tag, MPI_COMM_WORLD); //empieza en esta linea

					currentRow += rowsPerProcess;

					MPI_Send(&currentRow, 1, MPI_INT, currentWorker, tag, MPI_COMM_WORLD); //acaba en esta fila
			

					//enviar datos que procesara el worker

					MPI_Send(auxPtr, rowSize * rowsPerProcess, MPI_UNSIGNED_CHAR, currentWorker, tag, MPI_COMM_WORLD);
					auxPtr += rowSize*rowsPerProcess; //actualizamos el puntero

			}

			
			//recoger informacion de los workers
			int firstRow;
			int workerFinished;
			unsigned char* auxPtrR;
			unsigned char* pixelsBuffer;
			pixelsBuffer = (unsigned char*) malloc (rowSize * imgInfoHeaderInput.biHeight * sizeof(unsigned char * ));
			
			for(int i = 1; i <= size - 1 ; i++){
				//recibe la primera linea de los workers, segun vayan terminando
	
				MPI_Recv(&firstRow, 1, MPI_INT, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD, &status);
				workerFinished = status.MPI_SOURCE;

				//linea donde empieza el proceso
				auxPtrR = pixelsBuffer + (firstRow * rowSize);
				
				if(workerFinished == size-1){ //Si es el ultimo			
					rowsPerProcess += r;
				}
				//guardamos en el buffer los datos ya procesados del worker

				MPI_Recv(auxPtrR , rowSize * rowsPerProcess, MPI_UNSIGNED_CHAR,workerFinished, tag, MPI_COMM_WORLD, &status);

				
			}

			//volcamos en outputFile los datos procesados
			write(outputFile, pixelsBuffer, (rowSize * imgInfoHeaderInput.biHeight));

			// Close files
			close (inputFile);
			close (outputFile);

			// Process ends
			timeEnd = MPI_Wtime();


			// Show processing time
			printf("Filtering time: %f\n",timeEnd-timeStart);
		}


		// Worker process
		else{
			int firstRow;
			int lastRow;
			//recibir tamaño de la fila
			MPI_Recv(&rowSize, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
	
			//recibir la primera y la ultima fila
			MPI_Recv(&firstRow, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			MPI_Recv(&lastRow, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

			unsigned char* pixelBuffer = (unsigned char *) malloc (rowSize * (lastRow - firstRow ) * sizeof(unsigned char *));
			unsigned char* sendPixelBuffer = (unsigned char *) malloc (rowSize * (lastRow - firstRow) * sizeof(unsigned char *));
		
			//recibimos contenido de las filas
			
			MPI_Recv(pixelBuffer, rowSize * (lastRow - firstRow), MPI_UNSIGNED_CHAR, 0 , tag, MPI_COMM_WORLD, &status);
		
			//recorrer pixeles de todas las filas


			for(currentRow = 0; currentRow < (lastRow - firstRow); currentRow++){
				//recorremos cada pixel de las filas
				for(int pixel = 0; pixel < rowSize; pixel++){

					if(pixel == 0){
						numPixels = 2;
						vector[0] = pixelBuffer[currentRow * rowSize];
						vector[1] = pixelBuffer[currentRow * rowSize + 1];
					
					}
					else if(pixel == rowSize-1){
						numPixels = 2;
						vector[0] = pixelBuffer[(currentRow * rowSize) + (pixel - 1)];
						vector[1] = pixelBuffer[(currentRow * rowSize) + pixel ];				

					}
					else{
						numPixels = 3;
						vector[0] = pixelBuffer[(currentRow * rowSize) + (pixel - 1)];
						vector[1] = pixelBuffer[(currentRow * rowSize) + pixel ];
						vector[2] = pixelBuffer[(currentRow * rowSize) + (pixel + 1) ];
					}
				//modificamos cada pixel 
				sendPixelBuffer[(currentRow*rowSize) + pixel] = calculatePixelValue(vector, numPixels, threshold, 0);
				
				}


			}
				//enviamos la primera linea y el buffer con los pixels modificados
			MPI_Send(&firstRow, 1, MPI_INT, 0, tag, MPI_COMM_WORLD );
			MPI_Send(sendPixelBuffer, rowSize * (lastRow - firstRow),MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
			
			
		}


		// Finish MPI environment
		MPI_Finalize();


}
