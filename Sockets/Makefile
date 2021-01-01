CC = gcc
CFLAGS = -c -g -Wall

EXECS = clientGame serverGame

all: utils.o game.o $(EXECS)

game.o: game.c
	$(CC) $(CFLAGS) game.c

utils.o: utils.c
	$(CC) $(CFLAGS) utils.c
	
serverGame.o: serverGame.c
	$(CC) $(CFLAGS) serverGame.c

serverGame: game.o utils.o serverGame.o 
	$(CC) game.o utils.o serverGame.o -lpthread -o serverGame

clientGame.o: clientGame.c
	$(CC) $(CFLAGS) clientGame.c
	
clientGame: utils.o clientGame.o 
	$(CC) clientGame.o utils.o -o clientGame
	
clean:
	rm -f  *.o
	rm -f $(EXECS)
