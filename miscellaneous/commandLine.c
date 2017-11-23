#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commandLine.h"
#include "err.h"
char *parseCommandLine(int *argc, char* argv[]){

	if (*argc != 2){
		err("Usage is binScan <binary file>");
	}
	return argv[1];
}

void printHelp(){
	printf("Usage is binScan <binary file>\n");
}
