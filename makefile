all : client_ncurse client_text server_thread

testNcurse : testNcurse.c
	gcc -o testNcurse testNcurse.c vector.c -lncurses -lpthread

client_ncurse : client_thread_ncurse.c
	gcc -o clientApp_ncurse client_thread_ncurse.c vector.c -lpthread -lncurses

client_text : client_thread_ncurse.c
	gcc -o clientApp_text client_thread_text.c vector.c -lpthread

server_thread : server_thread.c vector.h vector.c
	gcc -o serverApp_thread server_thread.c vector.c -lpthread

vector : vector.h vector.c
	gcc -o testV vector.c

clean:
	rm -rf *.o