#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define debug(fmt, ...) \
	do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while(0)

unsigned char *deobfuscate(unsigned char *obfuscated_string, time_t start_time, int strlen);
time_t extract_time(void *buffer);
char *extract_string(void *buffer);
int get_file_size(FILE *fptr);

int STRING_OFFSET = 4;

int main(){
	debug("%s\n\n", "DEBUG SET");	
	// Open the file and read it in.
	FILE *fptr;
	fptr = fopen("Challenge_2_obfuscated.bin", "rb");
	int file_size = get_file_size(fptr);		
	
	// save the file into a buffer	
	char *file_contents = (char *)malloc(file_size * sizeof(char));
	int read = fread(file_contents, sizeof(char), file_size, fptr);
	if (read != file_size){
		printf("Issue reading full file\n");
		exit(1);
	}
	// done with file. 
	fclose(fptr);

	time_t start_time = extract_time(file_contents);

	// extract the string from the buffer
	unsigned char *obfuscated_string = file_contents + STRING_OFFSET;
	
	int strlen = file_size - STRING_OFFSET;
	
	debug("%s\n", "Obfuscated string: ");
	for (int i = 0; i < strlen; i++){
		debug("%x ", obfuscated_string[i]);
	}
	debug("%s", "\n");
	
	/*	
	// get the original string.	
	unsigned char *deobfuscated_string = deobfuscate(obfuscated_string, start_time, strlen);	
	for (int i = 0; i < strlen; i++ ){
		printf("%c ", deobfuscated_string[i]);
	}
	printf("\n");
	
	FILE *out_file = NULL;

	out_file = fopen("Challenge_2_deobfuscated.bin", "wb");
	fwrite(deobfuscated_string, sizeof(unsigned char), strlen, out_file);
	fclose(out_file);
	
	free(deobfuscated_string);
	*/
	free(file_contents);
	return 0;
}

// Given a buffer containing the file contents pull the time out and return it.
time_t extract_time(void *buf){
	// treat the beggining of the buffer as if it is time_t since that is what it holds.
	time_t start_time = *((time_t*)(buf));
	debug("Time: %u\n", start_time);
	return start_time;
}

// Given a file ptr, find and return the size of the file.
int get_file_size(FILE *fptr){ 
	fseek(fptr, 0, SEEK_END);
	int file_size = ftell(fptr);
	rewind(fptr);
	return file_size;
}

// return the original string from the obfuscated one.
unsigned char *deobfuscate(unsigned char *obfuscated_string, time_t start_time, int strlen){
	// create and null out buffer for deobfuscated string	
	unsigned char *deobfuscated_string = malloc(strlen * sizeof(char) + 1);
	memset(deobfuscated_string, '\0', strlen + 1);
	
	// deobfuscate
	time_t modified_time = start_time;
	time_t shifted_time = 0;
	unsigned char xored = 0;
	unsigned char subd = 0;
	unsigned char *c  = 0;
	for (int i = 0; i < strlen; i++){
		modified_time = modified_time * 0x7452845;
		modified_time += 0x2934563;
		shifted_time = modified_time >> 0x17;
		
		c = obfuscated_string + i;
		
		subd = *c - (modified_time & 0xFF);
		
		xored = subd ^ (shifted_time & 0xFF);
		
		deobfuscated_string[i] = xored;
	}

	return deobfuscated_string;
}
