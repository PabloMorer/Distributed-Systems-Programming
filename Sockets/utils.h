#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <arpa/inet.h>

/** True value */
#define TRUE 1

/** False value */
#define FALSE 0

/** Length for tString */
#define STRING_LENGTH 128

/** Deck's size */
#define DECK_SIZE 52

/** Number of suits in the deck */
#define SUIT_SIZE 13

/** Number of chars to represent the number of a card */
#define NUM_CHAR_CARD 2

/** Bet */
#define TURN_BET 0

/** The bet is correct */
#define TURN_BET_OK 1

/** Play (player's turn) */
#define TURN_PLAY 2

/** Player hits */
#define TURN_PLAY_HIT 3

/** Player stands */
#define TURN_PLAY_STAND 4

/** Player exceeds 21 */
#define TURN_PLAY_OUT 5

/** Player must wait and see the rival's play */
#define TURN_PLAY_WAIT 6

/** Rival is done */
#define TURN_PLAY_RIVAL_DONE 7

/** Player wins */
#define TURN_GAME_WIN 8

/** Player loses */
#define TURN_GAME_LOSE 9

/** Type for names, messages and this kind of variables */
typedef char tString [STRING_LENGTH];

/** 4 different suits */
typedef enum {clubs, spades, diamonds, hearts} tSuit;

/** Number of a card */
typedef char tCardNumber[NUM_CHAR_CARD];

/** Structure that represents a deck */
typedef struct{
	unsigned int cards [DECK_SIZE];
	unsigned int numCards;
}tDeck;


/**
 * Gets the suit of a given card.
 *
 * @param number Given card.
 * @return Suit of the given card.
 */
char suitToChar (unsigned int number);

/**
 * Gets the number of a given card.
 *
 * @param number Given card.
 * @cardNumber Number of the given card in string format.
 */
void cardNumberToChar (unsigned int number, tCardNumber cardNumber);

/**
 * Prints a deck.
 *
 * @param deck Deck to be shown.
 */
void printDeck (tDeck* deck);
