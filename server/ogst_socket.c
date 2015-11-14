#include "ogst_socket.h"

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
	return remote_sock;
}
