all : client server

client : client_exemple.c
	gcc -o clientApp client_exemple.c

server : server_exemple.c
	gcc -o serverApp server_exemple.c