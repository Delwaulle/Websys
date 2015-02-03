#include <stdio.h>
#include <string.h>
#include "socket.h"
#include <unistd.h>
int main (int argc , char ** argv ){
	int socket_serveur=creer_serveur(8000);
	if(socket_serveur!=1){
		attendre_socket(socket_serveur);
	}
	return 0;
}
