#include <stdio.h>
#include <string.h>
#include "socket.h"
#include <unistd.h>
#include <sys/stat.h>
int main (int argc , char ** argv ){

	struct stat fichier;
        stat(argv[1], &fichier);
        if(S_ISDIR(fichier.st_mode)==0){
		printf("dossier %s inconnu",argv[1]);
		return 0;
	}
	int socket_serveur=creer_serveur(8000);
	if(socket_serveur!=1){
		attendre_socket(socket_serveur,argv[1]);
	}
	return 0;
}
