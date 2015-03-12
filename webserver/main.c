#include <stdio.h>
#include <string.h>
#include "socket.h"
#include <unistd.h>
#include <sys/stat.h>
#include "tools.h"
int main (int argc , char ** argv ){

	struct stat fichier;
	if(argc!=2){
		printf("Nombre d'arguments invalides");
		return 1;
	}
        stat(argv[1], &fichier);
        if(S_ISDIR(fichier.st_mode)==0){
		printf("dossier %s inconnu\r\n",argv[1]);
		return 1;
	}
	get_type_mime();
	int socket_serveur=creer_serveur(8000);
	if(socket_serveur!=1){
		attendre_socket(socket_serveur,argv[1]);
	}
	return 0;
}
