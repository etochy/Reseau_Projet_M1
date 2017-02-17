/*----------------------------------------------
Serveur à lancer avant le client
------------------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <linux/types.h> 
/* pour les sockets */
#include <sys/socket.h>
#include <netdb.h> 
/* pour hostent, servent */
#include <string.h> 
/* pour bcopy, ... */  
#include <pthread.h>
/**/

#define TAILLE_MAX_NOM 256
typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef enum{false , true} bool;

//-------Var Globales

int compteurTab = 0;
int * tabentier = {0} ,*temp;
int position=0;

//--------------

/*
struct varRenvoi{
void * soc;
void * compteurTab;
void * position;
};
*/
/*------------------------------------------------------*/

bool checkClient(int client){

    int i;
    for ( i = 0 ; i < compteurTab ; i++ )
    {
        if(tabentier[i] == client){
            return true;
        }
    }

    return false;
}

void * ecoute (void * arg){

    char buffer[256];
    int longueur;

    int * tmp = (int *)arg;
    int client = tmp;

    while(1){
       // int i = 0;
        //if(compteurTab != 0){
            //for(i = 0; i < compteurTab; ++i){
        if ((longueur = read(client, buffer, sizeof(buffer))) < 0) {return;}
        else{
            printf("message lu : %s \n", buffer);

            buffer[longueur] ='\0';

            char quit[] = "/q";
            char list[] = "/l";
            if(strcmp(buffer,quit) == 0){
                printf("quit \n");
                close(client);
                return;
            }
            else if(strcmp(buffer,list) == 0){
                printf("list \n");
                buffer[longueur] ='\0';
/* mise en attente du programme pour simuler un delai de transmission */
                //sleep(1);
                int j;
                for ( j = 0 ; j < compteurTab ; j++ ){

                    printf("avant");
                    char str[40];
                    sprintf(str, "%d", tabentier[j]);
                    printf("string : %s", str);

                    write(client,str,strlen(str)+1);    
                    printf("message envoye. \n");  
                }    
            }
            else{
                buffer[longueur] ='\0';
                printf("message apres traitement : %s \n", buffer);  
                printf("renvoi du message traite.\n");
/* mise en attente du programme pour simuler un delai de transmission */
               // sleep(1);

                int j;
                for ( j = 0 ; j < compteurTab ; j++ ){
                    write(tabentier[j],buffer,strlen(buffer)+1);    
                    printf("message envoye. \n");  
                }    
            }

        }

    }
        //}
    //}
    close(client);
    (void) arg;
    pthread_exit(NULL);
}

void * renvoi (void * varV) {
//    struct varRenvoi *var = (struct varRenvoi*)varV;

    char buffer[256];
    int longueur;


/*
int **tmp = (int **)(*var).soc;
int * tmp2 = (int *)(*var).compteurTab;
int compteur = tmp2;
tmp2 = (int *)(*var).position;
int pos = tmp2;
int client = tmp[pos];
*/
    int * tmp = (int *)varV;
    int client = tmp;

//---------------------------------------------------------------------------------------------------

/* Redimensionnement*/
/*   if(checkClient(client) == true){
// di nothing
    }
    else{*/
    temp = realloc (tabentier, compteurTab+1 * sizeof(int ) );
    compteurTab ++;

    if ( temp == NULL )
    {
        fprintf(stderr,"Reallocation impossible");
        free(tabentier);
        exit(EXIT_FAILURE);
    }
    else
    {
        tabentier = temp;
    }

    tabentier[compteurTab -1 ] = client;
    //}

/*
    printf("client : %d \n",client);
    if ((longueur = read(client, buffer, sizeof(buffer))) <= 0) 
        return;
    printf("message lu : %s \n", buffer);

    buffer[longueur] ='\0';

    char quit[] = "/q";
    if(strcmp(buffer,quit) == 0){
        printf("quit \n");
        close(client);
        return;
// ---------------------------------- Enleve client dans tab

/*
temp = realloc (tabentier, compteurTab-1 * sizeof(int ) );
compteurTab --;

if ( temp == NULL )
{
fprintf(stderr,"Reallocation impossible");
free(tabentier);
exit(EXIT_FAILURE);
}
else
{
int i;
int j = 0;
for(i = 0; i < compteurTab; ++i){
if(tabentier[i] == client){
j++;
}
else{
temp[i] = tabentier[j];
}
++j;
}
tabentier = temp;
}
*/
// ----------------------------------
    //}

//    buffer[0] = 'R';
//    buffer[1] = 'E';
    /*
    buffer[longueur] ='\0';
    printf("message apres traitement : %s \n", buffer);  
    printf("renvoi du message traite.\n");*/
/* mise en attente du programme pour simuler un delai de transmission */
   /* sleep(3);

    int i;
    for ( i = 0 ; i < compteurTab ; i++ ){
        write(tabentier[i],buffer,strlen(buffer)+1);    
        printf("message envoye. \n");  
    }*/
    pthread_exit(NULL);
}
/*------------------------------------------------------*/

/*------------------------------------------------------*/
main(int argc, char **argv) {
    int 
    socket_descriptor, 
/* descripteur de socket */
    nouv_socket_descriptor, 
/* [nouveau] descripteur de socket */
    longueur_adresse_courante; 
/* longueur d'adresse courante d'un client */
    sockaddr_in 
    adresse_locale, 
/* structure d'adresse locale*/
    adresse_client_courant; 
/* adresse client courant */
    hostent*
    ptr_hote; 
/* les infos recuperees sur la machine hote */
    servent*
    ptr_service; 

// ------------------------------------------- TAB

/* Création d'un tableau de 1 entiers */

    tabentier = realloc ( tabentier , 1 * sizeof(int) );

    if ( tabentier == NULL ){
        fprintf(stderr,"Allocation impossible");
        exit(EXIT_FAILURE);
    }

    tabentier[0] = 0;

// -------------------------------------------

//  int* psocket_descriptor = &socket_descriptor;
/* les infos recuperees sur le service de la machine */
    char 
    machine[TAILLE_MAX_NOM+1]; 
/* nom de la machine locale */
    gethostname(machine,TAILLE_MAX_NOM);
/* recuperation du nom de la machine */

    pthread_t threadSock;

/* recuperation de la structure d'adresse en utilisant le nom */
    if ((ptr_hote = gethostbyname(machine)) == NULL) {
        perror("erreur : impossible de trouver le serveur a partir de son nom.");
        exit(1);
    }    
/* initialisation de la structure adresse_locale avec les infos recuperees */
/* copie de ptr_hote vers adresse_locale */
    bcopy((char*)ptr_hote->h_addr, (char*)&adresse_locale.sin_addr, ptr_hote->h_length);
    adresse_locale.sin_family
    = ptr_hote->h_addrtype; 
/* ou AF_INET */
    adresse_locale.sin_addr.s_addr
    = INADDR_ANY; 
/* ou AF_INET */
/* 2 facons de definir le service que l'on va utiliser a distance */
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
    printf("numero de port pour la connexion au serveur : %d \n", 
ntohs(adresse_locale.sin_port) /*ntohs(ptr_service->s_port)*/);
/* creation de la socket */
        if ((socket_descriptor = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("erreur : impossible de creer la socket de connexion avec le client.");
            exit(1);
        }
/* association du socket socket_descriptor à la structure d'adresse adresse_locale */
        if ((bind(socket_descriptor, (sockaddr*)(&adresse_locale), sizeof(adresse_locale))) < 0) {
            perror("erreur : impossible de lier la socket a l'adresse de connexion.");
            exit(1);
        }
/* initialisation de la file d'ecoute */
        listen(socket_descriptor,5);
/* attente des connexions et traitement des donnees recues */
        for(;;) {

            longueur_adresse_courante = sizeof(adresse_client_courant);
/* adresse_client_courant sera renseignée par accept via les infos du connect */
            if ((nouv_socket_descriptor = 
                accept(socket_descriptor, 
                    (sockaddr*)(&adresse_client_courant),
                    &longueur_adresse_courante))
                < 0) {
                perror("erreur : impossible d'accepter la connexion avec le client.");
            exit(1);
        }
/* traitement du message */

//--------------
/*
struct varRenvoi *var;

var = malloc(sizeof(struct varRenvoi));
(*var).soc = (int**) &tabentier;
(*var).compteurTab = (int *)&compteurTab;
(*var).position = (int *)position;
*/
        printf("nouveauClient : %d \n",nouv_socket_descriptor);
        char welcome[] = "Bienvenue sur le chan";
        write(nouv_socket_descriptor,welcome,strlen(welcome)+1);

        pthread_create (&threadSock, NULL, renvoi, (void*)nouv_socket_descriptor);
        pthread_t threadEcoute;
        pthread_create (&threadEcoute, NULL, ecoute, (void*)nouv_socket_descriptor);
        pthread_join(threadSock, NULL);
       // pthread_join(threadEcoute, NULL);
    }   
}