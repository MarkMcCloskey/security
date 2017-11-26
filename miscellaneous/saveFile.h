#include <openssl/md5.h>//for MD5_DIGEST_LENGTH

typedef struct{
	int size;
	long sizeOfTextSection;
	unsigned char hash[MD5_DIGEST_LENGTH];
	long numDlopenCalls;
	double entropy;
	char strings[];	
} SaveFile;

int createFileBuffer(char *buffer, struct elfDetails *deets);
