
#include <pthread.h>
#include "wrappers.h"

#define SERVERIP  "127.0.0.1"
 
#define MAXRCVLEN 200
#define STRLEN 200

char name[STRLEN],
opponent[STRLEN];	
pthread_t  tid;   

void playgame(int  socket, char * buffer, int playerID); //Função para jogo da velha

void * server_Comm(void * parm); //Função para comunicação com servidor
void * opponent_Comm(char * ip); //Função para comunicação com oponente

int serversocket, peersocket, mysocket, consocket; // Sockets de conexão
int ingame, turn;


int main()
{
	const char server[30] = SERVERIP;
	
	// Conexão TCP com servidor
	struct sockaddr_in dest;
	serversocket = socket(AF_INET, SOCK_STREAM, 0);	
	memset(&dest, 0, sizeof(dest));           
	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = htonl(inet_network(server)); 
	dest.sin_port = htons(33333);               
	connect(serversocket, (struct sockaddr *)&dest, sizeof(struct sockaddr));
	
	// Comunicação com servidor continuada em outra thread
	pthread_create(&tid, NULL, server_Comm, (void*) &serversocket);   
    
    
    // Escuta por comunicação com outro clientes
	struct sockaddr_in peer;
	struct sockaddr_in serv;
	socklen_t socksize = sizeof(struct sockaddr_in);
	memset(&serv, 0, sizeof(serv)); 
    serv.sin_family = AF_INET;   
    serv.sin_addr.s_addr = htonl(INADDR_ANY); 
    serv.sin_port = htons(44444);
	mysocket = socket(AF_INET, SOCK_STREAM, 0); // Usa conexão *TCP*
	bind(mysocket, (struct sockaddr *)&serv, sizeof(struct sockaddr));
	listen(mysocket, 1); 
	consocket = accept(mysocket, (struct sockaddr *)&peer, &socksize);    
	
	char buffer[MAXRCVLEN];
    int len;
	
	while(consocket)
	{
		pthread_cancel(tid);
		
		// Recebe nome do oponente
		len = recv(consocket, buffer, MAXRCVLEN, 0); 
		buffer[len] = '\0';
		strcpy(opponent, buffer);	
		printf("\n%s convidou para uma partida. (y/n) ", opponent);
		
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer)-1] = '\0';
		if (strcmp(buffer, "y") == 0)
		{
			ingame = 1;
			send(consocket, buffer, strlen(buffer), 0);
		}
		else
		{
			send(consocket, buffer, strlen(buffer), 0);
			printf("\nYou declined...\n");
			ingame = 0; 
		} //Envia resposta para oponente
		
		turn = 0;		
		int playerID = 2;
		while(ingame)
		{
			playgame(consocket, buffer, playerID); 
		}
		
		//Após partida(s), abre thread com servidor novamente e continua a ouvir por outros clientes
		opponent[0] = '\0';
		pthread_create(&tid, NULL, server_Comm, (void*) &serversocket); 
		consocket = accept(mysocket, (struct sockaddr *)&peer, &socksize);  
	}

	// Fechar sockets
	close(mysocket);
	close(consocket);
	close(peersocket);
	close(serversocket);
			
	return EXIT_SUCCESS;    
}

// Função para thread de comunicação com servidor
void * server_Comm(void *obj)
{
	(void)obj;
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);

	char buffer[MAXRCVLEN + 1]; 
	int len;
	
	/* Print use instructions */
	printf("\nEscolha: \n conectar {name}\n listar\n convidar {jogador}\n sair\n\n");

	while(1)
	{
		fgets(buffer, sizeof(buffer), stdin);
		buffer[strlen(buffer)-1] = '\0';		
		char arg1[STRLEN], arg2[STRLEN];
		int n = sscanf(buffer, "%s %s", arg1, arg2);
		
		// Comando para listar
		if(strcmp(buffer, "listar") == 0)
		{
			Send(serversocket, buffer, strlen(buffer), 0);
			len = Recv(serversocket, buffer, MAXRCVLEN, 0);
			buffer[len] = '\0';
			printf("\n%s\n", buffer);
		}
		// Comando para conectar
		else if((strcmp(arg1, "conectar") == 0) && n > 1)
		{
			if(name == NULL || strlen(name) < 1) /* Internal check to see whether user is already joined. */
			{
				Send(serversocket, buffer, strlen(buffer), 0);
				len = Recv(serversocket, buffer, MAXRCVLEN, 0);
				buffer[len] = '\0';
				printf("\n%s\n", buffer);
				strcpy(name, arg2);	/* Store this name */
			}
			else printf("\nJá se conectou ao servidor anteriormente: %s\n", name);
			
		}
		
		// Comando para convite para partida
		else if(strcmp(arg1, "convidar") == 0 && n > 1)
		{
			if(name == NULL || strlen(name) < 1)
			{
				printf("\nNão está conectado ao servidor\n");
			}
			else
			{
				//Obtem IP do oponente pelo servidor e inicia thread de comunicação
				strcpy(opponent, arg2);
				Send(serversocket, buffer, strlen(buffer), 0);
				len = Recv(serversocket, buffer, MAXRCVLEN, 0);
				buffer[len] = '\0';
				pthread_create(&tid, NULL, (void*)opponent_Comm, buffer); 
				pthread_exit(0);
			}
			
		}
		// Comando para saída
		else if(strcmp(buffer, "sair") == 0)
		{
			name[0] = '\0';
			printf("\nSaindo\n");
			Close(mysocket);
			Close(consocket);
			Close(peersocket);
			Close(serversocket);
			exit(1);			
		}
		else printf("\nEscolha: \n conectar {name}\n listar\n convidar {jogador}\n sair\n\n");
	}
}

// Thread para comunicação com outro cliente
void * opponent_Comm(char * ip)
{
	char buffer[MAXRCVLEN + 1]; 
	int len;	

	//Abre conexão com cliente de IP identificado
	struct sockaddr_in dest;
	peersocket = socket(AF_INET, SOCK_STREAM, 0);	
	memset(&dest, 0, sizeof(dest));

	dest.sin_family = AF_INET;
	dest.sin_addr.s_addr = htonl(inet_network(ip)); 
	dest.sin_port = htons(44444); 

	printf("\nConvidando...\n");	
	Connect(peersocket, (struct sockaddr *)&dest, sizeof(struct sockaddr));

	//Envia nome
	Send(peersocket, name, strlen(name), 0);

	//Recebe resposta para convite
	len = Recv(peersocket, buffer, MAXRCVLEN, 0);
	buffer[len] = '\0';
	if (strcmp(buffer, "y") == 0)
	{
		ingame = 1;
		
	}
	else
	{
		ingame = 0;
		printf("\nConvite negado\n");
	} 

	turn = 0;	
	int playerID = 1;
	while(ingame)
	{
		playgame(peersocket, buffer, playerID);
	}
	
	//Depois de partida, inicia novamente comunicação com servidor em thread separada
	opponent[0] = '\0';
	Close(peersocket);
	pthread_create(&tid, NULL, server_Comm, (void*) &serversocket);  
	pthread_exit(0);	
}

//Lógica para jogo da velha, baseada no livro "From Novice to Professional (Horton 2006)".
void playgame(int socket, char * buffer, int playerID)
{
	int len, datasocket = socket;

	printf("\nINICIANDO\n");

	int i = 0;                                   /* Loop counter                         */
	int player = 0;                              /* Player number - 1 or 2               */  
	int go = 0;                                  /* Square selection number for turn     */
	int row = 0;                                 /* Row index for a square               */  
	int column = 0;                              /* Column index for a square            */
	int line = 0;                                /* Row or column index in checking loop */
	int winner = 0;                              /* The winning player                   */
	char board[3][3] = {                         /* The board                            */
					   {'1','2','3'},          /* Initial values are reference numbers */
					   {'4','5','6'},          /* used to select a vacant square for   */
					   {'7','8','9'}           /* a turn.                              */
					 };
				 

	/* The main game loop. The game continues for up to 9 turns */
	/* As long as there is no winner                            */
	for( i = (0 + turn); i<(9 + turn) && winner==0; i++)
	{
		/* Display the board */
		printf("\n\n");
		printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
		printf("---+---+---\n");
		printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
		printf("---+---+---\n");
		printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);
	  
		player = i%2 + 1;                           /* Select player */

		/* Take appropriate actions based on turn. */
		do
		{
			if (player == playerID)
			{
				printf("\n%s, informe número da posição do tabuleiro"
				"para inserir próximo caractere %c: ", name,(player==1)?'X':'O');
				scanf("%d", &go);
				send(datasocket, &go, sizeof(go), 0); /* Send your selection */
			}
			else 
			{
				printf("\nAguardando por %s...\n", opponent);
				len = recv(datasocket, &go, MAXRCVLEN, 0); /* Receive peer's selection */
				printf("%s escolheu %d\n", opponent, go);
			}

			row = --go/3;                                 /* Get row index of square      */
			column = go%3;                                /* Get column index of square   */
		} while(go<0 || go>9 || board[row][column]>'9');

		board[row][column] = (player == 1) ? 'X' : 'O';        /* Insert player symbol   */

		/* Check for a winning line - diagonals first */     
		if((board[0][0] == board[1][1] && board[0][0] == board[2][2]) ||
		 (board[0][2] == board[1][1] && board[0][2] == board[2][0]))
			winner = player;
		else
		/* Check rows and columns for a winning line */
		for(line = 0; line <= 2; line ++)
			if((board[line][0] == board[line][1] && board[line][0] == board[line][2])||
			 (board[0][line] == board[1][line] && board[0][line] == board[2][line]))
				winner = player;

	}
	printf("\n\n");
	printf(" %c | %c | %c\n", board[0][0], board[0][1], board[0][2]);
	printf("---+---+---\n");
	printf(" %c | %c | %c\n", board[1][0], board[1][1], board[1][2]);
	printf("---+---+---\n");
	printf(" %c | %c | %c\n", board[2][0], board[2][1], board[2][2]);

	if(winner == 0)
		printf("\nEmpate.\n");
	else if (winner == playerID)
		printf("\nVocê venceu\n");
	else printf("\nVocê perdeu\n");
	
	if (turn == 0 ) turn++;
	else turn--;

	printf("\nJogar novamente? (y/n) ");
	fgetc(stdin);
	fgets(buffer, sizeof buffer, stdin);
	buffer[strlen(buffer)-1] = '\0';
	printf("\nEsperando por resposta de %s\n", opponent);

	ingame=0;
	if (strcmp(buffer, "y") == 0) ingame = 1;
	Send(datasocket, buffer, strlen(buffer), 0);
	len = Recv(datasocket, buffer, MAXRCVLEN, 0);
	buffer[len] = '\0';
	if (strcmp(buffer, "y") != 0 && ingame==1)
	{
		printf("\n%s recusou revanche\n", opponent);
		ingame = 0;
	}
}