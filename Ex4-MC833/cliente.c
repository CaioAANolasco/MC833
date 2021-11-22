#include "wrappers.h"

#define MAX(a,b) (((a)>(b))?(a):(b))

#define MAXLINE 4096

void str_cli(int sockfd){
   char *sendline, recvline[MAXLINE]; //buffers for sending and receiveing echoed message
   fd_set rset; //FD_SET struct for select()
   size_t bufsize = 4096;
   int maxfdp1, stdineof;

   stdineof = 0; //EOF flag

   sendline = (char *)malloc(bufsize * sizeof(char)); //Buffer for getline() needs to be a pointer initialized by malloc()

   FD_ZERO(&rset);

   for( ; ; ){
      if (stdineof == 0){
         FD_SET(fileno(stdin), &rset);
      }
      FD_SET(sockfd, &rset);
      //Set FD read sets

      maxfdp1 = MAX(fileno(stdin), sockfd) + 1;
      //Find maximum file descriptor value plus one.

      Select(maxfdp1, &rset, NULL, NULL, NULL); 
      //Call select()

      if(FD_ISSET(fileno(stdin), &rset)){
         if(Getline(&sendline, &bufsize, stdin) == -1){ //Read input file line by line.
            stdineof = 1; //If getline() == -1, found EOF.
            Shutdown(sockfd, SHUT_WR); //Call Shutdown to socket
            FD_CLR(fileno(stdin), &rset);
            continue;
         }
         Write(sockfd, sendline, strlen(sendline)); //Write to connected socket.
         bzero(sendline, bufsize);
      }

      if(FD_ISSET(sockfd, &rset)){
         if(Read(sockfd, recvline, MAXLINE) == 0){ //Read echoed message
            if(stdineof == 1){
               return;
            }else{
               perror("server terminated early");
            }
         }
         fputs(recvline, stdout); //Write echoed message to stoud, output file
         bzero(recvline, sizeof(recvline));
      }
   }

   free(sendline); //free malloc memory
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
   servaddr.sin_port   = htons(atoi(argv[2])); //Número de porta do socket do servidor
   Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

   Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)); //Faz a requisição de conexão com o servidor, e inicia o descritor de socket sockfd

   str_cli(sockfd);

   exit(0);
}


