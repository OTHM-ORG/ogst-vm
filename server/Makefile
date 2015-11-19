ogst_server: ogst_server.o socket/ogst_socket.o
	gcc ogst_server.o socket/ogst_socket.o -o ogst_server \
	-lothm_object -lothm_thread -lothm_symbols -lothm_base \
	-lothm_hashmap -pthread
ogst_server.o: ogst_server.c
	gcc -Wall -Werror -fPIC -c ogst_server.c -o ogst_server.o
socket/ogst_socket.o: socket/ogst_socket.c
	gcc -Wall -Werror -fPIC -c socket/ogst_socket.c -o socket/ogst_socket.o
