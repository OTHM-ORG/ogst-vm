#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

struct ogst_socket {
	struct sockaddr_un socket;
	unsigned int sd;
};

struct ogst_socket *ogst_socket_new(char *path)
{
	int len;
	struct ogst_socket *sock;

	sock = malloc(sizeof(struct ogst_socket));
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

struct ogst_socket *ogst_socket_accept(struct ogst_socket *sock)
{
	int len;
	struct ogst_socket *remote_sock;

	remote_sock = malloc(sizeof(struct ogst_socket));
	if ((remote_sock->sd =
	     accept(sock->sd, (struct sockaddr *) &remote_sock->socket,
		    &len)) == -1) {
		perror("accept");
		exit(1);
        }
	return remote_sock;
}

void test(struct ogst_socket *sock)
{
	int done, n;
	char str[100];

	done = 0;
        do {
		n = recv(sock->sd, str, 100, 0);
		if (n <= 0) {
			if (n < 0)
				perror("recv");
			done = 1;
		}

		if (!done)
			if (send(sock->sd, str, n, 0) < 0) {
				perror("send");
				done = 1;
			}
        } while (!done);
}

void ogst_socket_run(struct ogst_socket *sock,
		     void (*sock_funct)(struct ogst_socket *))
{
	sock_funct(sock);
}

int main(void)
{
    struct ogst_socket *s1 = ogst_socket_new("../echo_socket");


    if (listen(s1->sd, 5) == -1) {
        perror("listen");
        exit(1);
    }


    printf("Waiting for a connection...\n");

    struct ogst_socket *s2 = ogst_socket_accept(s1);

    printf("Connected.\n");

    ogst_socket_run(s2, test);

    close(s2->sd);

    free(s1);
    free(s2);

    return 0;
}
