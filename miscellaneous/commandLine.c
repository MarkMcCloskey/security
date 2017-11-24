#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commandLine.h"
#include "err.h"

#define debug 1
#define Debug(args...) if (debug) printf("commandline: "); printf(args);

#define STRING_LENGTH 4
char username[STRING_LENGTH + 1];
char password[STRING_LENGTH + 1];

extern int check(char*,char*);

char *parseCommandLine(int *argc, char* argv[]){

	if (*argc != 2){
		err("Usage is binScan <binary file>");
	}
	return argv[1];
}

void printHelp(){
	printf("Usage is binScan <binary file>\n");
}

//MARK MAKE USERNAME AND PASSWORD PRINT PROMPTS
void getUsername(){
	int i = 0;
	char c;
	
	while ((c = getchar()) != EOF && c != '\n' && i < STRING_LENGTH){
		username[i] = c;
		i++;
	}
	username[STRING_LENGTH] = '\0';

}

void getPassword(){
	int i = 0;
	char c;

	while ((c = getchar()) != EOF && c != '\n' && i < STRING_LENGTH){
		password[i] = c;
		i++;
	}
	password[STRING_LENGTH] = '\0';
}

void validateUsernamePassword(){

	if ( check(username,password) ){
		return;
	} else {
		//do other checking maybe
	}
	err("Invalid username and password.");
}
