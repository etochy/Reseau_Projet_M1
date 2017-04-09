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

#include "vector.h"

#define TAILLE_MAX_NOM 256
#define TAILLE_BUF 256
#define TAILLE_PSEUDO 20

typedef struct sockaddr sockaddr;
typedef struct sockaddr_in sockaddr_in;
typedef struct hostent hostent;
typedef struct servent servent;

typedef enum{false , true} bool;

//-------Var Globales

int compteurTab = 0;
int * tabentier = {0};
int position=0;

vector v;

int sizePseudo = 10;
char **pseudo;

/*------------------------------------------------------*/

bool checkClient(int client){
    int i;
    //parcours du vecteur des clients
    for (i= 0; i < vector_count(&v); i++) {
        int x = (int)vector_get(&v, i);
        //si le client est présent
        if(x == client){
            //retourne vrai
            return true;
        }
    }
    return false;
}

void suppr(int c){
    int i;
    //parcours du vecteur des clients
    for (i = 0; i < vector_count(&v); i++) {
        int x = (int)vector_get_int(&v, i);
        // si le client est celui que l'on veut supprimer
        if(x == c){
            //le supprime
            vector_delete(&v, i);
            return;
        }
    }
}

void * ecoute (void * arg){

    char buffer[TAILLE_BUF];
    int longueur;

    int * tmp = (int *)arg;
    int client = tmp;
    bool t = true;
    while(t == true){

        if ((longueur = read(client, buffer, sizeof(buffer))) < 0) {
            printf("quit %s\n",buffer);
            //reinitialise le pseudo
            suppr(client);
            strcpy(pseudo[client], "anonimous");
            close(client);
        }


        else{

            buffer[longueur] ='\0';
            char buf2[TAILLE_BUF];
            strcpy(buf2, buffer);

            char p1[TAILLE_BUF] = {0};
            char p2[TAILLE_BUF] = {0};
            char p3[TAILLE_BUF] = {0};
            int compteur = 0;
            char * pch;
            pch = strtok (buf2," ");
            //split de la chaine de caracteres
            while (pch != NULL)
            {
                //si c'est le 1er mot
                if(compteur == 0){
                    //le copier dans p1
                    strcpy(p1, pch);
                    ++compteur;
                }
                //si c'est le 2eme mot
                else if(compteur == 1){
                    //le copier dans p2
                    strcpy(p2, pch);
                    ++compteur;
                }
                else{
                    //si 3eme mot ou plus, mettre dans p3
                    strcat(p3, pch);
                    strcat(p3, " ");
                    ++compteur;
                }
                pch = strtok (NULL, " ");
            }

    char quit[] = "/q"; // permet au client de quitter
    char list[] = "/l"; // liste les clients connectés
    char priv[] = "/p"; // envoi d'un msg privé a un au client
    char pseu[] = "/n"; // changement de pseudo
    char help[] = "/h"; // list commandes

    if(strcmp(buffer,quit) == 0){
        printf("quit \n");
        //reinitialise le pseudo
        suppr(client);
        strcpy(pseudo[client], "anonimous");
        close(client);
        t = false;
    }
    else if(strcmp(buffer,help) == 0){
        //envoi les commandes au client
        write(client,"Commandes :",13);
        write(client,"- /q : quit",12);
        write(client,"- /l : liste des clients",25);
        write(client,"- /p : private message",23);
        write(client,"- /n : changement de pseudo",28);
        write(client,"- /h : help",12);
    }
    else if(strcmp(buffer,list) == 0){
        printf("list \n");
        buffer[longueur] ='\0';

        int i;
        write(client,"Liste des clients connectés : ",32);
        //pour tous les clients clients connectés
        for (i = 0; i < vector_count(&v); i++) {
            //evite d'envoyer tous a la fois, plus de beug graphiques
            sleep(1);
            //envoi les pseudo
            write(client,pseudo[(int)vector_get_int(&v, i)],TAILLE_PSEUDO);
            printf("pseudo : %s\n", pseudo[(int)vector_get_int(&v, i)]);
        }
        //reset de p1 - p2 - p3
        memset(p1, 0, TAILLE_BUF);
        memset(p2, 0, TAILLE_BUF);
        memset(p3, 0, TAILLE_BUF);
    }
    else if(strcmp(p1,pseu) == 0){

        if(strcmp(p2,"") == 0){
            printf(" ------ pseudo sans pseudo ------  \n");
            //informe le client en cas de pseudo vide
            char er[] = "Changement de pseudo impossible\n";
            write(client,er,sizeof(er));
        }
        else if(strcmp(p3,"") == 0){
            printf(" ------ pseudo ------  \n");
            //reinit du pseudo a 0
            memset(pseudo[client], 0, TAILLE_PSEUDO);
            //enregistrement du nouveau pseudo
            strcpy(pseudo[client], p2);
            //informe le client
            char er[] = "Pseudo modifie avec succes";
            write(client,er,sizeof(er));

        }
        else{
    //Pseudo avec espace
            printf(" ------ pseudo avec espace ------  \n");
            char er[] = "Le pseudo de doit pas contenir d\'espaces, changement impossible\n";
            write(client,er,sizeof(er));

        }
        //reset de p1 - p2 - p3
        memset(p1, 0, TAILLE_BUF);
        memset(p2, 0, TAILLE_BUF);
        memset(p3, 0, TAILLE_BUF);
    }

    else if(strcmp(p1,priv) == 0){
        int i = 0;
        bool fail = true;
        for(i = 0; i < sizePseudo; ++i){
            //si le pseudo est valide
            if(strcmp(pseudo[i],p2) == 0){
    //bon pseudo
                char str[TAILLE_BUF + TAILLE_PSEUDO + 10]={0};
                //concat pseudo + message
                sprintf(str, "(prive)%s : ", pseudo[client]);
                strcat(str, p3);

                //envoi du message
                write(i,str,strlen(str)+1);
                fail = false;
            }
        }
        //si le pseudo n'est pas trouvé
        if(fail == true){
            char er[] = "Message prive impossible"; 
            //envoi du message d'erreur
            write(client,er,sizeof(er));
        }
        else{
            char str[TAILLE_BUF + TAILLE_PSEUDO + 10]={0};
            //concat pseudo + message
            sprintf(str, "(prive)%s : ", pseudo[client]);
            strcat(str, p3);
            //envoi du message
            write(client,str,strlen(str)+1);
        }
        //reset de p1 - p2 - p3
        memset(p1, 0, TAILLE_BUF);
        memset(p2, 0, TAILLE_BUF);
        memset(p3, 0, TAILLE_BUF);
    }


    else{
        buffer[longueur] ='\0';
        printf("message apres traitement : %s \n", buffer);
        printf("renvoi du message traite.\n");

        int i;
        char str[TAILLE_BUF]={0};
        //concatener pseudo + message
        sprintf(str, "%s : ", pseudo[client]);
        strcat(str, buffer);

        //envoi du message a tous le monde
        for (i = 0; i < vector_count(&v); i++) {
            write((int)vector_get_int(&v, i),str,strlen(str)+1);
        }

        //reset de p1 - p2 - p3
        memset(p1, 0, TAILLE_BUF);
        memset(p2, 0, TAILLE_BUF);
        memset(p3, 0, TAILLE_BUF);
    }

}


}
(void) arg;
pthread_exit(NULL);
}

void * renvoi (void * varV) {

    char buffer[TAILLE_BUF];
    int longueur;

    //recuperation
    int * tmp = (int *)varV;
    int client = tmp;

    //si le 
    if(checkClient(client) == false){
        //ajout du client dans le vecteur de sockets
        vector_add_int(&v, client);
    }

    //si le nombre de client est superieur a la taille du tableau de pseudo
    if(client >= sizePseudo){
        sizePseudo = sizePseudo*2;

        //realloc du tableau
        int *item = realloc(pseudo, sizePseudo);
        if(item){
            pseudo = item;
        }

//allocation et initialisation du tableau
        int i;
        for (i = (sizePseudo/2)-1; i < sizePseudo; i++){
            pseudo[i] = malloc(TAILLE_PSEUDO);
            sprintf(pseudo[i], "anonimous%d", i);
        }
    }

    pthread_exit(NULL);
}
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

// --- vector ---
    //initialisation du vectuer de sockets
    vector_init(&v);

// initialisation du tableau des pseudo
    pseudo = malloc(sizePseudo * sizeof(*pseudo));
    int i;
    for (i = 0; i < sizePseudo; i++){
        pseudo[i] = malloc(TAILLE_PSEUDO);
        sprintf(pseudo[i], "anonimous%d", i);
    }

// -------------------------------------------

//  int* psocket_descriptor = &socket_descriptor;
/* les infos recuperees sur le service de la machine */
    char
    machine[TAILLE_MAX_NOM+1];
/* nom de la machine locale */
    gethostname(machine,TAILLE_MAX_NOM);
/* recuperation du nom de la machine */

//init thread de nouvelle connexion
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
            if ((nouv_socket_descriptor = accept(socket_descriptor,
                (sockaddr*)(&adresse_client_courant),
                &longueur_adresse_courante))
                < 0) {
                perror("erreur : impossible d'accepter la connexion avec le client.");
                exit(1);
            }
/* traitement du message */

//--------------

            printf("nouveauClient : %d \n",nouv_socket_descriptor);
            char welcome[] = "Bienvenue sur le chan, /h pour help";
            //envoi du message de bienvenue
            write(nouv_socket_descriptor,welcome,strlen(welcome)+1);
            //creation du thread
            pthread_create (&threadSock, NULL, renvoi, (void*)nouv_socket_descriptor);
            //initialisation du thread d'ecoute
            pthread_t threadEcoute;
            //creation du thread
            pthread_create (&threadEcoute, NULL, ecoute, (void*)nouv_socket_descriptor);
            //attente du thread de nouvelle connexion
            pthread_join(threadSock, NULL);

        }
}