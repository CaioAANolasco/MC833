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

int Socket(int family, int type, int flags)
{
	int sockfd;

	if ((sockfd = socket(family, type, flags)) < 0)
    {
		perror("socket");

        exit(1);
    }
	else
    {
        return sockfd;
    }
}

void Bind(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (bind(fd, sa, salen) < 0)
    {
		perror("bind error");
        exit(1);
    }
}

void Listen(int fd, int backlog)
{
	if (listen(fd, backlog) < 0)
    {
		perror("listen error");
        exit(1);
    }
}

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int connfd;

	if ((connfd = accept(fd, sa, salenptr)) < 0) 
    {
        perror("accept error");
	}
    else
    {
        return connfd;
    }
}

void Close(int connfd){
    close(connfd);
}

void Connect(int fd, const struct sockaddr *sa, socklen_t salen)
{
	if (connect(fd, sa, salen) < 0)
    {
		perror("connect error");
        exit(1);
    }
}

void Inet_pton(int af, const char *src, void *dst)
{
    if (inet_pton(af, src, dst) <= 0) {
        perror("inet_pton error");
        exit(1);
    }
}
