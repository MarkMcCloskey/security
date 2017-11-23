#include <math.h>
#include <inttypes.h>
#include <stdio.h>

#include "entropy.h" 

#define debug 1
#define Debug(args...) if (debug) printf("entropy: "); printf(args);

double calculateEntropy(void *data, uint64_t size){
	Debug("calculateEntropy called\n");	
	
	char byteCounts[256] = {0}; //count of specific bytes in data initialized to 0
	uint64_t i;
	char *newData = data;//cast data to char array
	double entropy = 0, probability = 0;
	
	for (i = 0; i < size; i++){
		//use byte value to increment into array
		//and keep track of the number of bytes with that value
		byteCounts[(unsigned int)newData[i]]++;
	}
	
	for (i = 0; i < 256; i++){
		if (byteCounts[i] != 0){
			probability = 1.0 * byteCounts[i] / size;
			entropy -= probability * (log10(probability) / log10(256));	
		}	
	}
	
	Debug("entropy calculated: %lf\n",entropy);
	//dummy return for now
	return entropy;
}
