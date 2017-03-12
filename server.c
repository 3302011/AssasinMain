/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
  perror(msg);
  exit(1);
}

/// Structure contenant les informations du vote ///

typedef struct ele
{
  int prenom; 
  int opinion;   
  struct ele* suivant; 

}Vote;  

/// structure gardant en memoire l'adresse du premier element ///

typedef struct liste
{
  Vote *premier; 
}Liste;

Liste* initialisation()
{
  Liste* L=malloc(sizeof(Liste)); 
  Vote* donnees=malloc(sizeof(Vote)); 
  L->premier=donnees; //on retient l'adresse du premier element
  
  return L; 
}

// fonctions d'ajout, de supprimer, afficher 

Liste* ajouter(Liste* liste, int prenom, int opinion)
{
  Vote *nouveau=malloc(sizeof(Vote)); 

  if(nouveau==NULL || liste==NULL)
    exit(EXIT_FAILURE); 
  
  if(nouveau!=NULL && liste!=NULL)
    {
      nouveau->prenom=prenom; 
      nouveau->opinion=opinion; 
      nouveau->suivant=liste->premier;
      liste->premier=nouveau; 
    }
  return liste; 
}

Liste* supprimer(Liste* liste)
{   

  if(liste==NULL)
    {
      exit(EXIT_FAILURE); 
      
    }

  if(liste!=NULL)
    {
      Vote* elementasupprimer=liste->premier; //on supprime l'element en tete
      liste->premier=liste->premier->suivant; // on passe a l'element suivant
      free(elementasupprimer); //On perd l'adresse donc on supprime l'element
       
    }
  return (liste); 
}

void *afficher (Liste* aafficher)
{
  
  if(aafficher==NULL)
    {
      exit(EXIT_FAILURE); 

    }
  
  if (aafficher!=NULL) 
    {
      Vote* element=aafficher->premier; 
      printf("prenom:%d, opinion:%d\n",element->prenom,element->opinion);
      element=element->suivant; 
    }
 
}

int main(int argc, char *argv[])
{
  /*
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     int vote=-10; //Valeur si il y a un soucis  
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1)
     {    
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
		 if (newsockfd < 0) 
		 error("ERROR on accept");

		 bzero(buffer,256);
		 if buffer[sizeof(buffer)-1]=='y' vote=1; // c'est avec buffer que l'on réupère les informations 
		 if buffer[sizeof(buffer)-1]=='n' vote=0;
		 else vote=-1; 

		 n = read(newsockfd,buffer,255); 
		 if (n < 0) 
		 error("ERROR reading from socket");

		 printf("%s",buffer);

		 close(newsockfd);
     }
     close(sockfd); // entier qui aura une valeur quelconque 
     return 0; 


  */

  Liste* maliste=initialisation();  
  Liste* toto; //pour les tests 
  int c;
  int p;
 
  printf("Prenom?");
  scanf("%d",&p);
  printf("Oui ou Non?");  
  scanf("%d",&c); 
  toto=ajouter(maliste,p,c);  
  supprimer(toto); 
  afficher(toto); 
  free(maliste); //marche mais ajoute element 0 0 
 
}

// on considère que newsockfd et sockfd sont identidiques. Ce sont des alias.
//Faire un read. Si résultat positif alors on peut recuperer le message et tout va bien, sinon résultat négatif et on comprend qu'il a un pb.  
