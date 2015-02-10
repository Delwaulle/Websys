#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <stdio.h>
#ifndef __SOCKET_H__
#define __SOCKET_H__
/** Crée une socket serveur qui écoute sur toute les interfaces IPv4
de la machine sur le port passé en paramètre . La socket retournée
doit pouvoir être utilisée directement par un appel à accept .
La fonction retourne -1 en cas d ’ erreur ou le descripteur de la
socket créée .*/
int creer_serveur(int port );
char * afficherMessage();
int check_and_open ( const char * url ,  char * document_root );
void traiterClient(int socket_client,char * root);
int attendre_socket(int socket_serveur,char * root);
void send_status( FILE * client , int code , const char * reason_phrase );
void send_response ( FILE * client , int code ,const char * message_cours ,const char * message_long );
enum http_method {
	HTTP_GET,
	HTTP_UNSUPPORTED,
};
typedef struct{
	enum http_method method;
	int major_version ;
	int minor_version ;
	char * url ;
} http_request ;
#endif

