#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "saveFile.h"

#define debugSaveFile 1
#define Debug(args...) if (debugSaveFile){ printf("SaveFile: "); printf(args);}
#define MAX_BUF_SIZE 0x1000

int createFileBuffer(char *buffer, struct elfDetails *deets){
	int size = 0;
	int i = 0, j = 0, temp = 0;

	SaveFile *bufPtr = (SaveFile)buffer;
	size += sizeof(bufPtr->size);
	
	bufPtr->sizeOfTextSection = deets->sizeOfTextSection;
	size += sizeof(bufPtr->sizeOfTextSection);
	
	memcpy(bufPtr->hash, deets->md5Hash,MD5_DIGEST_LENGTH);
	size += MD5_DIGEST_LENGTH;
	
	bufPtr->numDlopenCalls = deets->numDlopenCalls;
	size += sizeof(bufPtr->numDlopenCalls);

	bufPtr->entropy = deets->entropy;
	size += sizeof(bufPtr->entropy);
	
	while (deets->strings[i] && i < NUM_STRING_ADDRS){
		strcpy(&bufPtr[j], deets->strings[i]);
		temp = strlen(deets->strings[i]) + 1;
		size += temp;
		j += temp;
		i++;
	}			
	
}
