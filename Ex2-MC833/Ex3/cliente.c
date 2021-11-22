#include "wrappers.h"


#define MAXLINE 4096

void str_cli(int sockfd, struct sockaddr_in servaddr){
   int n;
   char   recvline[MAXLINE + 1];

   socklen_t len = sizeof(servaddr);
   if (getsockname(sockfd, (struct sockaddr *)&servaddr, &len) == -1)
      perror("getsockname");
   else
   {
      char clientIP[16];
      Inet_ntop(AF_INET, &servaddr.sin_addr, clientIP, sizeof(clientIP));
      printf("Client IP address: %s\n", clientIP);
      printf("Client port number: %d\n", ntohs(servaddr.sin_port));
   }

   while ( (n = Read(sockfd, recvline, MAXLINE)) > 0) {
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
}
 

int main(int argc, char **argv) {
   int    sockfd ; //Descritores de socket, para se comunicar com o servidor
   char   error[MAXLINE + 1];
   struct sockaddr_in servaddr; //Struck sockaddr_in para armazenar endereço de dominio e numero de porta

   if (argc != 3) {
      strcpy(error,"uso: ");
      strcat(error,argv[0]);
      strcat(error," <IPaddress>");
      perror(error);
      exit(1);
   }

   sockfd = Socket(AF_INET, SOCK_STREAM, 0); //Inicialização do socket

   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port   = htons(33333); //Número de porta do socket do servidor
   Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

   Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)); //Faz a requisição de conexão com o servidor, e inicia o descritor de socket sockfd

   str_cli(sockfd, servaddr);

   exit(0);
}
