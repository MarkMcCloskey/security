#include <stdio.h>
#include <openssl/md5.h>
#include <stdlib.h>
#include <string.h>

#include "md5.h"

#define debug 1
#define Debug(args...) if(debug) printf("md5: "); printf(args);
unsigned char *hash(void *loc, unsigned long size){
	Debug("hash called\n");
	unsigned long i = 0;
	unsigned char *text = loc;
	if (debug){
		printf("original text: ");
		for (i = 0; i < size; i++ ){
			printf("%x",text[i]);
		}
		printf("\n");
	}	
	
	unsigned char *hash = (unsigned char *)malloc(MD5_DIGEST_LENGTH);
	MD5((const unsigned char*)loc, size, hash);
	
	if (debug){
		printHash(hash);
	}	
	return hash;
}

void printHash(unsigned char *hash){
	Debug("printHash called\n");
	int i = 0;
	unsigned char storage[MD5_DIGEST_LENGTH + 1];
	memcpy((void *)storage,(const void*)hash,MD5_DIGEST_LENGTH);
	storage[MD5_DIGEST_LENGTH] = '\0';
	
	printf("hash: ");
	for (i = 0; i < MD5_DIGEST_LENGTH; i++){
		printf("%x",storage[i]);
	}
	printf("\n");
}