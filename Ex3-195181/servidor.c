#include "wrappers.h"

#define LISTENQ 10
#define MAXDATASIZE 4096

void str_echo(int sockfd, struct sockaddr_in servaddr)
{
   char   buf[MAXDATASIZE];
   time_t ticks;

   socklen_t len = sizeof(servaddr);
   if (getpeername(sockfd, (struct sockaddr *)&servaddr, &len) == -1)
      perror("getpeername");
   else
   {
      char clientIP[16];
      Inet_ntop(AF_INET, &servaddr.sin_addr, clientIP, sizeof(clientIP));
      printf("Client IP address: %s\n", clientIP);
      printf("Client port number: %d\n", ntohs(servaddr.sin_port));
   }

   ticks = time(NULL);
   snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
   Write(sockfd, buf, strlen(buf));
   
} 


void sig_chld() {

   pid_t pid;
   int stat;

   pid = wait(&stat);
   printf("child %d terminated\n", pid );
}

int main (int argc, char **argv) {
   int    listenfd, connfd; //Descritores de socket, para escutar e para quando conexão é aceita
   struct sockaddr_in servaddr; //Struck sockaddr_in para armazenar endereço de dominio e numero de porta
   pid_t childpid;

   if(argc != 2)
   {
      printf("Informe numero da porta\n");
      exit(1);
   }

   listenfd = Socket(AF_INET, SOCK_STREAM, 0); //Inicialização do socket

   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family      = AF_INET;
   servaddr.sin_addr.s_addr = INADDR_ANY; //Adota como endereço de domínio o IP da máquina do servidor
   servaddr.sin_port        = htons(atoi(argv[1]));  //Permite que o sistema atribua um numero de porta temporário

   Bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); //Unir o socket com a porta de numero especificada 

   Listen(listenfd, 9); //Iniciar a escuta por conexões, usando o descritor de socket listenfd criado

   Signal(SIGCHLD, sig_chld);

   for ( ; ; ) {
      sleep(3);
      connfd = Accept(listenfd, (struct sockaddr *) NULL, NULL); //Aceita a conexão, e inicia outro descritor de socket para esta conexão

      if ((childpid = Fork()) == 0)
      {
         Close(listenfd);
         str_echo(connfd, servaddr);  
         exit(0);
      }

      Close(connfd);
   }
   return(0);
}

