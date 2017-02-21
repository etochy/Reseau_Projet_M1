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

typedef struct sockaddr 
sockaddr;
typedef struct sockaddr_in 
sockaddr_in;
typedef struct hostent 
hostent;
typedef struct servent 
servent;

typedef enum{false , true} bool;

// ---------- var global --------------

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
char msg[40] = {0};

// ------------------------------------
// ------ fgets : ------------

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


void * void_reception(void * arg){
// --------------- Thread reception ----------------
   // printf("th_reception \n");
    /* lecture de la reponse en provenance du serveur */

    while((longueur = read(socket_descriptor, buffer, sizeof(buffer))) > 0) {
        printf("\nserveur : \n");
        write(1,buffer,longueur);
        printf("\nMessage : \n");
    }
    
// ------------------------------
    pthread_exit(NULL);
}

void * void_envoi(void * args){
// --------------- Thread envoi ----------------
    char quit[40] = "/q";

    /* envoi du message vers le serveur */
    if ((write(socket_descriptor, msg, strlen(msg))) < 0) {
        perror("erreur : impossible d'ecrire le message destine au serveur.");
        exit(1);
    }

    if(strcmp(msg,quit) == 0){
        printf("connexion avec le serveur fermee, fin du programme.\n");
        close(socket_descriptor);
        exit(0);        
    }

    /* mise en attente du prgramme pour simuler un delai de transmission */
    //sleep(3);
    //printf("message envoye au serveur. \n");
    // ------------------------------
    pthread_exit(NULL);
}

int main(int argc, char **argv) {

/* message envoyé */
    if (argc != 2) {
        perror("usage : client <adresse-serveur>");
        exit(1);
    }
    prog = argv[0];
    host = argv[1];
    //mesg = argv[2];

    printf("nom de l'executable : %s \n", prog);
    printf("adresse du serveur  : %s \n", host);

 //   printf("message envoye      : %s \n", mesg);

    if ((ptr_host = gethostbyname(host)) == NULL) {
        perror("erreur : impossible de trouver le serveur a partir de son adresse.");
        exit(1);
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
        perror("erreur : impossible de creer la socket de connexion avec le serveur.");
        exit(1);
    }
    /* tentative de connexion au serveur dont les infos sont dans adresse_locale */
    if ((connect(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
        perror("erreur : impossible de se connecter au serveur.");
        exit(1);
    }
    printf("connexion etablie avec le serveur. \n");

    // TODO :  init thread

    pthread_t th_reception;
    pthread_t th_envoi;
    //printf("init thread \n");


    pthread_create(&th_reception, NULL, void_reception, NULL);
    while(1){
        //printf("\nMessage : \n");
        fgets(msg, sizeof msg, stdin);
        clean(msg);
      //  scanf("%s",msg);
        printf("\n");
        pthread_create(&th_envoi, NULL, void_envoi, NULL);

        if(pthread_join(th_envoi, NULL)){
            perror("join");
            return EXIT_FAILURE;
        }
    }

    if(pthread_join(th_reception, NULL)){
        perror("join");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}