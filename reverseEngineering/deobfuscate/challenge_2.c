#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Ensure debug is off unless compiled with it defined as: gcc -DDEBUG
#ifndef DEBUG
#define DEBUG 0
#endif

#define debug(fmt, ...) \
	do { if (DEBUG) fprintf(stderr, fmt, __VA_ARGS__); } while (0)

unsigned char *deobfuscate(unsigned char *obfuscated_string, time_t start_time, int strlen);
time_t extract_time(void *buffer);
char *extract_string(void *buffer);
int get_file_size(FILE *fptr);

int STRING_OFFSET = 1;

int main(){
	debug("%s\n\n", "DEBUG SET");	
	
	// Open the file and read it in.
	FILE *fptr;
	fptr = fopen("Challenge_2_obfuscated.bin", "rb");
	int file_size = get_file_size(fptr);		
	debug("File size: %d\n", file_size);

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
	debug("Time: %u\n", start_time);
	
	// extract the string from the buffer
	unsigned char *obfuscated_string = file_contents + STRING_OFFSET;
	int strlen = file_size - STRING_OFFSET;

	debug("Obfuscated string length: %d\n", strlen);	
	debug("%s\n", "Obfuscated string: ");
	for (int i = 0; i < strlen; i++){
		debug("%x ", obfuscated_string[i]);
	}
	debug("%s", "\n");
	
		
	// get the original string.	
	unsigned char *deobfuscated_string = deobfuscate(obfuscated_string, start_time, strlen);	
	for (int i = 0; i < strlen; i++ ){
		printf("%c", deobfuscated_string[i]);
	}
	printf("\n");
		
	FILE *out_file = NULL;

	out_file = fopen("Challenge_2_deobfuscated.bin", "wb");
	fwrite(deobfuscated_string, sizeof(unsigned char), strlen, out_file);
	fclose(out_file);
	
	free(deobfuscated_string);
	free(file_contents);
	return 0;
}

// Given a buffer containing the file contents pull the time out and return it.
time_t extract_time(void *buf){
	// treat the beggining of the buffer as if it is time_t since that is what it holds.
	time_t start_time = *((unsigned char*)(buf));
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
	
	
	// time variables
	unsigned char time_low = start_time & 0xFF;
	unsigned char new_time_low = 0, time_left = 0, time_right = 0;
	
	// character variables
	unsigned char *obfuscated_char= 0;
	unsigned char working_char = 0;
	unsigned char zeroeth_bit = 0, fifth_bit = 0, fourth_bit = 0;

	for (int i = 0; i < strlen; i++){
		// get the current character to deobfuscate
		obfuscated_char = obfuscated_string + i;
		debug("Deobfuscating char: %x\n", *obfuscated_char);
		
		// handle all operations on the time	
		time_right = time_low >> 7;
		time_left = time_low << 1;
		new_time_low = (time_right | time_left) & 0xFF;
		
		// get lowest bit in obfuscated character	
		fourth_bit = (*obfuscated_char & 0x80) >> 7;
		debug("Fourth bit: %u\n", fourth_bit);
		
		// figure out what the correct zeroeth bit should be
		zeroeth_bit = ((*obfuscated_char & 4) >> 2) ^ fourth_bit;
		// zeroeth_bit = ((fourth_bit << 2) ^ (*obfuscated_char & 4)) >> 2;
		debug("Zeroeth bit: %u\n", zeroeth_bit);

		// figure out what the correct fifth bit should be.
		fifth_bit = ((*obfuscated_char & 8) >> 3) ^ zeroeth_bit;
		//fifth_bit = ((zeroeth_bit << 3) ^ (*obfuscated_char & 8)) >> 3;
		debug("Fifth bit: %u\n", fifth_bit);
		
		working_char = 0;
		// working_char |= zeroeth_bit;
		// working_char |= ((0xF0 & *obfuscated_char) >> 3);

		// working_char |= (fifth_bit << 5);
		//working_char |= (sixth_bit << 6);
		//working_char |= (*obfuscated_char & 2) << 6;
		//working_char |= ((0x03 & *obfuscated_char) << 6);
		
		working_char = ((*obfuscated_char & 3) << 6) | (fifth_bit << 5) | ((*obfuscated_char & 0xF0) >>3) | zeroeth_bit;
		debug("Before time xor working char: %c\n", working_char);
		working_char = working_char ^ time_low;

		debug("New time: %x\n", new_time_low);
		debug("Deobfed char: %c\n", working_char);
		time_low = new_time_low;
		deobfuscated_string[i] = working_char;
	}

	return deobfuscated_string;
}
