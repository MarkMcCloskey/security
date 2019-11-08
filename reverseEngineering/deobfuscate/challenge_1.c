#include <stdio.h>
#include <stdlib.h>
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
	
	// extract the time from the buffer
	int time_t_size = sizeof(time_t);
	printf("Size of time_t: %d\n", time_t_size);
	time_t start_time = *((time_t*)(buffer));
	printf("Time: %d\n", start_time);

	// extract the string from the buffer
	unsigned char *obfuscated_string = buffer + time_t_size;
	printf("Obfuscated string:\n");
	for (int i = 0; i < file_length - time_t_size; i++){
		printf("%d ", obfuscated_string[i]);
	}
	printf("\n");

	// clean stuff up.
	fclose(fptr);
	free(buffer);
	return 0;
}
