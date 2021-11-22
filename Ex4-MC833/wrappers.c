#include "wrappers.h"

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
        exit(1);
	}
    else
    {
        return connfd;
    }
}

void Close(int connfd)
{
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

pid_t Fork(){
    pid_t proc_id = fork();

    return proc_id;
}


ssize_t Read(int fd, void *buf, size_t count)
{
    return read(fd, buf, count);
}

ssize_t Write(int fd, const void *buf, size_t count){
    return write(fd, buf, count);
}

void Signal(int signum, sig_t handler){
    signal(signum, handler);
}

const char *Inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    return inet_ntop(af, src, dst, size);
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout){
    return select(nfds, readfds, writefds, exceptfds, timeout);
}

int Shutdown(int socket, int how){
    return shutdown(socket, how);
}

ssize_t Getline(char **lineptr, size_t *n, FILE *stream){  
    return  getline(lineptr, n, stream); 
}
