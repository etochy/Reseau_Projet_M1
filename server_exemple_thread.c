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
/*------------------------------------------------------*/
void * renvoi (void * soc, void * compteurTab, void * position) {
    char buffer[256];
    int longueur;
    int **tmp = (int **)soc;
    int * tmp2 = (int *)compteurTab;
    int compteur = tmp2;
    tmp2 = (int *)position;
    int pos = tmp2;

    int client = tmp[pos];
    if ((longueur = read(client, buffer, sizeof(buffer))) <= 0) 
        return;
    printf("message lu : %s \n", buffer);
//    buffer[0] = 'R';
//    buffer[1] = 'E';
    buffer[longueur] = '#';
    buffer[longueur+1] ='\0';
    printf("message apres traitement : %s \n", buffer);  
    printf("renvoi du message traite.\n");
    /* mise en attente du programme pour simuler un delai de transmission */
    sleep(3);
    write(client,buffer,strlen(buffer)+1);    
    printf("message envoye. \n");       
    return;
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

    int compteurTab = 1;
    int * tabentier = NULL ,*temp;
    int position=0;

/* Création d'un tableau de 1 entiers */
    
    tabentier = realloc ( tabentier , 1 * sizeof(int) );

    if ( tabentier == NULL )
    {
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

 pthread_t thread1;

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

/* Redimensionnement*/

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

/* Ajout d'un élement */

 tabentier[compteurTab -1 ] = nouv_socket_descriptor;

 for ( int i = 0 ; i < compteurTab ; i++ )
 {
     printf(" tabentier[%d] = %d \n", i , tabentier[i] );
 }

//--------------

 pthread_create (&thread1, NULL, renvoi, (int**) &tabentier,(int *)&compteurTab, (int *)position);
 printf("reception d'un message. thread1\n");
 pthread_join(thread1, NULL);
 close(nouv_socket_descriptor);
}    
}