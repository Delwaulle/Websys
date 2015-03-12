#include <sys/types.h>
#include <stdio.h>
#ifndef TOOLS_H  
#define TOOLS_H
typedef struct{
	char type[500] ;
	char ext[150] ;
} type_ext;

#define MAX_MIMES 2000
extern type_ext mime[MAX_MIMES];
#endif
