#include "socket.h"
#include "signal.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

int creer_serveur(int port ){
	int socket_serveur ;

	//On ignore le signal SIGPIPE
	initialiser_signaux();

	socket_serveur = socket(AF_INET , SOCK_STREAM , 0);
	if ( socket_serveur == -1){
		perror ("socket_serveur");
		return -1;
	}
	
	//Ctrl+c sans délai
	int optval = 1;
	if(setsockopt(socket_serveur , SOL_SOCKET , SO_REUSEADDR , &optval , sizeof(int)) == -1)
		perror("Can not set SO_REUSEADDR option ");

	struct sockaddr_in saddr ;
	saddr.sin_family = AF_INET ; /* Socket ipv4 */
	saddr.sin_port = htons(port); /* Port d ’ écoute */
	saddr.sin_addr.s_addr = INADDR_ANY ; /* écoute sur toutes les interfaces */
	int bindd=bind(socket_serveur , (struct sockaddr *)&saddr , sizeof( saddr ));
	if ( bindd == -1){
		perror ("bind socket_serveur");
		return -1;
	}
	int listenn=listen(socket_serveur , 10);
	if ( listenn == -1){
		perror ("listen socket_serveur");
		return -1;
	}

	return socket_serveur;
}

int afficherMessage(int socket_client){
	char * message_bienvenue=" __          ________ ____   _______     _______ \n\\ \\        / /  ____|  _ \\ / ____\\ \\   / / ____|\n\\ \\  /\\  / /| |__  | |_) | (___  \\ \\_/ / (___  \n\\ \\/  \\/ / |  __| |  _ < \\___ \\  \\   / \\___ \\ \n\\  /\\  /  | |____| |_) |____) |  | |  ____) |\n       \\/  \\/   |______|____/|_____/   |_| |_____/ \n                                                   \n                                                    \n Welcome to websys, our dedicated webserver for your websites. \n Created by Delwaulle Loic & Froment Benoit during a student project.\n";
	int fd=write(socket_client, message_bienvenue , strlen(message_bienvenue));
	if(fd==-1){
		perror("write");
		return -1;
	}
	close(fd);
	return 0;
}

char *substr(char *src,int pos,int len) { 
	char *dest=NULL;                        
	if (len>0) {                  
   		/* allocation et mise à zéro */          
   		dest = calloc(len+1, 1);      
   		/* vérification de la réussite de l'allocation*/  
    		if(NULL != dest) {
       			strncat(dest,src+pos,len);            
    		}
  	}                                       
	return dest;                            
}

int traitement_entete_http(char ligne []){
	int nbMots=1;
	int i;
	char get[50];
	char str[50];
	sscanf(ligne,"%s %s", get, str);
	if(strcmp(get,"GET")!=0)
		return -1;
	for(i=0;i<strlen(ligne);i++){
		if(ligne[i]==' '){
			nbMots++;
			if(nbMots==3){
				char *http;
				char *version;
				http=substr(ligne,i+1,4);
				//printf("%s\n",http);
				if(strcmp(http,"HTTP")!=0)
					return -1;
				version=substr(ligne,i+6,strlen(ligne));
				//printf("%s\n",version);
				char c=version[0];
				char c2=version[2];
				int v1=(int)c-48;
				int vv=(int)c2-48;
				if(v1!=1 || (vv!=0 && vv!=1)) {
					return -1;
				}
			}
		}
	}
	if(nbMots!=3)
		return -1;

	
	return 0;
}

#define BUFF_SIZE 128
void traiterClient(int socket_client){
	char p[BUFF_SIZE];
	int i=0;
	const char * mode="w+";
	FILE * f=fdopen(socket_client, mode);
	while(fgets(p,BUFF_SIZE,f)!=NULL){
		if(i==0){
			if(traitement_entete_http(p)!=0){
				fprintf(f, "HTTP/1.1 400 Bad Request\r\nConnection: close\r\nContent-Length: %i\r\n400 Bad request\r\n",17);
				exit(0);
			}	
		}
		if(p[0]=='\n' || (strcmp(p,"\r\n")==0))
			break;
		
		printf("<websys> %s", p);
		i++;
	}

	/*while((i=read(socket_client,p,BUFF_SIZE))> 0){
		write(socket_client, p, i);
	}*/
	afficherMessage(socket_client);
	fprintf(f,"HTTP/1.1 200 OK\r\nContent-Length: %i\r\n",10);
}

int attendre_socket(int socket_serveur){
	pid_t pid;
	int status;
	while(1){
		int socket_client ;
		socket_client = accept(socket_serveur , NULL , NULL );
		if (socket_client == -1){
			perror("accept");
			return -1;
		}
			
		pid=fork();
		if(pid==0){
			traiterClient(socket_client);
			exit(0);
		}
		else{ 
			close(socket_client);
		}		
	}
}
