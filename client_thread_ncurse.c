/*-----------------------------------------------------------
Client a lancer apres le serveur avec la commande :
client <adresse-serveur> <message-a-transmettre>
------------------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <ncurses.h>
#include <semaphore.h> 
#include "vector.h"

#define MAX_BUF 141
#define NB_LINE 100

typedef struct sockaddr 
sockaddr;
typedef struct sockaddr_in 
sockaddr_in;
typedef struct hostent 
hostent;
typedef struct servent 
servent;

sem_t *mutex;
// ---------- var global --------------
int rec = 0;
int i;
int 
socket_descriptor, 
/* descripteur de socket */
longueur; 
/* longueur d'un buffer utilisé */
sockaddr_in adresse_locale; 
/* adresse de socket local */
hostent * ptr_host; 
/* info sur une machine hote */
servent * ptr_service; 
/* info sur service */
char buffer[256];
char * prog; 
/* nom du programme */
char * host; 
/* nom de la machine distante */
char * mesg; 

char msg[MAX_BUF]={0};

WINDOW *haut, *bas;

char ** listBif;
// ------------------------------------

void init_msgs(){

    listBif = malloc(100 * sizeof(*listBif));
    int i;
    for (i = 0; i < 100; i++){
        listBif[i] = malloc(256);
        sprintf(listBif[i], "%s", " ");
    }
}

void afficheMsg(char* msg){

    for(i=99 ; i>0; --i){
        sprintf(listBif[i],"%s", listBif[i-1]);
    }

    sprintf(listBif[0],"%s",msg);

}

void purger(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {}
}

void clean (char *chaine)
{
    char *p = strchr(chaine, '\n');
    if (p)
    {
        *p = 0;
    }
    else
    {
        purger();
    }
}

//----------------------------
static volatile int keepRunning = 1;

void intHandler(int dummy) {
    keepRunning = 0;
    write(socket_descriptor, "/q",10);
    printf("connexion avec le serveur fermee, fin du programme.\n");
    close(socket_descriptor);
    quitter();
}

void quitter(){
    keepRunning = 0;

    endwin();
    printf("Appuyer sur entrer pour fermer \n");
    return 0;
}

void * void_envoi(void * arg){
// --------------- Thread envoi ----------------
    char quit[40] = "/q";
    char * tmp = (char *)arg;

    if(keepRunning == 1){
        /* envoi du message vers le serveur */
        if ((write(socket_descriptor, tmp, strlen(tmp))) <= 0) {
            perror("erreur : impossible d'ecrire le message destine au serveur.");
            quitter();
            (void) arg;
            pthread_exit(NULL);
        }else{
            pthread_exit(NULL);
        }

        if(strcmp(tmp,quit) == 0){
            printf("connexion avec le serveur fermee, fin du programme.\n");
            close(socket_descriptor);
            quitter();
            (void) arg;
            pthread_exit(NULL);       
        }
    }
    else{
        printf("connexion avec le serveur fermee, fin du programme.\n");
        close(socket_descriptor);
        quitter();
        (void) arg;
        pthread_exit(NULL);       
    }

    /* mise en attente du prgramme pour simuler un delai de transmission */
    //sleep(3);
    //printf("message envoye au serveur. \n");
    // ------------------------------
    (void) arg;
    pthread_exit(NULL);
}

void affichageNcurses(){
    
    pthread_t th_envoi;
    char *msg2= "CHAT";
    char *sm= "";
    int taille= strlen(msg2);
    int lec = 0;

    int b = -1;

    while(keepRunning){
        if(rec == 1){
            clear();

            haut= subwin(stdscr, 3*LINES /4  -1, COLS, 1, 0);       
            box(haut, ACS_VLINE, ACS_HLINE);
            mvwprintw(haut, 1, 1, "Serveur :");

            for(i=0; i<NB_LINE ; ++i){
                if((3*LINES)/4 -3 -i <= 1){
                //print nothing
                }else{

                    mvwprintw(haut, (3*LINES)/4 -3 -i,2,listBif[i]);
                }
            }   
            attron(A_STANDOUT);
            mvprintw(0, (COLS / 2) - (taille / 2), msg2);
            attroff(A_STANDOUT);       
            bas= subwin(stdscr, LINES / 4, COLS, 3*LINES / 4, 0); 

            nodelay(bas, TRUE);
            nodelay(haut, TRUE);

            box(bas, ACS_VLINE, ACS_HLINE);

            mvwprintw(bas, 1, 1, "Message (140 caracteres max):");
            move(3*LINES/4 + 2, 1);
            wrefresh(bas); 
            b=fork();

            if ( b== 0){

                memset(msg, 0, 256);
                char msgBis[256];
                if(lec == 0){
                    lec =1;
                    getnstr(msgBis,140);
                    if(strlen(msgBis) > 0) {
                        wrefresh(bas);
                        pthread_create(&th_envoi, NULL, void_envoi, (void*)msgBis);
                        if(pthread_join(th_envoi,NULL)){
                            perror("affiche");
                            lec =0;
                            quitter();
                            exit(0);
                        }
                    }
                    lec = 0;    
                }

                exit(0);
                }

            rec = 0;
        }
        
    }

}

void afficheTest(){

    wclear(haut);

    haut= subwin(stdscr, 3*LINES /4  -1, COLS, 1, 0);       
    box(haut, ACS_VLINE, ACS_HLINE);
    mvwprintw(haut, 1, 1, "Serveur test :");

    for(i=0; i<NB_LINE ; ++i){
        if((3*LINES)/4 -3 -i <= 1){
                //print nothing
        }else{
            mvwprintw(haut, (3*LINES)/4 -3 -i,2,listBif[i]);
        }
    }   

    move(3*LINES/4 + 2, 1);
}

void * void_reception(void * arg){
    /* lecture de la reponse en provenance du serveur */
    while(keepRunning){
    if((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
        afficheMsg(buffer);
  
        memset(buffer, 0, 256);
        rec = 1;
    }

    }

// ------------------------------
    (void) arg;
    pthread_exit(NULL);
}

int main(int argc, char **argv) {

// --------------------------------------------------------------------
    sem_init(&mutex, 0, 1);
    init_msgs();
    initscr();

    /* message envoyé */
    if (argc != 2) {
        perror("usage : client <adresse-serveur>");

        mvwprintw(haut, 10,2,"hey 4");
        quitter();
    }
    prog = argv[0];
    host = argv[1];

    printf("nom de l'executable : %s \n", prog);
    printf("adresse du serveur  : %s \n", host);

 //   printf("message envoye      : %s \n", mesg);

    if ((ptr_host = gethostbyname(host)) == NULL) {
        perror("erreur : impossible de trouver le serveur a partir de son adresse.");

        mvwprintw(haut, 10,2,"hey 4");
        quitter();
    }

    /* copie caractere par caractere des infos de ptr_host vers adresse_locale */
    bcopy((char*)ptr_host->h_addr, (char*)&adresse_locale.sin_addr, ptr_host->h_length);
    adresse_locale.sin_family = AF_INET; /* ou ptr_host->h_addrtype; */
    /* 2 facons de definir le service que l'on va utiliser a distance (le même que sur le serveur) */
    /* (commenter l'une ou l'autre des solutions) */
    /*-----------------------------------------------------------*/
    /* SOLUTION 1 : utiliser un service existant, par ex. "irc" */
    /*
    if ((ptr_service = getservbyname("irc","tcp")) == NULL) {
perror("erreur : impossible de recuperer le numero de port du service desire.");
exit(1);
    }
    adresse_locale.sin_port = htons(ptr_service->s_port);
    /*-----------------------------------------------------------*/
    /* SOLUTION 2 : utiliser un nouveau numero de port */
    adresse_locale.sin_port = htons(5000);
    /*-----------------------------------------------------------*/
    printf("numero de port pour la connexion au serveur : %d \n", ntohs(adresse_locale.sin_port));
    /* creation de la socket */
    if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("erreur : impossible de creer la socket de connexion avec le serveur.");;
        quitter();
    }
    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
        perror("erreur : impossible de se connecter au serveur.");
        quitter();
    }
    printf("connexion etablie avec le serveur. \n");
    if(keepRunning){
        pthread_t th_reception;
        pthread_t th_affichage;

        bas= subwin(stdscr, LINES / 4, COLS, 3*LINES / 4, 0); 
        wrefresh(haut);
        wrefresh(bas); 

        struct sigaction act;
        act.sa_handler = intHandler;
        sigaction(SIGINT, &act, NULL);

        pthread_create(&th_reception, NULL, void_reception, NULL);
        //pthread_create(&th_affichage, NULL, affichageNcurses, NULL);
        affichageNcurses();
        // --------------------------------

        if(pthread_join(th_reception,NULL)){
            perror("affiche");
                //mvwprintw(haut, 10,2,"hey 4");
            quitter();
        }
        
    }
    quitter();

    return 0;
}