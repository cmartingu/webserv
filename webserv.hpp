#pragma once

# include <iostream>


//****************FUNCIONES UNISTD****************
#include <unistd.h>
//int execve(const char *pathname, char *const argv[], char *const envp[]);
//int dup(int oldfd);
//int dup2(int oldfd, int newfd);
//int pipe(int pipefd[2]);
//pid_t fork(void);


//****************FUNCIONES STRING****************
#include <string.h>
/*
 * strerror: Devuelve una cadena de texto que describe el error basado en el valor de errno.
 */
//char *strerror(int errnum);


//****************FUNCIONES NETDB****************
#include <netdb.h>

/*
 * gai_strerror: Devuelve una cadena de texto que describe errores de getaddrinfo.
 */
//const char *gai_strerror(int errcode);


//****************FUNCIONES ERRNO****************
#include <errno.h>

/*
 * errno: Variable global que contiene el código de error del sistema en la última llamada a una función.
 * Sintaxis: extern int errno;
 */


//****************FUNCIONES SYS/SOCKET****************
#include <sys/socket.h>
/*
 * socketpair: Crea un par de sockets conectados entre sí.
 */
//int socketpair(int domain, int type, int protocol, int sv[2]);

/*
 * socket: Crea un nuevo descriptor de socket.
 */
//int socket(int domain, int type, int protocol);

/*
 * accept: Acepta una conexión entrante en un socket.
 */
//int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

/*
 * listen: Pone el socket en modo de escucha para conexiones entrantes.
 */
//int listen(int sockfd, int backlog);

/*
 * send: Envía datos a través de un socket.
 */
//ssize_t send(int sockfd, const void *buf, size_t len, int flags);

/*
 * recv: Recibe datos desde un socket.
 */
//ssize_t recv(int sockfd, void *buf, size_t len, int flags);

/*
 * connect: Conecta un socket a un servidor.
 */
//int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

/*
 * setsockopt: Establece opciones en un socket.
 */
//int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);


//****************FUNCIONES ARPA/INET****************
#include <arpa/inet.h>
/*
 * htons: Convierte un número corto de host a formato de red (big-endian).
 */
//uint16_t htons(uint16_t hostshort);

/*
 * htonl: Convierte un número largo de host a formato de red (big-endian).
 */
//uint32_t htonl(uint32_t hostlong);

/*
 * ntohs: Convierte un número corto de formato de red a formato de host (little-endian).
 */
//uint16_t ntohs(uint16_t netshort);

/*
 * ntohl: Convierte un número largo de formato de red a formato de host (little-endian).
 */
//uint32_t ntohl(uint32_t netlong);


//****************FUNCIONES NETINET/IN****************
#include <netinet/in.h>
// Definiciones relacionadas con direcciones y estructuras para redes.


//****************FUNCIONES SYS/SELECT****************
#include <sys/select.h>
/*
 * select: Espera a que uno o más descriptores estén listos para una operación de I/O.
 */
//int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);


//****************FUNCIONES SYS/EPOLL****************
#include <sys/epoll.h>
/*
 * epoll_create: Crea un descriptor de epoll.
 */
//int epoll_create(int size);

/*
 * epoll_ctl: Controla los eventos de un descriptor epoll.
 */
//int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);

/*
 * epoll_wait: Espera a que ocurran eventos en un descriptor epoll.
 */
//int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);


//****************FUNCIONES FCNTL****************
#include <fcntl.h>

/*
 * fcntl: Manipula los descriptores de archivo.
 */
//int fcntl(int fd, int cmd, ...);


//****************FUNCIONES SYS/TYPES, SYS/STAT****************
#include <sys/types.h>
#include <sys/stat.h>
/*
 * open: Abre un archivo y devuelve su descriptor.
 */
//int open(const char *pathname, int flags, mode_t mode);

/*
 * stat: Obtiene información sobre un archivo.
 */
//int stat(const char *pathname, struct stat *statbuf);

/*
 * access: Verifica los permisos de un archivo.
 */
//int access(const char *pathname, int mode);


//****************FUNCIONES SYS/EVENT****************
# include <sys/event.h>
/*
 *Crea una cola de eventos que puede ser monitoreada.
 *Devuelve un descriptor de archivo que se usa para interactuar con el sistema de eventos
 */
//int kqueue(void);

/*
 *Gestiona eventos en una cola creada con kqueue.
 *Se utiliza para registrar nuevos eventos y recuperar eventos que hayan ocurrido
 */
//int kevent(int kq, const struct kevent *changelist, int nchanges, struct kevent *eventlist, int nevents, const struct timespec *timeout);