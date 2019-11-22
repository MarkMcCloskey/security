#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(){
	
	// Open the file and read it in.
	FILE *fptr;

	fptr = fopen("Challenge_1_obfuscated.bin", "rb");
	fseek(fptr, 0, SEEK_END);
	int file_length = ftell(fptr);
	rewind(fptr);
	printf("File size:%d\n", file_length);
	
	// save the file into a buffer	
	char *buffer = (char *)malloc(file_length * sizeof(char));
	int read = fread(buffer, sizeof(char), file_length, fptr);
	if (read != file_length){
		printf("Issue reading full file\n");
		exit(1);
	}
	// done with file. 
	fclose(fptr);

	// extract the time from the buffer
	int time_t_size = sizeof(unsigned int);
	printf("Size of time_t: %d\n", time_t_size);
	time_t start_time = *((time_t*)(buffer));
	printf("Time: %u\n", start_time);

	// extract the string from the buffer
	unsigned char *obfuscated_string = buffer + time_t_size;
	
	int strlen = file_length - time_t_size;
	printf("String length: %d\n", strlen); 
	
	printf("Obfuscated string:\n");
	for (int i = 0; i < file_length - time_t_size; i++){
		printf("%x ", obfuscated_string[i]);
	}
	printf("\n");
	
	unsigned char *deobfuscated_string = malloc(strlen * sizeof(char));
	memset(deobfuscated_string, '\0', strlen);
	
	// deobfuscate
	time_t modified_time = start_time;
	printf("Starting time: %d\n", modified_time);
	time_t shifted_time = 0;
	unsigned char xored = 0;
	unsigned char subd = 0;
	unsigned char *c  = 0;
	for (int i = 0; i < strlen; i++){
		modified_time = modified_time * 0x7452845;
		modified_time += 0x2934563;
		printf("Modified time: %u\n", modified_time);
		shifted_time = modified_time >> 0x17;
		printf("Shifted time: %u\n", shifted_time);
		c = obfuscated_string + i;
		printf("Char being deobfed: %x\n", *c);
		subd = *c - (modified_time & 0xFF);
		printf("subbed: %u\n", /*(subd & 0xFF)*/ subd);
		
		xored = subd ^ (shifted_time & 0xFF);
		printf("Xored: %u\n", xored);
		deobfuscated_string[i] = xored;
		printf("\n");
	}
	// printf("%s\n", deobfuscated_string);
	for (int i = 0; i < strlen; i++ ){
		printf("%c ", deobfuscated_string[i]);
	}
	printf("\n");
	
	free(deobfuscated_string);
	free(buffer);
	return 0;
}
