#include "game.h"


tPlayer getNextPlayer (tPlayer currentPlayer){

	tPlayer next;

		if (currentPlayer == player1)
			next = player2;
		else
			next = player1;

	return next;
}

void initDeck (tDeck *deck){

	int i;

		// Set number of cards
		deck->numCards = DECK_SIZE;

		for (i=0; i<DECK_SIZE; i++){
			deck->cards[i] = i;
		}
}

void clearDeck (tDeck *deck){

	int i;

		// Set number of cards
		deck->numCards = 0;

		for (i=0; i<DECK_SIZE; i++){
			deck->cards[i] = UNSET_CARD;
		}
}

void printSession (tSession *session){

		printf ("\n ------ Session state ------\n");

		// Player 1
		printf ("%s [bet:%d; %d chips] Deck:", session->player1Name, session->player1Bet, session->player1Stack);
		printDeck (&(session->player1Deck));

		// Player 2
		printf ("%s [bet:%d; %d chips] Deck:", session->player2Name, session->player2Bet, session->player2Stack);
		printDeck (&(session->player2Deck));

		// Current game deck
		if (DEBUG_PRING_GAMEDECK){
			printf ("Game deck: ");
			printDeck (&(session->gameDeck));
		}
}

void initSession (tSession *session){

	clearDeck (&(session->player1Deck));
	session->player1Bet = 0;
	session->player1Stack = INITIAL_STACK;

	clearDeck (&(session->player2Deck));
	session->player2Bet = 0;
	session->player2Stack = INITIAL_STACK;

	initDeck (&(session->gameDeck));
}

unsigned int getRandomCard (tDeck* deck){

	unsigned int card, cardIndex, i;

		// Get a random card
		cardIndex = rand() % deck->numCards;
		card = deck->cards[cardIndex];

		// Remove the gap
		for (i=cardIndex; i<deck->numCards-1; i++)
			deck->cards[i] = deck->cards[i+1];

		// Update the number of cards in the deck
		deck->numCards--;
		deck->cards[deck->numCards] = UNSET_CARD;


	return card;
}

unsigned int calculatePoints (tDeck *deck){

	unsigned int points, i;

		// Init...
		points = 0;

		for (i=0; i<deck->numCards; i++){

			if (deck->cards[i] % SUIT_SIZE < 9)
				points += (deck->cards[i] % SUIT_SIZE) + 1;
			else
				points += FIGURE_VALUE;
		}

	return points;
}

void updateStacks (tSession *session){

	unsigned int points1, points2, bet;

		// Check game!
		points1 = calculatePoints (&(session->player1Deck));
		points2 = calculatePoints (&(session->player2Deck));
		bet = min (session->player1Bet, session->player2Bet);

		// Draw game!
		if (((points1 > GOAL_GAME) && (points2 > GOAL_GAME)) || (points1 == points2)){
			session->player1Stack += session->player1Bet;
			session->player2Stack += session->player2Bet;
		}

		// Player 1 is out
		else if (points1 > GOAL_GAME){
			session->player1Stack += session->player1Bet - bet;
			session->player2Stack += bet;
			session->player2Stack += session->player2Bet;
		}

		// Player 2 is out
		else if (points2 > GOAL_GAME){
			session->player1Stack += bet;
			session->player1Stack += session->player1Bet;
			session->player2Stack += session->player2Bet - bet;
		}

		// Player 1 wins
		else if ((GOAL_GAME-points1) < (GOAL_GAME-points2)){
			session->player1Stack += bet;
			session->player1Stack += session->player1Bet;
			session->player2Stack += session->player2Bet - bet;
		}

		// Player 2 wins
		else{
			session->player1Stack += session->player1Bet - bet;
			session->player2Stack += bet;
			session->player2Stack += session->player2Bet;
		}
}

unsigned int min (unsigned int a, unsigned int b){
	return (a<b?a:b);
}
