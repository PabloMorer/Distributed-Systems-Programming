#include "game.h"

/** Debug server? */
#define SERVER_DEBUG 1

/** Sockets of a game used by a thread in the server */
typedef struct threadArgs{
	int socketPlayer1;
	int socketPlayer2;
} tThreadArgs;

/**
 * Function that shows an error message.
 *
 * @param msg Error message.
 */
void showError(const char *msg);


/**
 * Function executed by each thread.
 *
 * @param threadArgs Argument that contains the socket of the players.
 */
void *threadProcessing(void *threadArgs);
