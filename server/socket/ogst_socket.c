#include "ogst_socket.h"
#include <pthread.h>

struct ogst_connecter *ogst_connecter_new(struct ogst_connecter *(*gen)(void),
					  char *path)
{
	int len;
	struct ogst_connecter *sock;

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

struct ogst_connection *ogst_connecter_accept(struct ogst_connection *(*gen)(void),
					      struct ogst_connecter *sock)
{
	unsigned int len;
	struct ogst_connection *remote_sock;

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

	/* fcntl(remote_sock->sd, F_SETFL, O_NONBLOCK); */


	return remote_sock;
}

int ogst_connection_end_check(struct ogst_connection *socket)
{
	int value;
	pthread_mutex_lock(&socket->end_mutex);
	value = socket->end_bool;
	pthread_mutex_unlock(&socket->end_mutex);
	return value;
}

void ogst_connection_end_mutate(struct ogst_connection *socket, int value)
{
	pthread_mutex_lock(&socket->end_mutex);
	socket->end_bool = value;
	pthread_mutex_unlock(&socket->end_mutex);
}

void ogst_connection_kill(struct ogst_connection *user_socket)
{
	int true_val = 1;
	pthread_mutex_lock(&user_socket->done_mutex);
	ogst_connection_end_mutate(user_socket, 1);
	setsockopt(user_socket->sd, SOL_SOCKET,SO_REUSEADDR,
		   &true_val, sizeof(int));
	pthread_mutex_unlock(&user_socket->done_mutex);
}
