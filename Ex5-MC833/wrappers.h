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
#include <time.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/time.h>
#include <math.h>

int Socket(int family, int type, int flags);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);
void Listen(int fd, int backlog);
int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Close(int connfd);
void Connect(int fd, const struct sockaddr *sa, socklen_t salen);
void Inet_pton(int af, const char *src, void *dst);
pid_t Fork();
ssize_t Read(int fd, void *buf, size_t count);
ssize_t Write(int fd, const void *buf, size_t count);
void Signal(int signum, sig_t handler);
const char *Inet_ntop(int af, const void *src, char *dst, socklen_t size);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int Shutdown(int socket, int how);
ssize_t Getline(char **lineptr, size_t *n, FILE *stream);
ssize_t Send(int sockfd, const void *buf, size_t len, int flags);
ssize_t Recv(int sockfd, void *buf, size_t len, int flags);