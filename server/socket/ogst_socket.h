#ifndef OGST_SOCKET_H
#define OGST_SOCKET_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


struct ogst_connecter {
	struct sockaddr_un socket;
	unsigned int sd;
};

struct ogst_connection {
	struct sockaddr_un socket;
	unsigned int sd;

	fd_set set;
	struct timeval timeout;

	pthread_mutex_t done_mutex;

	pthread_mutex_t end_mutex;
	int end_bool;
};

struct ogst_connecter *ogst_connecter_new(struct ogst_connecter *(*gen)(void),
					  char *path);

struct ogst_connection *ogst_connecter_accept(struct ogst_connection *(*gen)(void),
					      struct ogst_connecter *sock);

int ogst_connection_end_check(struct ogst_connection *socket);

void ogst_connection_end_mutate(struct ogst_connection *socket, int value);

void ogst_connection_kill(struct ogst_connection *user_socket);

#endif
