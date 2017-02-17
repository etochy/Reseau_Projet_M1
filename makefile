all : client server client_fork client_thread server_fork server_thread

client : client_exemple.c
	gcc -o clientApp client_exemple.c

server : server_exemple.c
	gcc -o serverApp server_exemple.c

client_fork : client_exemple_fork.c
	gcc -o clientApp_fork client_exemple_fork.c

client_thread : client_exemple_thread.c
	gcc -o clientApp_thread client_exemple_thread.c -lpthread

server_fork : server_exemple_fork.c
	gcc -o serverApp_fork server_exemple_fork.c

server_thread : server_exemple_thread.c
	gcc -o serverApp_thread server_exemple_thread.c -lpthread

clean:
	rm -rf *.o