#include <stdio.h>
#include <stdlib.h>
#include "err.h"

void err(char *s){
	printf("Error: %s\n", s);
	exit(EXIT_FAILURE);
}
