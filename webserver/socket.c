#include "socket.h"
#include "signal.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "tools.h"
#include <ctype.h>


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

char * afficherMessage(){
	char * message_bienvenue=" __          ________ ____   _______     _______ \n\\ \\        / /  ____|  _ \\ / ____\\ \\   / / ____|\n\\ \\  /\\  / /| |__  | |_) | (___  \\ \\_/ / (___  \n\\ \\/  \\/ / |  __| |  _ < \\___ \\  \\   / \\___ \\ \n\\  /\\  /  | |____| |_) |____) |  | |  ____) |\n       \\/  \\/   |______|____/|_____/   |_| |_____/ \n                                                   \n                                                    \n Welcome to websys, our dedicated webserver for your websites. \n Created by Delwaulle Loic & Froment Benoit during a student project.\n";
	return message_bienvenue;
}

char *substr(const char *src,int pos,int len) { 
	char *dest=NULL;                        
	if (len>0) {                           
   		dest = calloc(len+1, 1);       
    		if(NULL != dest) {
       			strncat(dest,src+pos,len);            
    		}
  	}                                       
	return dest;                            
}
int parse_http_request( const char * ligne , http_request * request){
	int nbMots=1;
	int i;
	char get[50];
	char str[50];
	sscanf(ligne,"%s %s", get, str);
	if(strcmp(get,"GET")==0){
		request->method=HTTP_GET;
	}
	else {
		request->method=HTTP_UNSUPPORTED;
	}
	int k =strlen(ligne);
	for(i=0;i<k;i++){
		if(ligne[i]==' '){
			nbMots++;
			if(nbMots==3){
				char *http=substr(ligne,i+1,4);
				char *version=substr(ligne,i+6,strlen(ligne));
				if(strcmp(http,"HTTP")!=0){
					return 0;
				}
				char c=version[0];
				char c2=version[2];
				int v1=(int)c-48;
				int vv=(int)c2-48;
				request->major_version=v1;
				request->minor_version=vv;
				if(v1!=1 || (vv!=0 && vv!=1)) {
					return 0;
				}
			}
			else if (nbMots==2){
				char method[50];
				char *url=malloc(50);
				char reste[50];
				sscanf(ligne,"%s %s %s", method, url,reste);
				request->url=url;
			}
		}
	}
	if(nbMots!=3)
		return 0;
	return 1;
}

char * rewrite_url(char * url,FILE * f){
	int i;
	if(strcmp(url,"/")==0){
		return "/index.html";
	}
	int k=strlen(url);
	for(i=0;i<k;i++){
		if(url[i]=='?')
			break;
	}

	if (strstr(url, "..") != NULL) {
   		send_response(f, 403 , "Refused" , "Accès refusé\r\n" );
		exit(0);
	}
	return substr(url,0,i);

}

int compareExt(char * ext){
	int i;
	for(i=0;i<MAX_MIMES;i++){
		if(strcmp(ext,mime[i].ext)==0){
			return i;
		}
	}
	return -1;
}

char * searchSpace(char * ptr){
	while (*ptr && !isspace(*ptr))
			ptr++;
	if(*ptr)
		return ptr;
	return NULL;
}

char * searchEndSpace(char *ptr){
	while (*ptr && isspace(*ptr))
			ptr++;

	if(*ptr)
		return ptr;
	return NULL;
}

type_ext mime[MAX_MIMES];
#define BUFF_SIZE 256
int get_type_mime(){
	FILE *fichier = fopen("/etc/mime.types","r");
	char buffer[BUFF_SIZE];
	int j=0;
	while(fgets(buffer,BUFF_SIZE,fichier) != NULL){

		/* commentaires */		
		char *ptr;
		ptr=strchr(buffer,'#');
		if(ptr)
			*ptr='\0';
		type_ext *p = &mime[j];
		int i;
		/* remplacement tab et autre par espace */
		for (i = 0 ; buffer[i] ; ++i)
			if (isspace(buffer[i]))
				buffer[i] = ' ';

		/* type mime */
		char *type = strtok(buffer," ");
		if(type==NULL)
			continue;
		//printf("type: %s: %d %d\n",type, (int)strlen(type), j);
		strcpy(p->type,type);
		//printf("%s\n",p->type);
		type = strtok(NULL, " ");
		int idxType=j;
		while( type != NULL ) 
  		{
			if(j!=idxType)
				strcpy(p->type,mime[idxType].type);
			//printf("ext: %s: %d\n",type, (int)strlen(type));
    			strcpy(p->ext,type);		
			//printf("tour : %i  -- %i --   %s\n",j,idxType, p->ext );
			j++;
			p=&mime[j];
      			type = strtok(NULL, " ");
  		}
		j++;
		if (j == MAX_MIMES)
			break;
	}
	return 0;

}

char * get_ext_file(char * url){
	char *ext=strrchr(url,'.');
	if(ext == NULL)
		return "";
	return ext+1;
}

int get_file_size(int fd){
	struct stat fileStat;
    	if(fstat(fd,&fileStat) < 0)    
        	return -1;
	return fileStat.st_size;
}

char *fgets_or_exit( char * buffer , int size , FILE * stream ){
	if(fgets(buffer,size,stream)==NULL){
		exit(0);
	}
	return NULL;
}

void send_response ( FILE * client , int code ,const char * message_cours ,const char * message_long ){
	send_status(client,code,message_cours);
	fprintf(client, "Content-Length: %i\r\n\r\n%s",(int)strlen(message_long),message_long);
	fflush(client);
}

void send_status( FILE * client , int code , const char * reason_phrase ){
	fprintf(client,"HTTP/1.1 %i\r\n%s\r\n",code,reason_phrase);
	fflush(client);
}

#define BUFF_SIZE 256
void skip_headers(FILE *client){
	char p[BUFF_SIZE];
	while(fgets_or_exit(p,BUFF_SIZE,client)){
		if(p[0]=='\n' || (strcmp(p,"\r\n")==0)){
			break;
		}
	}
}


int check_and_open(const char * url , char * document_root ){
	
	char *str=malloc(strlen(url)+strlen(document_root)+1);
	strcpy(str,document_root);
	strcat(str, url);
	struct stat fi;
        stat(str, &fi);
        if(S_ISREG(fi.st_mode)==0){
		return -1;
	}
	int fichier = open(str,O_RDONLY);
	if(fichier==-1){
		perror("open fichier");
		return -1;
	}
	return fichier;

}
#define BUFF_SIZE 256
int copy(int in, int out){
	char buffer[BUFF_SIZE];
	int nbEcrit;
	while((nbEcrit=read(in,buffer,sizeof(buffer)))>0){
    		write(out,buffer,nbEcrit);
	}
	return 0;
}

void send_stats ( FILE * client ){
	send_status(client,200,"OK");
	fprintf(client, "Content-Length: %i\r\n",8);
	fprintf(client,"Content-Type: %s\r\n\r\n","text/plain");
	fflush(client);
	fprintf(client,"STATUS\r\n");
	fflush(client);
}

#define BUFF_SIZE 256
void traiterClient(int socket_client, char * root){
	char p[BUFF_SIZE];
	int i=0;
	int open;
	const char * mode="w+";
	http_request req;
	http_request *r=&req;
	FILE * f=fdopen(socket_client, mode);
	fgets_or_exit(p,BUFF_SIZE,f);
	i=parse_http_request(p,r);
	skip_headers(f);
	char *rew_url=rewrite_url(req.url,f);

	open=check_and_open(rew_url, root );

	if(strcmp(rew_url,"/stats")==0){
		send_stats(f);
		exit(0);
	}
	else if(i==0){
		send_response(f, 400 , "Bad Request" , "Bad request\r\n");
		exit(0);			
	}		
	else if(req.method == HTTP_UNSUPPORTED ){
		send_response( f , 405 , "Method Not Allowed" , "Method Not Allowed\r\n" );
		exit(0);		
	}
	else if(open== -1){
		send_response(f, 404 , "Not Found" , "Not Found\r\n" );
		exit(0);
	} 
	else{	
		send_status(f,200,"OK");
		fprintf(f, "Content-Length: %i\r\n",get_file_size(open));
		char *ext=get_ext_file(rew_url);
		int result=compareExt(ext);
		if(result>0){
			fprintf(f,"Content-Type: %s\r\n\r\n",mime[result].type);
		}
		else {
			fprintf(f,"Content-Type: %s\r\n\r\n","text/plain");
		}
		fflush(f);
		copy(open,fileno(f));
		exit(0);
	}

}

int attendre_socket(int socket_serveur, char * root){
	pid_t pid;
	while(1){
		int socket_client ;
		socket_client = accept(socket_serveur , NULL , NULL);
		if (socket_client == -1){
			perror("accept");
			return -1;
		}
			
		pid=fork();
		if(pid==0){

			traiterClient(socket_client,root);
			exit(0);
		}
		else{ 
			close(socket_client);
		}		
	}
}
