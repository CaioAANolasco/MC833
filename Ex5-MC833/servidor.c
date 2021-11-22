
#include <pthread.h>
#include "strmap.h"
#include "wrappers.h"

void * server_Comm(void * parm);       /* Thread function to handle communication with clients */

void getPlayers(const char *key, const char *value, void *obj);

StrMap *players; 

int size =  0; 	/* Sum of users currently joined as players */

pthread_mutex_t  mut; /* Mutex to prevent race conditions. */

#define PROTOPORT         27428        /* default protocol port number */
#define QLEN              10             /* size of request queue        */
#define MAXRCVLEN 200
#define STRLEN 200
#define LISTENQ 10

int main ()
{
	
	players = sm_new(100);	// Hash table para armazenar [nome, IP]
    struct   sockaddr_in serveraddr;   // Informações do servidor  
    struct   sockaddr_in cad;    //Informações do cliente
    int      listenfd, connectfd;              
    int      alen;                
    pthread_t  tid;             

    pthread_mutex_init(&mut, NULL);

    memset((char  *)&serveraddr,0,sizeof(serveraddr)); 

    serveraddr.sin_family = AF_INET;        
    serveraddr.sin_addr.s_addr = INADDR_ANY;  
	serveraddr.sin_port = htons(33333);

    /* Create a socket */
    listenfd = Socket (PF_INET, SOCK_STREAM, 0);

    /* Bind a local address to the socket */
    Bind(listenfd, (struct sockaddr *)&serveraddr, sizeof (serveraddr));
     
    Listen(listenfd, LISTENQ);

    alen = sizeof(cad);

    fprintf(stderr, "Servidor iniciado.\n");
    while (1) {         
        connectfd = Accept(listenfd, (struct sockaddr *)&cad, (unsigned int*) &alen); //Cliente conectou; iniciar nova thread para comunicação
		pthread_create(&tid, NULL, server_Comm, (void *) (intptr_t) connectfd );
    }
     
    Close(listenfd); /* Close socket */     
    sm_delete(players); /* Delete the hasmap */
}

// Função para comunicação com clientes em outra thread
void * server_Comm(void * parm)
{
	int tsd, len;
	tsd = (int) (intptr_t) parm;
	
	//Endereço de IP do cliente para armazenar em hash table
	char ip[INET_ADDRSTRLEN];
	struct sockaddr_in peeraddr;
	socklen_t peeraddrlen = sizeof(peeraddr);
	getpeername(tsd, (struct sockaddr *)&peeraddr, &peeraddrlen);
	inet_ntop(AF_INET, &(peeraddr.sin_addr), ip, INET_ADDRSTRLEN);
	
	char buf[MAXRCVLEN+1];      
	char name[STRLEN+1]; 
	
	//Enquanto cliente estiver conectado
	while((len = Recv(tsd, buf, MAXRCVLEN, 0)))
    {
		buf[len] = '\0';		
		char arg1[STRLEN], arg2[STRLEN];
		sscanf(buf, "%s %s", arg1, arg2);
		
		// Comando para conexão
		if(strcmp(arg1, "conectar") == 0 && arg2 != NULL)
		{
			pthread_mutex_lock(&mut);
			if(sm_exists(players, arg2) == 0)
			{
				sm_put(players, arg2, ip);
				size++;
				strcpy(name, arg2);
				sprintf(buf, "Jogador %s conectado\n", arg2);
				printf("Jogador %s conectado\n", arg2);
			}
			else
			{
				sprintf(buf, "Jogador %s já está na lista\n", arg2);
			}
			pthread_mutex_unlock(&mut);
			Send(tsd, buf, strlen(buf), 0);
		}
		
		//Comando para convite
		else if (strcmp(arg1, "convidar") == 0 && arg2 != NULL)
		{	
			pthread_mutex_lock(&mut);
			if((size > 0) && (sm_exists(players, arg2) == 1))
			{
				sm_get(players, arg2, buf, sizeof(buf));
			}
			else
			{
				sprintf(buf, "Jogador %s não encontrado\n", arg2);
			}
			pthread_mutex_unlock(&mut);
			//Envia endereço de IP
			Send(tsd, buf, strlen(buf), 0);
		}
		
		//Comando para listar
		else if (strcmp(arg1, "listar") == 0)
		{
			pthread_mutex_lock(&mut);
			if(size > 0)
			{
				sprintf(buf, "Jogadores: ");
				sm_enum(players, getPlayers, buf);
			}
			else
			{
				sprintf(buf, "Não há jogadores\n");
			}
			pthread_mutex_unlock(&mut);
			Send(tsd, buf, strlen(buf), 0);
		}
    }
    
    //Remoção do cliente da lista
    pthread_mutex_lock(&mut);
	if(name != NULL && strlen(name) > 1)
	{
		sm_get(players, name, buf, sizeof(buf));
		if (strcmp(buf, ip) == 0)
		{			
			sm_remove(players, name);
			size--;
			printf("Jogador %s removido da lista\n", name);
			name[0] = '\0';
		}
	}
	pthread_mutex_unlock(&mut);
	Close(tsd);
	pthread_exit(0);
}    

//Função auxiliar para listar jogadores na hash table
void getPlayers(const char *key, const char *value, void *obj)
{
	if (value != NULL)
	{
		strcat(obj, key);
		strcat(obj, ", ");
	}
}
