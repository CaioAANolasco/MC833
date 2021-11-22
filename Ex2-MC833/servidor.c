#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LISTENQ 10
#define MAXDATASIZE 100

int main (int argc, char **argv) {
   int    listenfd, connfd; //Descritores de socket, para escutar e para quando conexão é aceita
   struct sockaddr_in servaddr; //Struck sockaddr_in para armazenar endereço de dominio e numero de porta
   char   buf[MAXDATASIZE];
   time_t ticks;

   if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) { //Inicialização do socket
      perror("socket");
      exit(1);
   }

   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family      = AF_INET;
   servaddr.sin_addr.s_addr = INADDR_ANY; //Adota como endereço de domínio o IP da máquina do servidor
   servaddr.sin_port        = 0;  //Permite que o sistema atribua um numero de porta temporário

   if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) { //Unir o socket com a porta de numero especificada 
      perror("bind");
      exit(1);
   }
   
   if (listen(listenfd, LISTENQ) == -1) { //Iniciar a escuta por conexões, usando o descritor de socket listenfd criado
      perror("listen");
      exit(1);
   }

   socklen_t len = sizeof(servaddr);
   if (getsockname(listenfd, (struct sockaddr *)&servaddr, &len) == -1)
      perror("getsockname");
   else
      printf("Server port number: %d\n", ntohs(servaddr.sin_port)); //Imprime o numero da porta do socket do servidor

   for ( ; ; ) {
      if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) { //Aceita a conexão, e inicia outro descritor de socket para esta conexão
         perror("accept");
         exit(1);
      }

      socklen_t len = sizeof(servaddr);
      if (getpeername(connfd, (struct sockaddr *)&servaddr, &len) == -1)
         perror("getpeername");
      else
      {
         char clientIP[16];
         inet_ntop(AF_INET, &servaddr.sin_addr, clientIP, sizeof(clientIP));
         printf("Client IP address: %s\n", clientIP);
         printf("Client port number: %d\n", ntohs(servaddr.sin_port));
      }

      ticks = time(NULL);
      snprintf(buf, sizeof(buf), "%.24s\r\n", ctime(&ticks));
      write(connfd, buf, strlen(buf));
      
      close(connfd);
   }
   return(0);
}
