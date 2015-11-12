#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <othm_thread.h>
#include <othm_tag.h>

char *ogst_list_form = "list";

struct ogst_socket {
	struct sockaddr_un socket;
	unsigned int sd;
};

struct ogst_tag {
	int mutability;
	void *data_form;
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
	if ((remote_sock->sd = accept
	     (sock->sd,
	      (struct sockaddr *) &remote_sock->socket,
	      &len)) == -1) {
		perror("accept");
		exit(1);
        }
	return remote_sock;
}


OTHM_CHAIN_DEFUN(testing, testing)
{
	struct ogst_socket *sock = control->result;
	int done, n;
	char str[100];

	if (!OTHM_GET_LEFT_TAG
	    (struct ogst_tag *, position)->mutability)
		printf("I am not mutable!\n");

	done = 0;
        do {
		n = recv(sock->sd, str, 100, 0);
		printf("Got input.\n");
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

struct othm_list *ogst_list_new(void)
{
	struct ogst_tag *tagged =
		malloc(sizeof(struct ogst_tag) +
		       sizeof(struct othm_list));
	tagged->mutability = 0;
	tagged->data_form = ogst_list_form;
	return OTHM_GET_TAGGED_LEFT(struct othm_list *,
				    tagged);
}

int main(void)
{
    struct ogst_socket *s1 = ogst_socket_new("../echo_socket");
    struct othm_list *chain =
	    OTHM_CHAIN_DIRECT(ogst_list_new, testing);

    if (listen(s1->sd, 5) == -1) {
        perror("listen");
        exit(1);
    }


    /* printf("Waiting for a connection...\n"); */
    int i = 0;
    while (1) {
    struct ogst_socket *s2 = ogst_socket_accept(s1);

    struct othm_thread *thread = othm_thread_new(i, chain, NULL,
    						 s2, NULL);
    othm_thread_start(thread);
    ++i;
    }

    /* printf("Connected.\n"); */

    /* ogst_socket_run(s2, test); */
    /* while(1) {} */
    /* close(s2->sd); */

    /* free(s1); */
    /* free(s2); */
    pthread_exit(NULL);
    return 0;
}
