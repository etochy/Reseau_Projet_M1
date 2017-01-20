all : client server

client : client.c
	g++ -std=c++11 -pthread -o hello main.cpp semaphore.o

server : semaphore.cpp semaphore.h
	g++ -std=c++11 -pthread -c semaphore.cpp