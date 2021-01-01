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
	unsigned int receivedRows;						/** Total number of received rows */
	unsigned int threshold;							/** Threshold */
	unsigned int currentRow;						/** Current row being processed */
	unsigned int currentPixel;						/** Current pixel being processed */
	unsigned int outputPixel;						/** Output pixel */
	unsigned int readBytes;							/** Number of bytes read from input file */
	unsigned int writeBytes;						/** Number of bytes written to output file */
	unsigned int totalBytes;						/** Total number of bytes to send/receive a message */
	unsigned int numPixels;							/** Number of neighbour pixels (including current pixel) */
	unsigned int currentWorker;						/** Current worker process */
	unsigned int *processIDs;
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
		if (argc != 5){

			if (rank == 0)
				printf ("Usage: ./bmpFilterDynamic sourceFile destinationFile threshold numRows\n");

			MPI_Finalize();
			exit(0);
		}

		// Get input arguments...
		sourceFileName = argv[1];
		destinationFileName = argv[2];
		threshold = atoi(argv[3]);
		rowsPerProcess = atoi(argv[4]);

		// Allocate memory for process IDs vector
		processIDs = (unsigned int *) malloc (size*sizeof(unsigned int));

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

			// Show info before processing
			if (SHOW_LOG_MESSAGES){
				printf ("[MASTER] Applying filter to image %s (%dx%d) with threshold %d. Generating image %s\n", sourceFileName,
						rowSize, imgInfoHeaderInput.biHeight, threshold, destinationFileName);
				printf ("Number of workers:%d -> Each worker calculates (at most) %d rows\n", size-1, rowsPerProcess);
			}
 
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
			currentRow = 0;

			currentWorker= 1;

			int flag;


			while( currentRow < imgInfoHeaderInput.biHeight && currentWorker <= size - 1 ){

				//enviamos flag
				flag = 1;
				MPI_Send(&flag, 1, MPI_INT,currentWorker,tag, MPI_COMM_WORLD);

				//Enviamos tam de linea
				MPI_Send(&rowSize,1,MPI_INT,currentWorker,tag,MPI_COMM_WORLD);


				//Si el numero de lineas que quedan por procesar es menor que el tamaño del grano, se envian solo las lineas necesarias
				if((imgInfoHeaderInput.biHeight - currentRow ) < rowsPerProcess){

						rowsPerProcess = imgInfoHeaderInput.biHeight - currentRow;
				}

				//enviamos inicio de filas a procesar
				MPI_Send(&currentRow, 1, MPI_INT, currentWorker, tag, MPI_COMM_WORLD);
				
				currentRow +=  rowsPerProcess;
			
				//enviamos ultima fila

				MPI_Send(&currentRow, 1, MPI_INT, currentWorker, tag, MPI_COMM_WORLD);


				//enviamos buffer de datos

				MPI_Send(auxPtr, rowSize * rowsPerProcess, MPI_UNSIGNED_CHAR, currentWorker, tag, MPI_COMM_WORLD);
				
				auxPtr += rowSize*rowsPerProcess;

				currentWorker++;
			}
			while(currentWorker < size){	//si no hemos abarcado todos los workers...


				int auxflag = 0;	//	Mandamos flag 0;
				MPI_Send(&auxflag, 1, MPI_INT,currentWorker,tag, MPI_COMM_WORLD);
				MPI_Send(&auxflag,1,MPI_INT,currentWorker,tag,MPI_COMM_WORLD);
				currentWorker++;




			}



			

			int receivedRows = 0;
			int workerFinished;
	
			unsigned char* auxPtrR;
			unsigned char* pixelsBuffer;
			pixelsBuffer = (unsigned char*) malloc (rowSize * imgInfoHeaderInput.biHeight * sizeof(unsigned char * ));
			int numRows = 0;
			int firstRow;
			int lastRow;

			while(receivedRows < imgInfoHeaderInput.biHeight){
				//Recibimos lineas procesadas

				MPI_Recv(&firstRow,1,MPI_INT,MPI_ANY_SOURCE,tag,MPI_COMM_WORLD, &status); //recibimos la primera linea
				workerFinished = status.MPI_SOURCE; //guardamos el worker con el que vamos a trabajar
				
				
				MPI_Recv(&lastRow,1,MPI_INT,workerFinished,tag,MPI_COMM_WORLD, &status); //recibimos la ultima linea
				
				
				receivedRows += (lastRow - firstRow); //obtenemos el numero de lineas que ha procesado
			
				//linea donde empieza el proceso
				auxPtrR = pixelsBuffer + (firstRow * rowSize);

				//recibimos el buffer con los datos modificados
				MPI_Recv(auxPtrR , rowSize * (lastRow - firstRow), MPI_UNSIGNED_CHAR, workerFinished, tag, MPI_COMM_WORLD, &status);

				

				if(currentRow < imgInfoHeaderInput.biHeight){//si quedan lineas por procesar
				//Si el numero de lineas que quedan por procesar es menor que el tamaño del grano, se envian solo las lineas necesarias
					if((imgInfoHeaderInput.biHeight - currentRow ) < rowsPerProcess){

						rowsPerProcess = imgInfoHeaderInput.biHeight - currentRow;
					}

					flag = 1;
					//enviamos flag para que siga procesando lineas
					MPI_Send(&flag, 1,MPI_INT,status.MPI_SOURCE, tag, MPI_COMM_WORLD);
					//enviamos primera linea a procesar
					MPI_Send(&currentRow, 1, MPI_INT, workerFinished, tag, MPI_COMM_WORLD);
				
					currentRow +=  rowsPerProcess;
			
					//enviamos ultima fila a procesar
	
					MPI_Send(&currentRow, 1, MPI_INT, workerFinished, tag, MPI_COMM_WORLD);

					//enviamos buffer de datos

					MPI_Send(auxPtr, rowSize * rowsPerProcess, MPI_UNSIGNED_CHAR, workerFinished, tag, MPI_COMM_WORLD);

					auxPtr += rowSize*rowsPerProcess; //actualizamos puntero
					

				}else{

					//enviamos buffer de datos
					flag = -1;
					MPI_Send(&flag, 1,MPI_INT,workerFinished, tag, MPI_COMM_WORLD);
		

				}
				


			}

	

			//volcamos informacino del buffer en el fichero de salida
			write(outputFile, pixelsBuffer, (rowSize * imgInfoHeaderInput.biHeight));

			free(pixelsBuffer);
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

			unsigned char* pixelBuffer ;
			unsigned char* sendPixelBuffer ;
			//Recogemos flag
			int flag = -1;
			MPI_Recv(&flag, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
			//recogemos tamaño de la fila
			MPI_Recv(&rowSize, 1, MPI_INT,0, tag, MPI_COMM_WORLD, &status);
			int firstRow;
			int lastRow;
			int numFilas = 0;
			//mientras flag = 1, procesamos lineas
			while(flag == 1){
				
		
				//Recibimos primera linea
				MPI_Recv(&firstRow, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
				//Recibimos ultima linea
				MPI_Recv(&lastRow, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
	
				 pixelBuffer = (unsigned char *) malloc (rowSize * (lastRow - firstRow ) * sizeof(unsigned char *));
				 sendPixelBuffer = (unsigned char *) malloc (rowSize * (lastRow - firstRow) * sizeof(unsigned char *));

				numFilas = lastRow - firstRow;
				
				//recibimos datos a modificar
				MPI_Recv(pixelBuffer, rowSize * (lastRow - firstRow), MPI_UNSIGNED_CHAR, 0 , tag, MPI_COMM_WORLD, &status);
			
				//recorremos numero de filas
				
				for(currentRow = 0; currentRow < numFilas ; currentRow++){
					//por cada fila recorremos cada pixel
					for(int pixel = 0; pixel < rowSize ; pixel++){
				
						if(pixel == 0){
							
							numPixels = 2;

							vector[0] = pixelBuffer[currentRow * rowSize];
							vector[1] = pixelBuffer[currentRow * rowSize + 1];

							
					
						}
						else if(pixel == rowSize-2){
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

						//modificamos el pixel
						sendPixelBuffer[(currentRow*rowSize) + pixel] = calculatePixelValue(vector, numPixels, threshold, 0);
					
						
	
						
					}

					
				
				}

					//enviamos la primera linea que hemos modificado
					MPI_Send(&firstRow, 1, MPI_INT, 0, tag, MPI_COMM_WORLD );
					//enviamos la ultima linea que hemos modificado
					MPI_Send(&lastRow, 1, MPI_INT, 0, tag, MPI_COMM_WORLD );
					//enviamos buffer con los datos modificados
					MPI_Send(sendPixelBuffer, rowSize * (lastRow - firstRow),MPI_UNSIGNED_CHAR, 0, tag, MPI_COMM_WORLD);
			
					
					//recibimos flag;
					MPI_Recv(&flag, 1, MPI_INT, 0, tag, MPI_COMM_WORLD, &status);

				free(pixelBuffer);
				free(sendPixelBuffer);

			}


			




				
			

			}


		// Finish MPI environment
	
		MPI_Finalize();


}
