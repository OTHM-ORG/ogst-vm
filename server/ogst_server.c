#include <string.h>
#include <othm_thread.h>
#include <othm_tag.h>
#include <othm_object.h>

#include "socket/ogst_socket.h"

#include <fcntl.h>

char *ogst_list_form = "list";
char *ogst_socket_form = "socket";


struct othm_list *ogst_list_gen(void)
{
	struct othm_obj_tag *tagged =
		malloc(sizeof(struct othm_obj_tag) +
		       sizeof(struct othm_list));
	tagged->mutability = 0;
	tagged->data_form = ogst_list_form;
	return OTHM_GET_TAGGED_LEFT(struct othm_list *,
				    tagged);
}

/* struct ogst_socket *ogst_socket_gen(void) */
/* { */
/* 	struct othm_obj_tag *tagged = */
/* 		malloc(sizeof(struct othm_obj_tag) + */
/* 		       sizeof(struct ogst_socket)); */
/* 	tagged->mutability = 1; */
/* 	tagged->data_form = ogst_socket_form; */
/* 	return OTHM_GET_TAGGED_LEFT(struct othm_socket *, */
/* 				    tagged); */
/* } */

struct ogst_connecter *ogst_connecter_gen(void)
{
	struct othm_obj_tag *tagged =
		malloc(sizeof(struct othm_obj_tag) +
		       sizeof(struct ogst_connecter));
	tagged->mutability = 1;
	tagged->data_form = ogst_socket_form;
	return OTHM_GET_TAGGED_LEFT(struct ogst_connecter *,
				    tagged);
}

struct ogst_connection *ogst_connection_gen(void)
{
	struct othm_obj_tag *tagged =
		malloc(sizeof(struct othm_obj_tag) +
		       sizeof(struct ogst_connection));
	tagged->mutability = 1;
	tagged->data_form = ogst_socket_form;
	return OTHM_GET_TAGGED_LEFT(struct ogst_connection *,
				    tagged);
}

OTHM_CHAIN_DEFUN(testing, testing)
{

	struct ogst_connection *sock = control->result;
	int n;
	unsigned int old_size = 0;
	unsigned int size;
	char *str;

	str = malloc(old_size);

	if (!OTHM_GET_LEFT_TAG
	    (struct othm_obj_tag *, position)->mutability)
		printf("I am not mutable!\n");

        while (!ogst_connection_end_check(sock)) {
		pthread_mutex_lock(&sock->done_mutex);

		FD_ZERO(&sock->set);
		FD_SET(sock->sd,
		       &sock->set);
		sock->timeout.tv_sec = 0;
		sock->timeout.tv_usec = 0;

		int retval = select(FD_SETSIZE, &sock->set, NULL, NULL,  &sock->timeout);
		if (retval == 1) {

			recv(sock->sd, &size, sizeof(unsigned int), 0);
			printf("I got the number: %u\n", size);
			if (size > old_size) {
				free(str);
				str = malloc(size);
				old_size = size;
			}

			n = recv(sock->sd, str, size, 0);
			printf("and the string, %.*s", size, str);

			if (n <= 0) {
				if (n < 0)
					perror("recv");
				ogst_connection_end_mutate(sock, 1);
			}

			if (!ogst_connection_end_check(sock))
				if (send(sock->sd, str, n, 0) < 0) {
					perror("send");
					ogst_connection_end_mutate(sock, 1);
				}
		}

		pthread_mutex_unlock(&sock->done_mutex);
        }
	printf("bye!\n");
}

int main(void)
{
	struct ogst_connecter *s1 = ogst_connecter_new(ogst_connecter_gen, "../echo_socket");
	struct othm_list *chain =
		OTHM_CHAIN_DIRECT(ogst_list_gen, testing);

    /* printf("Waiting for a connection...\n"); */
	/* int i = 0; */
	/* while (1) { */
		struct ogst_connection *s2 = ogst_connecter_accept(ogst_connection_gen, s1);

		struct othm_thread *thread = othm_thread_new(9, chain, NULL,
							     s2, NULL);
		othm_thread_start(thread);

		sleep(3);
		ogst_connection_kill(s2);
	/* 	++i; */
	/* } */

    /* printf("Connected.\n"); */

    /* ogst_socket_run(s2, test); */
    /* while(1) {} */
    /* close(s2->sd); */

    /* free(s1); */
    /* free(s2); */
    pthread_exit(NULL);
    return 0;
}
