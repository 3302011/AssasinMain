#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
# define END 3 


//////////////////////////////////////////////////////////////// VARIABLES GLOBALES ///////////////////////////////////////////////////////////////////////////

int portno;
int nbj;  // compteur joueur == nbj pour commencer
int nbespions;
int meneurCourant;
int compteurJoueurs; // pour compter les joueurs
int compteurMissions; // pour savoir à quelle mission on est
int compteurVotes; // combien de votes ont été éffectués
int compteurReussites; // combien de vote reussite
int compteurRebelles; // combien de rebelles
int compteurEspions;  // combien d'Espions
int participantsMissions[5]={2,2,3,3,2};	// pour savoir combien de participant à la mission
int Nbparticipants = 1;
char serverbuffer[256];

char com;
char adrip[20];
int dportno;
char name[20];
char team[10];
//int equipe[10];


/* PRINCIPE :
		attente 'C'
		remplir la structure
		renvoyer le joueur aux autres joueurs
		si compteur == nbj
		fsmstate = 101 ;

*/





/////////////////////////////////////////////////////////////   STRUCTURE ET FONCTION DE CONCATENATION ///////////////////////////////////////////////////////////
struct joueur
{
	char nom[20];
	char ipaddress[20];
	int portno;
	int equipe; // 0=pas dans equipe, 1=dans equipe
	int role; // 0=rebelle, 1=espion
	int vote; // 0=refus, 1=accept
	int reussite; // 0=echec, 1=reussite
} tableauJoueurs[5];


int roles[5];


char *concat_string(char *s1,char *s2)
{
  char *s3=NULL;
     s3=(char *)malloc((strlen(s1)+strlen(s2))*sizeof(char));
     strcpy(s3,s1);
     strcat(s3,s2);
     return s3;
     }



//////////////////////////////////////////////////////  FONCTIONS D'INITIALISATION ET D'ENVOI DE MESSAGE /////////////////////////////////////////////////////////

void sendMessage(int j, char *mess);
void broadcast(char *message);

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void initRoles()
{
  srand(time(NULL));
  int i=rand()%5; 
  int j=rand()%5; 
  int k; 

  while(i==j)
    {
      j=rand()%5; 
    }

  for(k=0; k<5; k++)
    {
      if (k==i || k==j)
	{
	  roles[k]=1; 
	  tableauJoueurs[k].role = 1;

	}

      else {
	roles[k]=0; 
	tableauJoueurs[k].role=0; 
      }
    }
}


void sendRoles()

{
  int i; 
  int tab[2]; 
  int k=0; 
  char mess[100];
  char mess2[100]; 
  char *messr = "6";
  
  for(i=0;i<5; i++)
    {
      tableauJoueurs[i].role=roles[i]; //va affecter les roles 
      
      if(roles[i]==0)
	sendMessage(i,messr); 

      else if(roles[i]==1)
	{
	  tab[k]=i; //on stock les espions dans un tableau
	  k++; 
	}
    } 
  sprintf(mess,"7 %d",tab[0]);
  sprintf(mess2,"7 %d",tab[1]);


      sendMessage(tab[0],mess); //on envoie 7 car 7=espion
      sendMessage(tab[0],mess2);
      sendMessage(tab[1],mess);   
      sendMessage(tab[1],mess2);
}


void broadcast(char *message)
{
        int i;

        printf("broadcast %s\n",message);
        for (i=0;i<compteurJoueurs;i++)
                sendMessage(i,message);
}



void sendMeneur() // Le meneur tourne et on commence à 0
{
  char mess[100]; 
  sprintf(mess,"8 %d",participantsMissions[compteurMissions]);
  sendMessage(compteurMissions,mess);


}


void sendEquipe()
{

  //1rst etape le meneur repond dans le fichier joueur
	// 2nd etape on recupere les infos 
   //3eme etape on demande l'avis des gens
  //4eme etape si ils sont pas casse couilles ça devrait aller sinon... on demande une recomposition de l'equipe 

  // On envoie un message au meneur :
	char* mess; 
	sscanf(mess,"%s",serverbuffer);  
	//char* nom= tableauJoueurs[(int*)mess].nom; 

	broadcast(mess); 

}



void *server(void *ptr) //represente un serveur
{
     int sockfd, newsockfd; 
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;  //correspond a un appel systeme 
     int n; // socket : cree une file vide - bind: essaye de ce co au port - listen: le socket ecoute max 5 connections 

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, //on se connecte au port si il est correct 
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");
     listen(sockfd,5);  // si plus de 5 connections alors 6 eme next 
     clilen = sizeof(cli_addr);
     while (1) // serveur qui s'arrête jamais
     {
       newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);    //appel system bloquant, tant que personne n'est connecté on reste bloqué
  	if (newsockfd < 0)
       	error("ERROR on accept");
  	bzero(serverbuffer,256);
  	n = read(newsockfd,serverbuffer,255); // on lit le message 
  	if (n < 0) error("ERROR reading from socket");
  	printf("Here is a message from a client: '%s' '%c'\n",serverbuffer,serverbuffer[0]);

	if ( serverbuffer[0] == 'C' ) //si message envoyé commencant par C alors demande de connection
	{
		char connect;
		char mess[100]; 

		printf("Commande C\n");
		sscanf ( serverbuffer , "%c %s %d %s " , &connect ,
			tableauJoueurs[compteurJoueurs].ipaddress , 
			&tableauJoueurs[compteurJoueurs].portno , 
			tableauJoueurs[compteurJoueurs].nom ) ;
		sprintf(mess,"C %s %d",tableauJoueurs[compteurJoueurs].nom,compteurJoueurs); //construit un message
		compteurJoueurs++;
		broadcast(mess); //diffuse le message à tous
	}
	if(compteurJoueurs == 5)
	  {     
	    char mess[100];
	    broadcast("Let's go, vous êtes cinq");
	    initRoles(); //initialise les roles 
	    sendRoles(); 
	    sendMeneur();
	    sendEquipe();
	    int i;
	    for(i=0;i<compteurJoueurs;++i)
	      {
		sprintf(mess,"C %s %d",tableauJoueurs[i].nom,i);
		broadcast(mess);}
	  }
	    
  	close(newsockfd);
     }
     close(sockfd);
}

void sendMessage(int j, char *mess)
{
        int sockfd, n;

        struct sockaddr_in serv_addr; //client tcp car socket connect write 
        struct hostent *playerserver; 
        char buffer[256];

        sockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (sockfd < 0)
                error("ERROR opening socket");
        playerserver = gethostbyname(tableauJoueurs[j].ipaddress);
        if (playerserver == NULL) {
                fprintf(stderr,"ERROR, no such host\n");
                exit(0);
        }
        bzero((char *) &serv_addr, sizeof(serv_addr));
        serv_addr.sin_family = AF_INET;
        bcopy((char *)playerserver->h_addr, (char *)&serv_addr.sin_addr.s_addr,
                                playerserver->h_length);
        serv_addr.sin_port = htons(tableauJoueurs[j].portno);
        if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
                        error("ERROR connecting");

        n = write(sockfd,mess,strlen(mess));
        if (n < 0)
                error("ERROR writing to socket");
        close(sockfd);
}


void sendChosenOnes() //renvoie les joueurs 
{
}

int main(int argc, char *argv[])
{
     pthread_t thread1, thread2;
     int  iret1, iret2;

	if (argc!=3)
	{
		printf("Usage : ./mainserver nbjoueurs numport\n");
		exit(1);
	}

     com='0';
     nbj=atoi(argv[1]); //on transforme le nbr de joueurs en int 
     printf("Nombre de joueurs=%d\n",nbj);
     portno=atoi(argv[2]); //on transforme un char en int 
     printf("Serveur ecoute sur port %d\n",portno);
     compteurJoueurs=0;

     compteurRebelles=0;
     compteurEspions=0;
     compteurMissions=0;
     meneurCourant = 0;
     nbespions=1;


    /* Create independent threads each of which will execute function */

     iret1 = pthread_create( &thread1, NULL, server, NULL); //on cree le premier pthread - server doit être de type void* - 1er parametre=adresse thread1, permet de consulter le numero que la biblio a affecté 
     if(iret1) //si pas bon donc pas egal à 0
     {
         fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
         exit(EXIT_FAILURE);
     }

     printf("pthread_create() for thread 1 returns: %d\n",iret1);

     /* Wait till threads are complete before main continues. Unless we  */
     /* wait we run the risk of executing an exit which will terminate   */
     /* the process and all threads before the threads have completed.   */
    
     pthread_join( thread1, NULL);

     exit(0);
}
