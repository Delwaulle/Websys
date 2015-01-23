#include "socket.h"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



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
	char * message_bienvenue=" __          ________ ____   _______     _______ \n\\ \\        / /  ____|  _ \\ / ____\\ \\   / / ____|\n\\ \\  /\\  / /| |__  | |_) | (___  \\ \\_/ / (___  \n\\ \\/  \\/ / |  __| |  _ < \\___ \\  \\   / \\___ \\ \n\\  /\\  /  | |____| |_) |____) |  | |  ____) |\n       \\/  \\/   |______|____/|_____/   |_| |_____/ \n                                                   \n                                                    \n Welcome to websys, our dedicated webserver for your websites. \n Created by Delwaulle Loic & Froment Benoit during a student project.\n";
	int fd=write(socket_client, message_bienvenue , strlen(message_bienvenue));
	if(fd==-1){
		perror("write");
		return -1;
	}
	close(fd);
	return 0;
}

#define BUFF_SIZE 128
void traiterClient(int socket_client){
	char p[BUFF_SIZE];
	sleep(1000);
	afficherMessage(socket_client);
	int i=0;
	while((i=read(socket_client,p,BUFF_SIZE))!=EOF){
		write(socket_client, p, i);
	}
}
