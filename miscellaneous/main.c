#include <stdio.h>
#include <inttypes.h>

#include "err.h"
#include "commandLine.h"
#include "parseElf.h"
#include "md5.h"
#include "entropy.h"

#define debug 1
#define Debug(args...) if(debug) printf("main: "); printf(args);

int main(){
	int userChoice;
	char *binaryName;	
	ElfDetails *deets;
	
	getUsername();
	getPassword();
	validateUsernamePassword();
	
	do{
		userChoice = promptUser();
	
		switch(userChoice){
			
			case ANALYZE_BINARY:
				Debug("user chose analyze binary\n");	
				binaryName = getBinaryName();
				deets = parseElf(binaryName);
				deets->md5Hash = 0;
				deets->md5Hash = hash(deets->textData, 
							(unsigned long)deets->sizeOfTextSection);
				printElf(deets);
				printHash(deets->md5Hash);
				break;
			
			case SAVE_INFO:
				Debug("user chose save info\n");
				break;
			
			case EXIT:
				Debug("user chose exit\n");
				break;
			
			default://should never happen
				Debug("something weird happened with user choice\n");
				userChoice = EXIT;


		}
	}while(userChoice != EXIT);



	
	destroyElfDetails(deets);
	return 0;
}
