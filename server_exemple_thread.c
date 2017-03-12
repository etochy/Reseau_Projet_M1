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
//char pseudo[sizePseudo][20] = {0};
char **pseudo;

/*
v->size *= 2;
v->data = realloc(v->data, sizeof(void*) * v->size);
*/
/* --- TODO---
* Gestion des erreurs
* Realloc des erreurs
*
*/

//--------------

/*------------------------------------------------------*/

bool checkClient(int client){
    int i;
    for (i= 0; i < vector_count(&v); i++) {
        int x = (int)vector_get(&v, i);
        if(x == client){
            return true;
        }
    }
    return false;
}

void suppr(int c){
    int i;
    for (i = 0; i < vector_count(&v); i++) {
        int x = (int)vector_get_int(&v, i);
        if(x == c){
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
// int i = 0;
//if(compteurTab != 0){
//for(i = 0; i < compteurTab; ++i){
        if ((longueur = read(client, buffer, sizeof(buffer))) < 0) {return;}
        else{

            buffer[longueur] ='\0';
            char buf2[TAILLE_BUF];
            strcpy(buf2, buffer);

            char p1[TAILLE_BUF] = {0};
            char p2[TAILLE_BUF] = {0};
            char p3[TAILLE_BUF] = {0};
            int compteur = 0;
            char * pch;
            //printf("buffer avant : %s\n", buffer);
            //printf ("Splitting string \"%s\" into tokens:\n",buf2);
            pch = strtok (buf2," ");

            while (pch != NULL)
            {
//printf ("%s\n",pch);
                if(compteur == 0){
                    strcpy(p1, pch);
                    ++compteur;
                }
                else if(compteur == 1){
                    strcpy(p2, pch);
                    ++compteur;
                }
                else{
                    strcat(p3, pch);
                    strcat(p3, " ");
                    ++compteur;
                }
                pch = strtok (NULL, " ");
            }
            printf("p1 : %s\n", p1);
            printf("p2 : %s\n", p2);
            printf("p3 : %s\n", p3);
            printf("buffer apres : %s\n", buffer);

char quit[] = "/q"; // permet au client de quitter
char list[] = "/l"; // liste les clients connectés
char priv[] = "/p"; // envoi d'un msg privé a un au client
char pseu[] = "/n"; // changement de pseudo
char help[] = "/h"; // list commandes

if(strcmp(buffer,quit) == 0){
    printf("quit \n");
    suppr(client);
    strcpy(pseudo[client], "anonimous");
//vector_set(&pseudo, client, "");
    close(client);
    t = false;
}
else if(strcmp(buffer,list) == 0){
    printf("list \n");
    buffer[longueur] ='\0';
/* mise en attente du programme pour simuler un delai de transmission */
//sleep(1);
    int i;
    for (i = 0; i < vector_count(&v); i++) {
        printf("client : \n");
        printf("client %d : %s\n",(int)vector_get_int(&v, i), pseudo[(int)vector_get_int(&v, i)]);
        printf("size : %d\n", sizeof(pseudo[(int)vector_get_int(&v, i)]));
// write(client,pseudo[(int)vector_get_int(&v, i)],sizeof(pseudo[(int)vector_get_int(&v, i)]));
        write(client,pseudo[(int)vector_get_int(&v, i)],TAILLE_PSEUDO);
        write(client,"\n",1);
/*char str[50];
sprintf(str, "%d", (int)vector_get_int(&v, i));
write(client,str,strlen(str)+1);*/
// write(client,vector_get(&pseudo,(int)vector_get_int(&v, i)),21);
    }
    memset(p1, 0, TAILLE_BUF);
    memset(p2, 0, TAILLE_BUF);
    memset(p3, 0, TAILLE_BUF);
}
else if(strcmp(p1,pseu) == 0){

    if(strcmp(p3,"") == 0){
        printf(" ------ pseudo ------  \n");
        printf("p2 : %s\n", p2);

//memset(pseudo[client], 0, sizeof(pseudo[client]));
        memset(pseudo[client], 0, TAILLE_PSEUDO);
//vector_set_ps(&pseudo, client, p2);
        strcpy(pseudo[client], p2);
        printf("size : %d\n",sizeof(p2) );

//pseudo[client][sizeof(p2)+1]='\0';
        printf("pseudo[i] : %s\n", pseudo[client]);
//int i;
/*
for (i = 0; i < vector_count(&pseudo); i++) {
char str[50];
printf("test : %s ", (char*)vector_get(&pseudo, i));
}
*/
    }
    else{
        //Pseudo avec espace
        printf(" ------ pseudo avec espace ------  \n");
        char er[] = "Le pseudo de doit pas contenir d\'espaces, changement impossible\n";
        write(client,er,sizeof(er));

    }


    memset(p1, 0, TAILLE_BUF);
    memset(p2, 0, TAILLE_BUF);
    memset(p3, 0, TAILLE_BUF);
}

else if(strcmp(p1,priv) == 0){
    printf("msg prive \n");
    int i = 0;
    printf("taille : %d\n",sizePseudo );
    bool fail = true;
    for(i = 0; i < sizePseudo; ++i){
        printf("i : %d\n", i);
        if(strcmp(pseudo[i],p2) == 0){
//bon pseudo
            char str[TAILLE_BUF + TAILLE_PSEUDO + 10]={0};
            sprintf(str, "(prive)%s : ", pseudo[client]);
            strcat(str, p3);
            write(i,str,strlen(str)+1);
            fail = false;
        }
    }
    if(fail == true){
        char er[] = "Message prive impossible"; 
        write(client,er,sizeof(er));
    }
    else{
        char str[TAILLE_BUF + TAILLE_PSEUDO + 10]={0};
        sprintf(str, "(prive)%s : ", pseudo[client]);
        strcat(str, p3);
        write(client,str,strlen(str)+1);
    }
    memset(p1, 0, TAILLE_BUF);
    memset(p2, 0, TAILLE_BUF);
    memset(p3, 0, TAILLE_BUF);
}


else{
    buffer[longueur] ='\0';
    printf("message apres traitement : %s \n", buffer);
    printf("renvoi du message traite.\n");
/* mise en attente du programme pour simuler un delai de transmission */
// sleep(1);
    int i;
    char str[TAILLE_BUF]={0};
    sprintf(str, "%s : ", pseudo[client]);
    strcat(str, buffer);
    for (i = 0; i < vector_count(&v); i++) {
        write((int)vector_get_int(&v, i),str,strlen(str)+1);
    }

    memset(p1, 0, TAILLE_BUF);
    memset(p2, 0, TAILLE_BUF);
    memset(p3, 0, TAILLE_BUF);
}

}

}
//}
//}
//close(client);
(void) arg;
pthread_exit(NULL);
}

void * renvoi (void * varV) {
//    struct varRenvoi *var = (struct varRenvoi*)varV;

    char buffer[TAILLE_BUF];
    int longueur;

    int * tmp = (int *)varV;
    int client = tmp;

    printf("client : %d\n", client);
    if(checkClient(client) == false){
        vector_add_int(&v, client);
    }

    if(client >= sizePseudo){
        sizePseudo = sizePseudo*2;
        printf("avant 1er realloc");

        pseudo = realloc(pseudo, 20);
        printf("apres 1er realloc");
//Allocate memory for the new string item in the array
        int i;
        for (i = (sizePseudo/2)-1; i < sizePseudo; i++){
            printf("if+realloc : %d", i);
            pseudo[i] = malloc(TAILLE_PSEUDO+1);
            sprintf(pseudo[i], "anonimous%d", i);
        }
    }

    int i;
    printf("first round:\n");
    for (i = 0; i < vector_count(&v); i++) {
        printf("client : %d -> %d\n", i, (int)vector_get_int(&v, i));
    }

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

// --- vector ---

    vector_init(&v);
//vector_init_ps(&pseudo);

    pseudo = malloc(sizePseudo * sizeof(*pseudo));
    int i;
    for (i = 0; i < sizePseudo; i++){
        pseudo[i] = malloc(TAILLE_PSEUDO+1);
        sprintf(pseudo[i], "anonimous%d", i);
    }

/*
int m;
for(m=0;m<10;++m){
char str[10];
char num[6];
strcpy(str,  "anonimous");
sprintf(num, "%d", m);
strcat(str, num);
strcpy(pseudo[m], str);
}
*/
/*
for(m =0 ; m<10;++m){
printf("pseudo %d : %s\n",m, pseudo[m]);
}
*/
/*
for(m=0;m<10;++m){
printf("pseudo : %d = %s\n",m,pseudo[m] );
}
*/
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
        char welcome[] = "Bienvenue sur le chan";
        write(nouv_socket_descriptor,welcome,strlen(welcome)+1);

        pthread_create (&threadSock, NULL, renvoi, (void*)nouv_socket_descriptor);
        pthread_t threadEcoute;
        pthread_create (&threadEcoute, NULL, ecoute, (void*)nouv_socket_descriptor);
        pthread_join(threadSock, NULL);
// pthread_join(threadEcoute, NULL);
    }
}