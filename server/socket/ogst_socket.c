#include "ogst_socket.h"
#include <pthread.h>

struct ogst_socket *ogst_socket_new(struct ogst_socket *(*gen)(void),
				    char *path)
{
	int len;
	struct ogst_socket *sock;

	sock = gen();
	if ((sock->sd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	sock->socket.sun_family = AF_UNIX;
	strcpy(sock->socket.sun_path, path);
	unlink(path);
	len = strlen(path) + sizeof(sock->socket.sun_family);
	if (bind(sock->sd, (struct sockaddr *)&sock->socket, len) == -1) {
		perror("bind");
		exit(1);
	}
	return sock;
}

struct ogst_socket *ogst_socket_accept(struct ogst_socket *(*gen)(void),
				       struct ogst_socket *sock)
{
	unsigned int len;
	struct ogst_socket *remote_sock;

	if (listen(sock->sd, 5) == -1) {
		perror("listen");
		exit(1);
	}

	remote_sock = gen();
	if ((remote_sock->sd = accept
	     (sock->sd,
	      (struct sockaddr *) &remote_sock->socket,
	      &len)) == -1) {
		perror("accept");
		exit(1);
        }
	remote_sock->end_bool = 0;
	return remote_sock;
}

int ogst_socket_end_check(struct ogst_socket *socket)
{
	int value;
	pthread_mutex_lock(&socket->end_mutex);
	value = socket->end_bool;
	pthread_mutex_unlock(&socket->end_mutex);
	return value;
}

void ogst_socket_end_mutate(struct ogst_socket *socket, int value)
{
	pthread_mutex_lock(&socket->end_mutex);
	socket->end_bool = value;
	pthread_mutex_unlock(&socket->end_mutex);
}

void ogst_socket_kill_user(struct ogst_socket *user_socket)
{
	ogst_socket_end_mutate(user_socket, 1);
	pthread_mutex_lock(&user_socket->done_mutex);
	pthread_mutex_unlock(&user_socket->done_mutex);
	close(user_socket->sd);
}
