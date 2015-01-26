#include <stdio.h>
#include <string.h>
#include "socket.h"
#include <unistd.h>
#include <sys/wait.h>
int main (int argc , char ** argv ){

	int socket_serveur=creer_serveur(8000);
	pid_t pid;
	int status;
	if(socket_serveur!=1){
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
			}
			else{ 
				close(socket_client);
			}		
		}
	}
	return 0;
}
