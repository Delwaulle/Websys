#include <sys/types.h>
#include <stdio.h>
#ifndef TOOLS_H  
#define TOOLS_H
typedef struct{
	char type[50] ;
	char ext[15] ;
} type_ext;

type_ext mime[500];
#endif
