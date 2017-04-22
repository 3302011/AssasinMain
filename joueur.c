#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <gtk/gtk.h>

///////////////////////////////////////////////////////////// VARIABLES GLOBALES ////////////////////////////////////////////////////////////////

pthread_t server_thread;
int server_thread_ret;
int localServerThreadPortno;
char server_thread_buffer[1000];

char mainServerAddr[100];
char mainServerPort[100];
char localServerAddr[100];
char localServerPort[100];
char username[100];

GtkWidget *labelAddrServer;
GtkWidget *labelPortServer;
GtkWidget *labelPlayer[8];
GtkWidget *rolePlayer[8];
GtkWidget *checkboxPlayer[8];
GtkWidget *votePlayer[8];
GtkWidget *radiovotePlayer[2];
GtkWidget *radiosucceedPlayer[2];
GtkWidget *boutonProposition;

GtkTextBuffer *buffer;
GtkWidget *text_view;
GtkWidget *scrolled_window;


int mission=0; 
int participantsMissions[5]={2,2,3,3,2};

//////////////////////////////////////////////////////////////////// FONCTIONS //////////////////////////////////////////////////////////////////////////////

void *server_func(void *ptr)
{
     int sockfd, newsockfd;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int n;

     // Cree le socket

     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
     {
        printf("ERROR opening socket\n");
	exit(1);
     }

     // Bind le socket avec server_thread_portno

     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(localServerThreadPortno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0)
              error("ERROR on binding");

     // Ecoute sur la socket

     listen(sockfd,5);
     clilen = sizeof(cli_addr);
     while (1)
     {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0)
	{
          printf("ERROR on accept\n");
	  exit(1);
        }

        bzero(server_thread_buffer,256);
        n = read(newsockfd,server_thread_buffer,255);
        if (n < 0) 
	{
	  printf("ERROR reading from socket\n");
	  exit(1);
        }

        printf("received from main_server '%s'\n",server_thread_buffer);

	if (server_thread_buffer[0]=='1')
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkboxPlayer[0]), TRUE);
	else if (server_thread_buffer[0]=='0')
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkboxPlayer[0]), FALSE);
	else if (server_thread_buffer[0]=='2')
		gtk_widget_set_sensitive (checkboxPlayer[0], TRUE);
	else if (server_thread_buffer[0]=='3')
		gtk_widget_set_sensitive (checkboxPlayer[0], FALSE);
	else if (server_thread_buffer[0]=='4')
		gtk_widget_set_sensitive (boutonProposition, TRUE);
	else if (server_thread_buffer[0]=='5')
		gtk_widget_set_sensitive (boutonProposition, FALSE);
	else if (server_thread_buffer[0]=='6')
		gtk_label_set_text ((GtkLabel*)rolePlayer[0], "Rebelle");
	else if (server_thread_buffer[0]=='7')
	  {
	    int ta; 
	    int elem; 
	    sscanf(server_thread_buffer,"%d %d",&ta,&elem); 
	    gtk_label_set_text ((GtkLabel*)rolePlayer[elem], "Espion ");
	     gtk_label_set_text ((GtkLabel*)rolePlayer[ta], "Espion ");
	
	  }
    else if (server_thread_buffer[0] == '8')
    {
      int ta;
      int mission;
      int ins; //nombre de personnes donnees 
      sscanf(server_thread_buffer,"%d %d",&ta,&mission); 
      printf("Vous êtes le meneur, choisissez %d joueurs\n",mission);

      
    }
       else if (server_thread_buffer[0]=='C')
    
	{
		char connect;
		char nom[100];
		int index;

                printf("Commande C\n");
                sscanf ( server_thread_buffer , "%c %s %d" , &connect, nom, &index);
		
		printf("nom=%s index=%d\n",nom, index);

		gtk_label_set_text ((GtkLabel*)labelPlayer[index], nom);
	}

        close(newsockfd);
     }
     close(sockfd);
}
void sendMessageToMainServer(char *mess);

void click_boutonProposition(GtkWidget *widget, gpointer window) 
{
     GtkTextMark *mark;
     GtkTextIter iter;
     int pax=0; 
	printf("click_boutonProposition\n");

  //buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
  //gtk_text_buffer_get_iter_at_offset(buffer, &iter, 0);

int i;
for (i=0;i<5;i++)
{
  if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(checkboxPlayer[i]))) //si le bouton est coché 
  {

    char mess[100]; 
    sprintf(mess,"%d",i);
    printf("%s",mess); 
    sendMessageToMainServer(mess); 

  }
}
      
   //gtk_text_buffer_insert (buffer, &iter, "Proposition accepted\n", -1);
      
}

void voteOui(GtkWidget *widget, gpointer window) {
		printf("Oui\n");
}

void voteNon(GtkWidget *widget, gpointer window) {
		printf("Non\n");
}

void sendMessageToMainServer(char *mess)
{
    int sockfd, mainServerPortno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(mainServerAddr);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", mainServerAddr);
        exit(1);
    }

    mainServerPortno=atoi(mainServerPort);

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(mainServerPortno);

    /* connect: create a connection with the server */
    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
      error("ERROR connecting");

    /* send the message line to the server */
    n = write(sockfd, mess, strlen(mess));
    if (n < 0)
      error("ERROR writing to socket");

    close(sockfd);

}

int main(int argc, char** argv) {

  GtkWidget *window;
  GtkWidget *table;
  GtkWidget *fixed;

  GtkWidget *entry1;
  GtkWidget *entry2;
  GtkWidget *entry3;
  int i;
  char addr_server_text[50];
  char port_server_text[50];
  char com_connexion[200];

if (argc!=6)
{
	printf("Usage : .joueur @ip_server numport_server @ip_joueur numport_joueur prenom\n");
	exit(1);
}

  printf("mainserver   addr: %s\n", argv[1]); //adresse du serveur
  printf("mainserver   port: %s\n", argv[2]);//numero de port
  printf("local server addr: %s\n", argv[3]);//adresse de joueur 
  printf("local server port: %s\n", argv[4]);//numero de port de joueur 
  printf("         username: %s\n", argv[5]);//nom du joueur 

  strcpy(mainServerAddr,argv[1]);
  strcpy(mainServerPort,argv[2]);
  strcpy(localServerAddr,argv[3]);
  strcpy(localServerPort,argv[4]);
  strcpy(username,argv[5]);

  localServerThreadPortno=atoi(argv[4]);

  server_thread_ret = pthread_create( &server_thread, NULL, server_func, NULL);
  if (server_thread_ret)
     {
         fprintf(stderr,"Error - pthread_create() return code: %d\n",server_thread_ret); 
         exit(EXIT_FAILURE);
     }

  gtk_init(&argc, &argv); // on initialise gtk 

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL); // on cree une nouvelle fenetre gtk 
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER); //
  gtk_window_set_title(GTK_WINDOW(window), "The Resistance"); // titre 
  gtk_container_set_border_width(GTK_CONTAINER(window), 15); 
  gtk_window_set_default_size(GTK_WINDOW(window), 800, 600); // taille 

  fixed = gtk_fixed_new(); // code permettant de dire que les objets graphiques sont positionnés de manière fixée (manière manuelle)
  gtk_container_add(GTK_CONTAINER(window), fixed); // manuel : widget = objets , donne x et y en entrée 

  sprintf(addr_server_text,"Adresse serveur: %s",argv[1]); 
  labelAddrServer = gtk_label_new(addr_server_text); //on affiche du texte avec label_new 
  gtk_fixed_put(GTK_FIXED(fixed), labelAddrServer, 0, 0); // position zéro 
  gtk_widget_set_size_request(labelAddrServer,200,20); // taille 200x20

  sprintf(port_server_text,"Port serveur: %s",argv[2]);
  labelPortServer = gtk_label_new(port_server_text); // on ajoute un label (5 fois car max 5 joueurs)
  gtk_fixed_put(GTK_FIXED(fixed), labelPortServer, 200, 0);
  gtk_widget_set_size_request(labelPortServer,200,20);

  for (i=0;i<5;i++)
  {
  	labelPlayer[i] = gtk_label_new("Inconnu");
  	gtk_fixed_put(GTK_FIXED(fixed), labelPlayer[i], 0, 100+i*20);
  	gtk_widget_set_size_request(labelPlayer[i],100,20); // pour chaque joueur: creation label, check button, label 2, label 3  

        checkboxPlayer[i] = gtk_check_button_new();
	GTK_WIDGET_UNSET_FLAGS(checkboxPlayer[i], GTK_CAN_FOCUS);
  	gtk_fixed_put(GTK_FIXED(fixed), checkboxPlayer[i], 100, 100+i*20);
  	gtk_widget_set_size_request(checkboxPlayer[i],30,20);

  	rolePlayer[i] = gtk_label_new("Rebelle");
  	gtk_fixed_put(GTK_FIXED(fixed), rolePlayer[i], 150, 100+i*20);
  	gtk_widget_set_size_request(rolePlayer[i],60,20);

        votePlayer[i] = gtk_label_new("--------");
  	gtk_fixed_put(GTK_FIXED(fixed), votePlayer[i], 210, 100+i*20);
  	gtk_widget_set_size_request(votePlayer[i],60,20);
  }

  boutonProposition = gtk_button_new_with_label("Proposition"); //creation d'un bouton et on le positionne à une position
  gtk_fixed_put(GTK_FIXED(fixed), boutonProposition, 400, 100); //on lui donne une taille 
  gtk_widget_set_size_request(votePlayer[i],100,20);
  g_signal_connect(G_OBJECT(boutonProposition), "clicked", //permet d'associer une fonction de call back, fonction appelée quand on clique sur le bouton
//quand on sera dans la fonction call back, on cree un message reseau que l'on envoie au serveur 
      G_CALLBACK(click_boutonProposition), G_OBJECT(window));  // dans qt permet de faire de bonnes interfaces graphiques mais se programme en c++

  text_view = gtk_text_view_new(); //permet de visualiser le texte 
  gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD);
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window), 
                                  GTK_POLICY_AUTOMATIC, 
                                  GTK_POLICY_AUTOMATIC); 
  gtk_container_add (GTK_CONTAINER (scrolled_window), text_view);
  gtk_fixed_put(GTK_FIXED(fixed), scrolled_window, 10, 400);
  gtk_widget_set_size_request(scrolled_window,400,100);

  radiovotePlayer[0] = gtk_button_new_with_label("Oui");
  gtk_fixed_put(GTK_FIXED(fixed), radiovotePlayer[0], 210, 200);
  gtk_widget_set_size_request(radiovotePlayer[0],60,30);
  g_signal_connect(radiovotePlayer[0], "clicked", G_CALLBACK(voteOui), (gpointer) window);

  radiovotePlayer[1] = gtk_button_new_with_label("Non");
  gtk_fixed_put(GTK_FIXED(fixed), radiovotePlayer[1], 310, 200);
  gtk_widget_set_size_request(radiovotePlayer[1],60,30);
  g_signal_connect(radiovotePlayer[1], "clicked", G_CALLBACK(voteNon), (gpointer) window);
  
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_widget_show_all(window);   

  sprintf(com_connexion,"C %s %s %s",localServerAddr,localServerPort,username);
  sendMessageToMainServer(com_connexion); //on envoie au mainserver l'ordre de connexion 

  gtk_main(); //lancer la boucle des evenements 

  return 0;
}


// Règles du jeu et but de la première séance 

/* afficher sur les interfaces graphiques les 5 joueurs qui se sont connectés 
detecter 5 joueurs, basculer mode 
afficher sur chaque des fenetres les roles
rappels: rebelles ne savent rien
espions savent quels sont les autres espions */
