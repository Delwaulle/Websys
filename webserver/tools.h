#include <sys/types.h>
#include <stdio.h>
#ifndef TOOLS_H  
#define TOOLS_H
typedef struct{
	char type[30] ;
	char ext[15] ;
} type_ext;

extern type_ext mime[500];
#endif
