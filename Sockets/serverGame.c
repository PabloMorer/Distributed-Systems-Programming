#include "serverGame.h"
#include <pthread.h>


const int MAX_CONNECTIONS = 10;

void showError(const char *msg){
	perror(msg);
	exit(0);
}




void mandarApostar( tThreadArgs *sockets, tPlayer player, tSession *session){	

	int socketId;
	int code, bet;
	int messageLength;
	int maxStacks;
	if(player == player1){
		socketId = sockets->socketPlayer1;
		maxStacks = session->player1Stack;
	} 
	else{
		socketId = sockets->socketPlayer2;
		maxStacks = session->player2Stack;
	}

	code = TURN_BET;		//poner cod TURN_BET + stack a curr Player

	messageLength = send(socketId, &code, sizeof(int), 0);	//enviar codigo TURN_BET
	if(messageLength < 0){
		showError("error al enviar codigo al cliente");
	}
	messageLength = send(socketId, &maxStacks, sizeof(int), 0);	//enviar stack
	if(messageLength < 0){
		showError("error al enviar stacks al cliente");
	}

	messageLength = recv(socketId, &bet, sizeof(int), 0);	//RECIBIR BET
	if(messageLength < 0){
		showError("error al enviar bet al cliente");
	}

		while(bet < 0 || bet > maxStacks || bet > MAX_BET) {			//CASO BET NO VALIDO -> REPETIR

			code = TURN_BET;

			messageLength = send(socketId, &code, sizeof(int), 0);	//enviar codigo TURN_BET
			if(messageLength < 0){
				showError("error al enviar codigo al cliente");
			}
				
			messageLength = recv(socketId, &bet, sizeof(int), 0);	//RECIBIR BET
			if(messageLength < 0){
				showError("error al recibir bet del cliente");
			}
				
		}
	
	if(player == player1){
		session->player1Bet = bet;
		session->player1Stack -= bet;
	}else{
		session->player2Bet = bet;
		session->player2Stack -= bet;
	}
	
	


	
	code = TURN_BET_OK;
	
	messageLength = send(socketId, &code, sizeof(int), 0);	//enviar codigo TURN_BET_OK
	if(messageLength < 0){
		showError("error al enviar codigo al cliente");
	}


}

	
void mandarPlayDeckyPuntos(tThreadArgs *sockets, tPlayer currentPlayer, tSession *session){
	
	int socketIdJuega;
	int socketIdNoJuega;
	int code1;
	int code2;
	int msgLength;
	tDeck* deck;
	int puntos;
	int messageLength;
	int socketPlayer1 = ((tThreadArgs *) sockets)->socketPlayer1;
	int socketPlayer2 = ((tThreadArgs *) sockets)->socketPlayer2;
	//mandamos el deck y los puntos a los 2 jugadores


	if(currentPlayer == player1){
		socketIdJuega = socketPlayer1;
		socketIdNoJuega= socketPlayer2;
		deck = &session->player1Deck;
	}
	else{
		socketIdJuega = socketPlayer2;
		socketIdNoJuega = socketPlayer1;
		deck = &session->player2Deck;


	}
	

	puntos = calculatePoints(deck);

	if(puntos > 21){
		printf("MAS PUNTOS QUE LOS PERMITIDOS\n");
		code1 = TURN_PLAY_OUT;
		code2 = TURN_PLAY_WAIT;
		
	}else{
		code1 = TURN_PLAY;
		code2 = TURN_PLAY_WAIT;
	}

	
	
	//enviamos TURN_PLAY o TURN_PLAY_OUT al que realiza las acciones

	messageLength = send(socketIdJuega, &code1, sizeof(int), 0);
	if(messageLength < 0){
		showError("error al enviar codigo al cliente");
	}	
	
	//enviamos TURN_PLAY_WAIT  o TURN_PLAY_OUT al que no realiza las acciones

	messageLength = send(socketIdNoJuega, &code2, sizeof(int), 0);	
	if(messageLength < 0){
		showError("error al enviar codigo al cliente");
	}
	
	//enviamos la informacion del deck a los 2 jugadores
	messageLength = send(socketPlayer1, deck, sizeof(tDeck), 0);
	if(messageLength < 0){
		showError("error al enviar deck al cliente");
	}
	messageLength = send(socketPlayer2, deck, sizeof(tDeck), 0);
	if(messageLength < 0){
		showError("error al enviar deck al cliente");
	}

	//enviamos la informacion de los puntos a los 2 jugadores
	messageLength = send(socketPlayer1, &puntos, sizeof(int), 0);
	if(messageLength < 0){
		showError("error al enviar puntos al cliente");
	}
	messageLength = send(socketPlayer2, &puntos, sizeof(int), 0);
	if(messageLength < 0){
		showError("error al enviar puntos al cliente");
	}

}

void giveRandomCardToPlayer( tPlayer player, tSession *session){

	unsigned int card;
	tDeck *dest;
	card = getRandomCard(&session->gameDeck);

	if(player == player1) {
		dest = &session->player1Deck;
	}	
	else{
		dest = &session->player2Deck;
	}

	
	dest->cards[dest->numCards] = card;
	dest->numCards++;


}

int getStacks(tPlayer currentPlayer, tSession *session){
	int stacks;

	if(currentPlayer == player1){	
		stacks = session->player1Stack;
	} 
	else{
		stacks = session->player2Stack;
	}	

return stacks;

}

void limparDecks(tSession *session){
	tDeck* deck1;
	deck1 = &session->player1Deck;

	tDeck* deck2;
	deck2 = &session->player2Deck;
	
	clearDeck(deck1);
	clearDeck(deck2);



}

void *threadProcessing(void *threadArgs){

	tSession session;				/** Session of this game */
	int socketPlayer1;				/** Socket descriptor for player 1 */
	int socketPlayer2;				/** Socket descriptor for player 2 */
	tPlayer currentPlayer;			/** Current player */
	int endOfGame;					/** Flag to control the end of the game*/
	unsigned int card;				/** Current card */
	unsigned int code, codeRival;	/** Codes for the active player and the rival */

	char player1name[STRING_LENGTH]; /** nombre del primer jugador*/
	char player2name[STRING_LENGTH]; /** nombre del segundo jugador*/
	unsigned int messageLength;
	int socketId;
	int puntos;
	int socketId2;
	int stacks1;
	int stacks2;

		// Get sockets for players
		socketPlayer1 = ((tThreadArgs *) threadArgs)->socketPlayer1;
		socketPlayer2 = ((tThreadArgs *) threadArgs)->socketPlayer2;

		// Receive player 1 info
		
		
		memset(player1name, 0,STRING_LENGTH);
		messageLength = recv(socketPlayer1, player1name, STRING_LENGTH-1, 0);
		if(messageLength < 0){
			showError("error al recibir nombre del jugador1");
		}
		printf("Nombre del jugador 1 recibido: %s \n", player1name);
		
		// Receive player 2 info
	
		memset(player2name, 0,STRING_LENGTH);
		messageLength = recv(socketPlayer2, player2name, STRING_LENGTH-1, 0);
		if(messageLength < 0){
			showError("error al recibir nombre 2 del jugador2");
		}
		printf("Nombre del jugador 1 recibido: %s \n", player2name);

		// Init...
		endOfGame = FALSE;
		initSession(&session);
	
		currentPlayer = player1;

		while (!endOfGame){

				
			//repartimos las 2 cartas iniciales
			for(int i = 0; i < 2; i++){
				giveRandomCardToPlayer(player1, &session);
				giveRandomCardToPlayer(player2, &session);
			}
		
			//mandamos apostar primero al jugardor que le toca jugar
			if(currentPlayer == player1){
				mandarApostar(threadArgs, player1, &session);
				mandarApostar(threadArgs, player2, &session);
			}else{
				mandarApostar(threadArgs, player2, &session);
				mandarApostar(threadArgs, player1, &session);
			}

	
			int i = 0;
			
			while(  i < 2 ){
				if(currentPlayer == player1){	
					socketId = socketPlayer1;
					socketId2 = socketPlayer2;
				} 
				else{
					socketId = socketPlayer2;
					socketId2 = socketPlayer1;
				}

				//Comienza la jugada por parte de los jugadores
					mandarPlayDeckyPuntos(threadArgs,currentPlayer, &session);
				//recibimos respuesta de la jugada del currentPlayer

					messageLength = recv(socketId, &code, sizeof(int), 0);
				
					if(messageLength < 0){
						showError("error al recibir codigo del cliente");
					}
					while(code == TURN_PLAY_HIT){ // si decide seguir jugando	
						giveRandomCardToPlayer(currentPlayer, &session);
						//enviamos play, deck y puntos
						mandarPlayDeckyPuntos(threadArgs,currentPlayer, &session);
						printSession(&session);
						messageLength = recv(socketId, &code, sizeof(int), 0);
						if(messageLength < 0){
							showError("error al recibir codigo del cliente");
						}
					}

					if(code == TURN_PLAY_STAND){
						code = TURN_PLAY_RIVAL_DONE;
						messageLength = send(socketId2, &code, sizeof(int), 0);
						if(messageLength < 0){
							showError("error al enviar codigo al cliente");
						}
					}

					currentPlayer = getNextPlayer(currentPlayer); 
					i++;
					
			}
			
					updateStacks(&session);
					
					//getStacks
					
					stacks1 = getStacks(player1, &session);
					stacks2 = getStacks(player2, &session);


					if(stacks1 <= 0){		
						code = TURN_GAME_LOSE;
						messageLength = send(socketPlayer1, &code, sizeof(int), 0);
						if(messageLength < 0){
							showError("error al enviar codigo al cliente");
						}
						code = TURN_GAME_WIN;
						messageLength = send(socketPlayer2, &code, sizeof(int), 0);
						if(messageLength < 0){
							showError("error al enviar codigo al cliente");
						}
						endOfGame = TRUE;
					}else if(stacks2 <= 0){
						code = TURN_GAME_LOSE;
						messageLength = send(socketPlayer2, &code, sizeof(int), 0);
						if(messageLength < 0){
							showError("error al enviar codigo al cliente");
						}
						code = TURN_GAME_WIN;
						messageLength = send(socketPlayer1, &code, sizeof(int), 0);
						if(messageLength < 0){
							showError("error al enviar codigo al cliente");
						}
						endOfGame = TRUE;
					}

			currentPlayer = getNextPlayer(currentPlayer);		
			
			limparDecks(&session);
		}		

		
		// Close sockets
		close (socketPlayer1);
		close (socketPlayer2);
		free(threadArgs);

	return (NULL) ;
}


int createBindListenSocket (unsigned short port){

	int socketId;
	struct sockaddr_in echoServAddr;

		if ((socketId = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
			showError("Error while creating a socket") ;

		// Set server address
		memset(&echoServAddr, 0, sizeof(echoServAddr));
		echoServAddr.sin_family = AF_INET;
		echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		echoServAddr.sin_port = htons(port);

		// Bind
		if (bind(socketId, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
			showError ("Error while binding");

		if (listen(socketId, MAX_CONNECTIONS) < 0)
			showError("Error while listening") ;

	return socketId;
}

int acceptConnection (int socketServer){

	int clientSocket;
	struct sockaddr_in clientAddress;
	unsigned int clientAddressLength;

		// Get length of client address
		clientAddressLength = sizeof(clientAddress);

		// Accept
		if ((clientSocket = accept(socketServer, (struct sockaddr *) &clientAddress, &clientAddressLength)) < 0)
			showError("Error while accepting connection");

		printf("Connection established with client: %s\n", inet_ntoa(clientAddress.sin_addr));

	return clientSocket;
}


int main(int argc, char *argv[]){

	int serverSocket;						/** Socket descriptor */
	struct sockaddr_in serverAddress;	/** Server address structure */
	unsigned int port;					/** Listening port */
	struct sockaddr_in player1Address;	/** Client address structure for player 1 */
	struct sockaddr_in player2Address;	/** Client address structure for player 2 */
	int socketPlayer1;					/** Socket descriptor for player 1 */
	int socketPlayer2;					/** Socket descriptor for player 2 */
	unsigned int clientLength;			/** Length of client structure */
	tThreadArgs *threadArgs; 			/** Thread parameters */
	pthread_t threadID;					/** Thread ID */



	srand(time(0));

		// Check arguments
		if (argc != 2) {
			fprintf(stderr,"ERROR wrong number of arguments\n");
			fprintf(stderr,"Usage:\n$>%s port\n", argv[0]);
			exit(1);
		}

		serverSocket = createBindListenSocket (atoi(argv[1]));

		clientLength = sizeof(player1Address);	
		

		// Infinite loop
		while (1){

			// Establish connection with a client
			socketPlayer1 = acceptConnection(serverSocket);		//does accept and stuff inside

			socketPlayer2 = acceptConnection(serverSocket);

			// Allocate memory
			if ((threadArgs = malloc(sizeof(tThreadArgs))) == NULL )
				showError("Error while allocating memory");

			// Set socket to the thread's parameter structure
			threadArgs->socketPlayer1 = socketPlayer1;
			threadArgs->socketPlayer2 = socketPlayer2;

			// Create a new thread
			if (pthread_create(&threadID, NULL, threadProcessing,  threadArgs) != 0)	// launch game
				showError("pthread_create failed");

			
			
		}

	
		
		

			
		
}
