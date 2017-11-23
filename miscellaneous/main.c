#include <stdio.h>
#include <inttypes.h>

#include "err.h"
#include "commandLine.h"
#include "parseElf.h"
#include "md5.h"
#include "entropy.h"

#define debug 1
#define Debug(args...) if(debug) printf("main: "); printf(args);

int main(int argc, char *argv[]){
	char *binaryName;	
	ElfDetails *deets;

	binaryName = parseCommandLine(&argc, argv);
	printf("Binary name: %s\n", binaryName);
	
	deets = parseElf(binaryName);
	deets->md5Hash = 0;
	deets->md5Hash = hash(deets->textData, (unsigned long)deets->sizeOfTextSection);
	
	destroyElfDetails(deets);
	
	return 0;
}
