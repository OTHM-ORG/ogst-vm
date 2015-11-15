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

	pthread_mutex_t done_mutex;

	pthread_mutex_t end_mutex;
	int end_bool;
};

struct ogst_socket *ogst_socket_new(struct ogst_socket *(*gen)(void),
				    char *path);

struct ogst_socket *ogst_socket_accept(struct ogst_socket *(*gen)(void),
				       struct ogst_socket *sock);

int ogst_socket_end_check(struct ogst_socket *socket);

void ogst_socket_end_mutate(struct ogst_socket *socket, int value);

void ogst_socket_kill_user(struct ogst_socket *user_socket);

#endif
