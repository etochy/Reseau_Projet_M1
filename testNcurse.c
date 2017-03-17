#include <ncurses.h>
#include <pthread.h>

#include "vector.h"
#define NB_LINE 100

void init_msgs(vector *v){
	for(int i=0; i<NB_LINE; ++i){
		vector_add_st(v," ");
	}
}

void afficheMsg(vector *v ,char* msg){
	for(int i=vector_count(v) -1 ; i>0; --i){
		vector_set_st(v,i,(char*)vector_get(v,i-1));
	}
	vector_set_st(v,0,msg);
}

void * void_reception(void * arg){

    while(1) {

        //afficheMsg(&v,buffer);
        /*
        for(int i=vector_count(&v) -1 ; i>0; --i){
            vector_set_st(&v,i,(char*)vector_get(&v,i-1));
        }
        vector_set_st(&v,0,buffer);*/
        
       // mvwprintw(haut, 10,2,"hey 1");

       // wrefresh(haut);

        //memset(buffer, 0, 256);
        printf("\n");

    }
    
// ------------------------------
    pthread_exit(NULL);
}

int main(void){
	WINDOW *haut, *bas;
	char *msg= "CHAT";
	char *sm= "";
	int taille= strlen(msg);
	char *str = malloc (256 * sizeof (char));

	vector v;
	vector_init(&v);
	
	init_msgs(&v);
	initscr();

	pthread_t th_reception;

	pthread_create(&th_reception, NULL, void_reception, NULL);

	while(1){
		clear();   
        attron(A_STANDOUT);
    	mvprintw(0, (COLS / 2) - (taille / 2), msg);
		attroff(A_STANDOUT);
		haut= subwin(stdscr, 3*LINES /4  -1, COLS, 1, 0);       
    	bas= subwin(stdscr, LINES / 4, COLS, 3*LINES / 4, 0); 
	    box(haut, ACS_VLINE, ACS_HLINE);
	    box(bas, ACS_VLINE, ACS_HLINE);
	    
	    mvwprintw(haut, 1, 1, "Serveur :");
	    for(int i=0; i<NB_LINE ; ++i){
	    	if((3*LINES)/4 -3 -i <= 1){
	    		//print nothing
	    	}else{
	 			mvwprintw(haut, (3*LINES)/4 -3 -i,2,(char*)vector_get(&v,i));
	    	}
	  	}
	    
	    mvwprintw(bas, 1, 1, "Message (140 caracteres max):");
		move(3*LINES/4 + 2, 1);
	 
	    wrefresh(haut);
		wrefresh(bas); 

		char *str = malloc (256 * sizeof (char));
		strcpy(str, "\0");
		getstr(str);
		if(strcmp(str, "\0")!=0) 
			afficheMsg(&v,str);
		}                
    
    getch();
    endwin();

	free(haut);
	free(bas);
	
	return 0;
}