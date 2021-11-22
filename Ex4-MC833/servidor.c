#include "wrappers.h"

#define LISTENQ 10
#define MAXDATASIZE 4096

void str_echo(int sockfd)
{
   char   buf[MAXDATASIZE];
   ssize_t n;

   while((n = Read(sockfd, buf, MAXDATASIZE)) > 0){
      Write(sockfd, buf, n);
      bzero(buf, sizeof(buf));
   }
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

   Listen(listenfd, LISTENQ); //Iniciar a escuta por conexões, usando o descritor de socket listenfd criado

   Signal(SIGCHLD, sig_chld);

   for ( ; ; ) {
      connfd = Accept(listenfd, (struct sockaddr *) NULL, NULL); //Aceita a conexão, e inicia outro descritor de socket para esta conexão

      if ((childpid = Fork()) == 0)
      {
         Close(listenfd);
         str_echo(connfd);  
         exit(0);
      }

      Close(connfd);
   }
   return(0);
}