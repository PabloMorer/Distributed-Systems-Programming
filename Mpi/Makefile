CC = gcc
CFLAGS = -g -m32

all: bmpFilterStatic bmpFilterDynamic

bmpFilterStatic: bmpBlackWhite.o
	mpicc bmpBlackWhite.o bmpFilterStatic.c -o bmpFilterStatic -lm
	
bmpFilterDynamic: bmpBlackWhite.o
	mpicc bmpBlackWhite.o bmpFilterDynamic.c -o bmpFilterDynamic -lm

bmpBlackWhite.o: bmpBlackWhite.c
	$(CC) -c bmpBlackWhite.c
	
clean:
	rm -f bmpBlackWhite.o bmpFilterStatic
