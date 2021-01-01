#include "utils.h"

/** Debug mode? */
#define DEBUG_CLIENT 0

/** Action taken by the player to stand */
#define PLAYER_STAND 0

/** Action taken by the player to hit a card */
#define PLAYER_HIT_CARD 1


/**
 * Function that shows an error message.
 *
 * @param msg Error message
 */
void showError(const char *msg);

/**
 * Prints the received code.
 *
 * @param code Received code.
 */
void showReceivedCode (unsigned int code);

/**
 * Reads a bet entered by the player.
 *
 * @return A number that represents the bet for the current play.
 */
unsigned int readBet ();

/**
 * Reads the action taken by the player (stand or hit).
 *
 * @return A number that represents the action taken by the player.
 */
unsigned int readOption ();


