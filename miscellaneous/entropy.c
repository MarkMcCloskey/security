#include <math.h>
#include <inttypes.h>
#include <stdio.h>
#include <unistd.h>
#include "entropy.h" 

#define debugEntropy 0
#define Debug(args...) if (debugEntropy){ printf("entropy: "); printf(args);}

double calculateEntropy(int fd, uint64_t size){
	Debug("calculateEntropy called\n");	

	long byteCounts[256] = {0}; //count of specific bytes in data initialized to 0
	uint64_t i = 0;
	//char *newData = data;//cast data to char array
	double entropy = 0, probability = 0;
	unsigned char buffer[1024] = {0};
	unsigned int bytesRead = 0;
	
	//read the whole file
	while ((bytesRead = read(fd, buffer, 1024)) != 0){
		Debug("bytes read\n");
		//for each byte in the file
		for (i = 0; i < bytesRead; i++){
			//use byte value to increment into array
			//and keep track of the number of bytes with that value
			byteCounts[(unsigned int)buffer[i]]++;
			buffer[i] = 0;
		}
	}
	if (debugEntropy){	
		for (i = 0; i < 256; i++){
			printf("%lu ",byteCounts[i]);
		}
		printf("\n");
	}
	
	//calculate entropy
	for (i = 0; i < 256; i++){
		if (byteCounts[i] != 0){
			probability = 1.0 * byteCounts[i] / size;
			entropy -= probability * (log10(probability) / log10(256));	
		}	
	}

	Debug("entropy calculated: %lf\n",entropy);
	return entropy;
}
