#include "socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>


int creer_serveur(int port ){
	int socket_serveur ;
	socket_serveur = socket(AF_INET , SOCK_STREAM , 0);
	if ( socket_serveur == -1){
		perror ("socket_serveur");
		return -1;
	}

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
	char * message_bienvenue="Bonjour";
	int fd=write(socket_client, message_bienvenue , strlen(message_bienvenue));
	if(fd==-1){
		perror("write");
		return -1;
	}
	return 0;
}

void traiterClient(int socket_client){
	char buffer[128];
	afficherMessage(socket_client);
	while(read(socket_client,buffer,128)!=EOF){
		afficherMessage(socket_client);
	}
}
