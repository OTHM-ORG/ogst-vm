#ifndef OGST_SOCKET_H
#define OGST_SOCKET_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


struct ogst_socket {
	struct sockaddr_un socket;
	unsigned int sd;
};

struct ogst_socket *ogst_socket_new(struct ogst_socket *(*gen)(void),
				    char *path);

struct ogst_socket *ogst_socket_accept(struct ogst_socket *(*gen)(void),
				       struct ogst_socket *sock);

#endif
