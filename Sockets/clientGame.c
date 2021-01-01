#include "clientGame.h"

void showError(const char *msg){
	perror(msg);
	exit(0);
}

void showReceivedCode (unsigned int code){

	tString string;

		if (DEBUG_CLIENT){

			// Reset
			memset (string, 0, STRING_LENGTH);

			switch(code){

				case TURN_BET:
					strcpy (string, "TURN_BET");
					break;

				case TURN_BET_OK:
					strcpy (string, "TURN_BET_OK");
					break;

				case TURN_PLAY:
					strcpy (string, "TURN_PLAY");
					break;

				case TURN_PLAY_HIT:
					strcpy (string, "TURN_PLAY_HIT");
					break;

				case TURN_PLAY_STAND:
					strcpy (string, "TURN_PLAY_STAND");
					break;

				case TURN_PLAY_OUT:
					strcpy (string, "TURN_PLAY_OUT");
					break;

				case TURN_PLAY_WAIT:
					strcpy (string, "TURN_PLAY_WAIT");
					break;

				case TURN_PLAY_RIVAL_DONE:
					strcpy (string, "TURN_PLAY_RIVAL_DONE");
					break;

				case TURN_GAME_WIN:
					strcpy (string, "TURN_GAME_WIN");
					break;

				case TURN_GAME_LOSE:
					strcpy (string, "TURN_GAME_LOSE");
					break;

				default:
					strcpy (string, "UNKNOWN CODE");
					break;
			}

			printf ("Received:%s\n", string);
		}
}

unsigned int readBet (){

	int i, isValid, bet;
	tString enteredMove;

		// Init...
		bet = 0;

		// While player does not enter a correct bet...
		do{

			// Init...
			bzero (enteredMove, STRING_LENGTH);
			isValid = TRUE;

			// Show input message
			printf ("Enter a bet:");

			// Read move
			fgets(enteredMove, STRING_LENGTH-1, stdin);

			// Remove new-line char
			enteredMove[strlen(enteredMove)-1] = 0;

			// Check if each character is a digit
			for (i=0; i<strlen(enteredMove) && isValid; i++){

				if (!isdigit(enteredMove[i]))
					isValid = FALSE;
			}

			// Entered move is not a number
			if (!isValid){
				printf ("Entered bet is not correct. It must be a number greater than 0\n");
			}

			// Entered move is a number...
			else{

				// Conver entered text to an int
				bet = atoi (enteredMove);
			}

		}while (!isValid);

		printf ("\n");

	return ((unsigned int) bet);
}

unsigned int readOption (){

	int i, isValid, option;
	tString enteredMove;

		// Init...
		option = 0;

		// While player does not enter a correct bet...
		do{

			// Init...
			bzero (enteredMove, STRING_LENGTH);
			isValid = TRUE;

			// Show input message
			printf ("Press %d to hit a card and %d to stand:", PLAYER_HIT_CARD, PLAYER_STAND);

			// Read move
			fgets(enteredMove, STRING_LENGTH-1, stdin);

			// Remove new-line char
			enteredMove[strlen(enteredMove)-1] = 0;

			// Check if each character is a digit
			for (i=0; i<strlen(enteredMove) && isValid; i++){

				if (!isdigit(enteredMove[i]))
					isValid = FALSE;
			}

			// Entered move is not a number
			if (!isValid){
				printf ("Wrong option!\n");
			}

			// Entered move is a number...
			else{

				// Conver entered text to an int
				option = atoi (enteredMove);

				if ((option != PLAYER_HIT_CARD) && (option != PLAYER_STAND)){
					printf ("Wrong option!\n");
					isValid = FALSE;
				}
			}

		}while (!isValid);

		printf ("\n");

	return ((unsigned int) option);
}

void realizarApuesta(int socketfd, int stack){
	int code;
	unsigned int bet;
	unsigned int msgLength;
	printf("La apuesta debe ser >0 y <=%d \n", stack);
		do{
		 	bet = readBet(); //recogemos apuesta
			msgLength = send(socketfd, &bet, sizeof(int),0); //enviamos la apuesta
			if(msgLength < 0){
				showError("ERROR al recibir apuesta");
			}

			msgLength = recv(socketfd, &code, sizeof(int), 0); //recibimos codigo
			if(msgLength < 0){
				showError("ERROR al recibir apuesta");
			}
			
		}while(code != TURN_BET_OK);
}




void turnoJugar(int socketfd, int puntos){
int code;
int opcion;
int msgLength;
	
	opcion = readOption(); //ESCOGEMOS QUE SI PLANTARNOS O PEDIR UNA CARTA
		if(opcion == PLAYER_HIT_CARD){
			code = TURN_PLAY_HIT;
		}else{
			code = TURN_PLAY_STAND;
		}
					
		msgLength = send(socketfd, &code ,sizeof(int),0); //ENVIAMOS LA OPCION
			if(msgLength < 0){
				showError("error al enviar codigo del servidor");
			}
}


/* Turno de realizar la jugada*/
int jugar(int socketfd, int code){
	int msgLength;
	int puntos;
	tDeck* deck;
		while(code == TURN_PLAY ){

					msgLength = recv(socketfd, deck, sizeof(tDeck), 0);	//RECIBIR DECK
					if(msgLength < 0){
						showError("error al recibir deck del servidor");
					}
					msgLength = recv(socketfd, &puntos, sizeof(int), 0);	//RECIBIR PUNTOS DE JUGADA
					if(msgLength < 0){
						showError("error al recibir puntos del servidor");
					}

					printf("Tienes %d puntos\n", puntos);
					printDeck(deck);

					turnoJugar(socketfd, puntos); //ESCOGER JUGADA Y MANDARLA AL SERVIDOR
					
					msgLength = recv(socketfd, &code, sizeof(int), 0); 
					if(msgLength < 0){
						showError("error al recibir codigo del servidor");
					}	
					showReceivedCode(code);
		}



		if(code == TURN_PLAY_OUT){
					msgLength = recv(socketfd, deck, sizeof(tDeck), 0);	//RECIBIR DECK
					if(msgLength < 0){
						showError("error al recibir deck del servidor");
					}
					msgLength = recv(socketfd, &puntos, sizeof(int), 0);	//RECIBIR PUNTOS DE JUGADA
					if(msgLength < 0){
						showError("error al recibir puntos del servidor");
					}
					code = TURN_PLAY_STAND;
					printf("Tienes %d puntos\n", puntos);
					printDeck(deck);
					msgLength = send(socketfd, &code ,sizeof(int),0); //ENVIAMOS LA OPCION
					if(msgLength < 0){
						showError("error al enviar codigo del servidor");
					}
					printf("Te has pasado de 21 puntos...\n");

					msgLength = recv(socketfd, &code, sizeof(int), 0); //recibo turn_play_wait
					if(msgLength < 0){
						showError("error al recibir codigo del servidor");
					}
			
		}
		
		showReceivedCode(code);
		return code;
		
}


int esperar(int socketfd, int code){
	int msgLength;
	int puntos;
	tDeck* deck;

	while(code == TURN_PLAY_WAIT){
 
		msgLength = recv(socketfd, deck, sizeof(tDeck), 0);	//RECIBIR DECK
			if(msgLength < 0){
				showError("error al recibir deck del servidor");
			}
		msgLength = recv(socketfd, &puntos, sizeof(int), 0);	//RECIBIR PUNTOS DE JUGADA
					
			if(msgLength < 0){
				showError("error al recibir puntos del servidor");
			}
				
		printf("El rival tiene %d puntos\n", puntos);
		printDeck(deck);

		msgLength = recv(socketfd, &code, sizeof(int), 0);
			if(msgLength < 0){
				showError("error al recibir codigo del servidor");
			}

		}


	if(code == TURN_PLAY_RIVAL_DONE){
				printf("El turno del rival ha finalizado...\n");
				msgLength = recv(socketfd, &code, sizeof(int), 0);
					if(msgLength < 0){
						showError("error al recibir codigo del servidor");
					}
	}

	return code;
}


						



		






int main(int argc, char *argv[]){

	int socketfd;						/** Socket descriptor */
	unsigned int port;					/** Port number (server) */
	struct sockaddr_in server_address;	/** Server address structure */
	char* serverIP;						/** Server IP */
	unsigned int endOfGame;				/** Flag to control the end of the game */
	tString playerName;					/** Name of the player */
	unsigned int code;					/** Code */
	unsigned int msgLength;

	int stack;
	tDeck* deck;
	int puntos;
	int opcion;
	int i;

		// Check arguments!
		if (argc != 3){
			fprintf(stderr,"ERROR wrong number of arguments\n");
			fprintf(stderr,"Usage:\n$>%s serverIP port\n", argv[0]);
			exit(0);
		}

		// Get the server address
		serverIP = argv[1];

		// Get the port
		port = atoi(argv[2]);

		// Create socket
		
		socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //creacion de socket
		if(socketfd < 0){
			showError("ERROR mientras se creaba el socket");
		}

		// Fill server address structure
		
		memset(&server_address, 0, sizeof(server_address)); //creamos la estructura del servidor
		server_address.sin_family = AF_INET;
		server_address.sin_addr.s_addr = inet_addr(serverIP); //servidor de IP
		server_address.sin_port = htons(port); //puerto

		// Connect with server
		
		if(connect(socketfd, (struct sockaddr *) &server_address, //nos conectamos al servidor
		sizeof(server_address)) < 0){
			showError("ERROR al conectarse con el servidor");
		}
		
		printf ("Connection established with server!\n");


		// Init player's name
		do{
			memset(playerName, 0, STRING_LENGTH);
			printf ("Enter player name:");
			fgets(playerName, STRING_LENGTH-1, stdin);

			// Remove '\n'
			playerName[strlen(playerName)-1] = 0;

		}while (strlen(playerName) <= 2);

		

		// Init
		//enviamos el nombre al servidor

		msgLength = send(socketfd, playerName,strlen(playerName),0);
			if(msgLength < 0){
				showError("error al enviar codigo del servidor");
			}
		
		if(msgLength < 0){ 
			showError("ERROR al escribir el nombre"); 
		}

		// Game starts
		printf ("Game starts!\n\n");
		endOfGame = FALSE;
		// While game continues...
		msgLength = recv(socketfd, &code, sizeof(int), 0); //RECIBIR PRIMER CODIGO
			if(msgLength < 0){
				showError("error al recibir codigo del servidor");
			}
		while (!endOfGame){
		

			if(code == TURN_GAME_WIN){	
				printf("HAS GANADO LA PARTIDA\n");
				endOfGame = TRUE;
			}else if( code == TURN_GAME_LOSE){
				printf("HAS PERDIDO LA PARTIDA\n");
				endOfGame = TRUE;
			}else{
				msgLength = recv(socketfd, &stack, sizeof(int), 0);	//RECIBIR STACK
					if(msgLength < 0){
						showError("error al recibir stack del servidor");
					}
				if( code == TURN_BET){
					realizarApuesta(socketfd, stack);
				}
				
				msgLength = recv(socketfd, &code, sizeof(int), 0);
					if(msgLength < 0){
						showError("error al recibir codigo del servidor");
					}
				if(code == TURN_PLAY){
					printf("Empiezo jugando\n");
					code = jugar(socketfd, code);
					code = esperar(socketfd,code);	
				}else{
					printf("Empiezo esperando\n");
					code = esperar(socketfd,code);
					code = jugar(socketfd,code);
				}
			
				showReceivedCode(code);

				

			}
	
			

			

			
		}



	// Close socket
	close(socketfd);


return 0;
}
