#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE 4096

int main(int argc, char **argv) {
   int    sockfd, n; //Descritores de socket, para se comunicar com o servidor
   char   recvline[MAXLINE + 1];
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr; //Struck sockaddr_in para armazenar endereço de dominio e numero de porta

   if (argc != 2) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      perror(error);
      exit(1);
   }

   if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) { //Inicialização do socket
      perror("socket error");
      exit(1);
   }

   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port   = htons(50785); //Número de porta do socket do servidor
   if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
      perror("inet_pton error");
      exit(1);
   }

   if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) { //Faz a requisição de conexão com o servidor, e inicia o descritor de socket sockfd
      perror("connect error");
      exit(1);
   }

   socklen_t len = sizeof(servaddr);
   if (getsockname(sockfd, (struct sockaddr *)&servaddr, &len) == -1)
      perror("getsockname");
   else
   {
      char clientIP[16];
      inet_ntop(AF_INET, &servaddr.sin_addr, clientIP, sizeof(clientIP));
      printf("Client IP address: %s\n", clientIP);
      printf("Client port number: %d\n", ntohs(servaddr.sin_port));
   }

   while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
      recvline[n] = 0;
      if (fputs(recvline, stdout) == EOF) {
         perror("fputs error");
         exit(1);
      }
   }

   if (n < 0) {
      perror("read error");
      exit(1);
   }

   exit(0);
}
