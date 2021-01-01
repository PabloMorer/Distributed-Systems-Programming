#include "utils.h"

char suitToChar (unsigned int number){

	char suit;

		if ((number/SUIT_SIZE) == 0)
			suit = 'c';
		else if ((number/SUIT_SIZE) == 1)
			suit = 's';
		else if ((number/SUIT_SIZE) == 2)
			suit = 'd';
		else
			suit = 'h';

	return suit;
}

void cardNumberToChar (unsigned int number, tCardNumber cardNumber){

	// init
	cardNumber[1] = 0;

	if ((number%SUIT_SIZE) == 0)
		cardNumber[0] = 'A';
	else if ((number%SUIT_SIZE) == 1)
		cardNumber[0] = '2';
	else if ((number%SUIT_SIZE) == 2)
		cardNumber[0] = '3';
	else if ((number%SUIT_SIZE) == 3)
		cardNumber[0] = '4';
	else if ((number%SUIT_SIZE) == 4)
		cardNumber[0] = '5';
	else if ((number%SUIT_SIZE) == 5)
		cardNumber[0] = '6';
	else if ((number%SUIT_SIZE) == 6)
		cardNumber[0] = '7';
	else if ((number%SUIT_SIZE) == 7)
		cardNumber[0] = '8';
	else if ((number%SUIT_SIZE) == 8)
		cardNumber[0] = '9';
	else if ((number%SUIT_SIZE) == 9){
		cardNumber[0] = '1';
		cardNumber[1] = '0';
	}
	else if ((number%SUIT_SIZE) == 10)
		cardNumber[0] = 'J';
	else if ((number%SUIT_SIZE) == 11)
		cardNumber[0] = 'Q';
	else if ((number%SUIT_SIZE) == 12)
		cardNumber[0] = 'K';
}

void printDeck (tDeck* deck){

	tCardNumber cardNumber;
	char suitChar;

		// Print info for player 1
		printf ("%d cards -> ", deck->numCards);

		for (int i=0; i<deck->numCards; i++){

			// Get the suit of the given card
			suitChar = suitToChar (deck->cards[i]);
			cardNumberToChar (deck->cards[i], cardNumber);

			if (cardNumber[1] == '0')
				printf ("%c%c%c ", cardNumber[0], cardNumber[1], suitChar);
			else
				printf("%c%c ", cardNumber[0], suitChar);
		}
		printf("\n");
}
