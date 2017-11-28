#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "commandLine.h"
#include "err.h"

#define debugCommandLine 0
#define Debug(args...) if (debugCommandLine){ printf("commandline: ");  printf(args);}

char username[TRUE_STRING_LENGTH + 1] = "";
char password[TRUE_STRING_LENGTH + 1] = "";
char binaryName[BINARY_NAME_LENGTH + 1] = "";

extern int check(char*,char*);

char *parseCommandLine(int *argc, char* argv[]){
	Debug("parseCommandLineCalled\n");

	if (*argc != 2){
		err("Usage is binScan <binary file>");
	}
	return argv[1];
}

void printHelp(){
	Debug("printHelp called.\n");
	printf("Usage is binScan <binary file>\n");
}

char *getBinaryName(){
	Debug("getBinaryName called.\n");
	int i = 0;
	char c = 0;
	printf("Enter binary name: ");
	while ((c = getchar()) != EOF && c != '\n' && i < BINARY_NAME_LENGTH){
		binaryName[i] = c;
		i++;
	}
	
	binaryName[i] = '\0';	
	Debug("Binary name is %s\n",binaryName);
	return binaryName;
}
//REQUIREMENT 10
void getUsername(){
	Debug("getUsername called.\n");
	int i = 0;
	char c = 0;

	printf("Enter username: ");	
	while ((c = getchar()) != EOF && c != '\n' && i < TRUE_STRING_LENGTH){
		username[i] = c;
		i++;
	}
	if ( i < FAKE_STRING_LENGTH){
		username[i] = '\0';
	} else {
		username[FAKE_STRING_LENGTH] = '\0';
	}
	Debug("Username: %s\n",username);

}

void getPassword(){
	Debug("getPassword called.\n");
	int i = 0;
	char c = 0;

	printf("Enter password: ");
	while ((c = getchar()) != EOF && c != '\n' && i < TRUE_STRING_LENGTH){
		password[i] = c;
		i++;
	}
	if ( i < FAKE_STRING_LENGTH){
		password[i] = '\0';
	} else {
		password[FAKE_STRING_LENGTH] = '\0';
	}
	Debug("Password: %s\n",password);
}

void validateUsernamePassword(){
	Debug("validateUsernamePassword called.\n");
	if ( check(username,password) ){
		Debug("Check passed.\n"); 
		return;
	} else {
		Debug("Check failed.\n");
		//do other checking maybe
	}
	err("Invalid username and password.");
}

int promptUser(){
	Debug("promptUser called\n");
	char c[2] = "";
	int choice = -1;
	printPrompt();
	do{
		printf("Please choose a valid option: ");
		c[0] = getchar();
		choice  = atoi(c);
		clearInput();
		Debug("choice is %d\n", choice);
		

	}while( choice < ANALYZE_BINARY || choice > EXIT);


	return choice;	

}


void printPrompt(){
	Debug("printPrompt Called\n");
	printf("********\n");
	printf("OPTIONS\n");
	printf("********\n");
	printf("%d: Analyze Binary\n", ANALYZE_BINARY);
	printf("%d: Save Binary Information\n", SAVE_INFO);
	printf("%d: Exit\n",EXIT);
}

void clearInput(){
	char c = 0;
	while ((c = getchar()) != '\n' && c != EOF);
}



