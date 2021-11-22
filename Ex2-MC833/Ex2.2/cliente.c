#include "wrappers.h"

char *reverse_str(char *str, int len);

#define MAXLINE 4096

//QUESTÃO 2 -FUNÇÃO CLIENTE PARA QUESTÃO 2
//QUESTÃO 4 - FECHAR CONEXÃO AO RECEBER O COMANDO "QUIT" DO SERVIDOR 
void str_cli(int sockfd, struct sockaddr_in servaddr){
   char buf[MAXLINE]; //Buffer para comandos enviados pelo servidor
   char command[20]; //string para comando
   size_t buf_idx = 0; //Index para buffer de comandos
   size_t cmd_idx = 0; //Index para string de comando
   char output_buff[MAXLINE]; //Buffer para arquivo de saída do comando

   if(Read(sockfd, buf, MAXLINE) < 0)
   {
      perror("read error");
      exit(1);
   } //Ler cadeia de caracteres para mensagem enviada pelo servidor.

   while(buf_idx < strlen(buf) + 1)
   {
      if(buf[buf_idx] == ' ' || buf[buf_idx] == '\0')
      {
         //QUESTÃO 4-a) - COMANDO QUIT
         /*if(!strcmp(command, "quit"))
         {
            printf("Comando quit encontrado. Encerrando cliente\n");
            return;
         } */

         //QUESTÃO 4-b) Inverter cada comando recebido pelo cliente
         /*char cpy_cmd[20];
         strcpy(cpy_cmd, command);
         printf("%s\n", reverse_str(cpy_cmd, strlen(cpy_cmd)));
         */

         FILE *fp = popen(command, "r"); //Executar o comando e abrir um descritor de arquivo com a sua saída.

         fread(output_buff, MAXLINE, 1, fp);

         Write(sockfd, output_buff, sizeof(output_buff)); //Escrever saída no fd do socket.

         memset(command, 0, strlen(command)); //Zerar buffer de comando para ler próximo
         cmd_idx = 0;

         fclose(fp);
      }

      else
      {
         command[cmd_idx] = buf[buf_idx];
         cmd_idx++; //Adiciona cada caracter da lista de comando até achar um comando
      }

      buf_idx++;
   } 

   char clientIP[16];
   char port_str[16]; //Strings para IP e porta do cliente
   uint16_t port_num = 0;
   char client_info[32];

   socklen_t len = sizeof(servaddr);
   if (getsockname(sockfd, (struct sockaddr *)&servaddr, &len) == -1)
      perror("getsockname"); //Usa getsocketname para pegar informações do cliente para enviar para o servidor.
   else
   {
      inet_ntop(AF_INET, &servaddr.sin_addr, clientIP, sizeof(clientIP));
      printf("Client IP address: %s\n", clientIP);
      port_num = ntohs(servaddr.sin_port);
      printf("Client port number: %d\n", port_num);
   } 

   strcat(client_info, clientIP);
   strcat(client_info, "\n");
   sprintf(port_str, "%u", port_num);
   strcat(client_info, port_str);
   strcat(client_info, "\n"); //Junta strings de IP e porta em uma só e escreve no socket.

   Write(sockfd, client_info, sizeof(client_info));

} 


//QUESTÃO 3- FUNÇÃO CLIENTE PARA QUESTÃO 3
/*void str_cli(int sockfd, struct sockaddr_in servaddr){
   char clientIP[16];
   char port_str[16]; //Strings para IP e porta do cliente
   uint16_t port_num = 0;
   char client_info[32] = "";

   socklen_t len = sizeof(servaddr);
   if (getsockname(sockfd, (struct sockaddr *)&servaddr, &len) == -1)
      perror("getsockname"); //Usa getsocketname para pegar informações do cliente para enviar para o servidor.
   else
   {
      inet_ntop(AF_INET, &servaddr.sin_addr, clientIP, sizeof(clientIP));
      printf("Client IP address: %s\n", clientIP);
      port_num = ntohs(servaddr.sin_port);
      printf("Client port number: %d\n", port_num);
   } 

   printf("%s", client_info);

   strcat(client_info, clientIP);
   strcat(client_info, "\n");
   sprintf(port_str, "%u", port_num);
   strcat(client_info, port_str);
   strcat(client_info, "\n"); //Junta strings de IP e porta em uma só e escreve no socket.

   Write(sockfd, client_info, strlen(client_info));
} */

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

   str_cli(sockfd, servaddr);

   exit(0);
}

char *reverse_str(char *str, int len)
{
    char *p1 = str;
    char *p2 = str + len - 1;

    while (p1 < p2) {
        char tmp = *p1;
        *p1++ = *p2;
        *p2-- = tmp;
    }

    return str;
}
