#include "utils.h"

/** Maximum bet */
#define MAX_BET 5

/** Initial stack for each player */
#define INITIAL_STACK 5

/** Shows the game deck? */
#define DEBUG_PRING_GAMEDECK 1

/** Number of points to win the game */
#define GOAL_GAME 21

/** Value of a figure */
#define FIGURE_VALUE 10

/** Code to represents an empty card (in the deck) */
#define UNSET_CARD 100

/** Players in one session */
typedef enum {player1, player2} tPlayer;

/** Session represents a game between 2 players */
typedef struct{

	// Data for player 1
	tString player1Name;
	tDeck player1Deck;
	unsigned int player1Stack;
	unsigned int player1Bet;

	// Data for player 2
	tString player2Name;
	tDeck player2Deck;
	unsigned int player2Stack;
	unsigned int player2Bet;

	// Deck for the current game
	tDeck gameDeck;
}tSession;


/**
 * Calculates the next player.
 *
 * @param currentPlayer The current player.
 * @return The next player to make a move.
 */
tPlayer getNextPlayer (tPlayer currentPlayer);

/**
 * Inits the game deck with all the cards.
 *
 * @param deck Game deck.
 *
 */
void initDeck (tDeck *deck);

/**
 * Clears a deck (for players)
 *
 * @param deck Player deck.
 */
void clearDeck (tDeck *deck);

/**
 * Prints a session Useful for debugging.
 *
 * @param session Session to be shown.
 */
void printSession (tSession *session);

/**
 * Inits a session.
 *
 * @param session Session to be initialized.
 */
void initSession (tSession *session);

/**
 * Gets a random card of the game deck. The obtained card is removed from the game deck.
 *
 * @param deck Game deck.
 * @return Randomly selected card from the game deck.
 */
unsigned int getRandomCard (tDeck* deck);

/**
 * Calculates the current points of a given deck.
 *
 * @param deck Given deck.
 * @return Points of the deck.
 */
unsigned int calculatePoints (tDeck *deck);

/**
 * Updates the stacks for the current play.
 * The information in the session structure is updated.
 *
 * @session Session containing the information of the game.
 */
void updateStacks (tSession *session);

/**
 * Calculates the minimum of two given numbers.
 *
 * @param a First number.
 * @param b Second number.
 * @return Minimun value of the two given numbers.
 */
unsigned int min (unsigned int a, unsigned int b);
