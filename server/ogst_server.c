#include <string.h>
#include <othm_thread.h>
#include <othm_tag.h>

#include "socket/ogst_socket.h"
#include "object/ogst_object.h"

char *ogst_list_form = "list";
char *ogst_socket_form = "socket";


struct othm_list *ogst_list_gen(void)
{
	struct ogst_tag *tagged =
		malloc(sizeof(struct ogst_tag) +
		       sizeof(struct othm_list));
	tagged->mutability = 0;
	tagged->data_form = ogst_list_form;
	return OTHM_GET_TAGGED_LEFT(struct othm_list *,
				    tagged);
}

struct ogst_socket *ogst_socket_gen(void)
{
	struct ogst_tag *tagged =
		malloc(sizeof(struct ogst_tag) +
		       sizeof(struct ogst_socket));
	tagged->mutability = 1;
	tagged->data_form = ogst_socket_form;
	return OTHM_GET_TAGGED_LEFT(struct othm_socket *,
				    tagged);
}

OTHM_CHAIN_DEFUN(testing, testing)
{

	struct ogst_socket *sock = control->result;
	int n;
	unsigned int old_size = 0;
	unsigned int size;
	char *str;

	str = malloc(old_size);

	if (!OTHM_GET_LEFT_TAG
	    (struct ogst_tag *, position)->mutability)
		printf("I am not mutable!\n");

	ogst_socket_end_mutate(sock, 0);
        do {
		pthread_mutex_lock(&sock->done_mutex);

		recv(sock->sd, &size, sizeof(unsigned int), 0);
		if (size > old_size) {
			free(str);
			str = malloc(size);
			old_size = size;
		}

		n = recv(sock->sd, str, size, 0);
		printf("I got the number: %u\n", size);
		printf("and the string, %s", str);

		/* if (size == 5 && strncmp(str, "sock->end", 4) == 1) { */
		/* 	sock->end = 1; */
		/* 	close(sock->sd); */
		/* } */

		if (n <= 0) {
			if (n < 0)
				perror("recv");
			ogst_socket_end_mutate(sock, 1);
		}

		if (!ogst_socket_end_check(sock))
			if (send(sock->sd, str, n, 0) < 0) {
				perror("send");
				ogst_socket_end_mutate(sock, 1);
			}

		pthread_mutex_unlock(&sock->done_mutex);
        } while (!ogst_socket_end_check(sock));
}

int main(void)
{
	struct ogst_socket *s1 = ogst_socket_new(ogst_socket_gen, "../echo_socket");
	struct othm_list *chain =
		OTHM_CHAIN_DIRECT(ogst_list_gen, testing);

    /* printf("Waiting for a connection...\n"); */
	/* int i = 0; */
	/* while (1) { */
		struct ogst_socket *s2 = ogst_socket_accept(ogst_socket_gen, s1);

		struct othm_thread *thread = othm_thread_new(9, chain, NULL,
							     s2, NULL);
		othm_thread_start(thread);

		sleep(50);
		ogst_socket_kill_user(s2);
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
