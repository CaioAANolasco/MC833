#include "wrappers.h"

#define LISTENQ 10
#define MAXDATASIZE 4096

//QUESTÃO 2 - FUNÇÃO ECHO PARA QUESTÃO 2
//QUESTÃO 4 - FECHAR CONEXÃO AO RECEBER O COMANDO "QUIT" DO SERVIDOR 
void str_echo(int sockfd)
{
   //QUESTÃO 2
   char a[] = {"pwd ls ifconfig"}; //Comandos para enviar ao cliente, separados por espaço
   
   //QUESTÃO 4)A
   //char a[] = {"pwd ls ifconfig quit"};

   char  buf[MAXDATASIZE];
   char output_name[16] = "output.txt";

   char string_output [MAXDATASIZE];

   FILE *fp = fopen(output_name, "ab");

   Write(sockfd,  a , strlen(a)); //Escrever os comandos no socket

   while( Read(sockfd, buf, MAXDATASIZE) > 0)
   {
      printf("%s", buf); //Ler resultados enviados pelo cliente
      strcat(string_output, buf);
   } 

   fwrite(string_output, strlen(string_output), 1, fp);
   fclose(fp);
} 

//FUNÇÃO ECHO PARA QUESTÃO 3
/*void str_echo(int sockfd, FILE *fp)
{
   char  buf[MAXDATASIZE];
   char string_output [MAXDATASIZE];

   while( Read(sockfd, buf, MAXDATASIZE) > 0)
   {
      strcat(string_output, buf);
   } 

   fwrite(string_output, strlen(string_output), 1, fp);
} */

int main (int argc, char **argv) {
   int    listenfd, connfd; //Descritores de socket, para escutar e para quando conexão é aceita
   struct sockaddr_in servaddr; //Struck sockaddr_in para armazenar endereço de dominio e numero de porta
   pid_t childpid;

   //QUESTÃO 3 - ABRIR ARQUIVO PARA QUESTÃO 3
   //FILE *fp = fopen("serverlog.txt", "ab+");

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

   for ( ; ; ) {
      connfd = Accept(listenfd, (struct sockaddr *) NULL, NULL); //Aceita a conexão, e inicia outro descritor de socket para esta conexão

      // QUESTÃO 3 - CLIENTE CONECTOU
      /*char   time_buf[MAXDATASIZE];
      time_t ticks = time(NULL);
      snprintf(time_buf, sizeof(time_buf), "%.24s\r\n", ctime(&ticks));
      fwrite(time_buf, strlen(time_buf), 1, fp); */

      if ((childpid = Fork()) == 0)
      {
         Close(listenfd);

         //QUESTÃO 2 E 4 
         str_echo(connfd);

         //QUESTÃO 3
         //str_echo(connfd, fp);
         

         //QUESTÃO 3 - CLIENTE DESCONECTOU
         /*memset(time_buf, 0, strlen(time_buf));
         ticks = time(NULL);
         snprintf(time_buf, sizeof(time_buf), "%.24s\r\n", ctime(&ticks));
         fwrite(time_buf, strlen(time_buf), 1, fp);
         */
            
         exit(0);
      }

      Close(connfd);
   }
   
   //QUESTÃO 3 - CLOSE
   //fclose(fp);
   return(0);
}
