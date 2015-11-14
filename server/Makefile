ogst_server: ogst_server.o ogst_socket.o
	gcc ogst_server.o ogst_socket.o -o ogst_server -lothm_thread \
	-lothm_symbols -lothm_base -lothm_hashmap -pthread
ogst_server.o: ogst_server.c ogst_socket.o
	gcc -Wall -Werror -fPIC -c ogst_server.c -o ogst_server.o
ogst_socket.o: ogst_server.c
	gcc -Wall -Werror -fPIC -c ogst_socket.c -o ogst_socket.o
