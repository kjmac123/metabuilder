#include <stdio.h>

#include "mylib.h"

int thisIsMyFunctionThereAreManyLikeItButThisOneIsMine(int a)
{
	#ifdef METABUILDER
	printf("Metabuilder is defined\n");
	#else
	printf("Metabuilder is NOT defined\n");
	#endif
	
	return a+a;
}
