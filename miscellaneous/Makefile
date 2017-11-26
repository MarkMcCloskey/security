CC=gcc
CFLAGS=-c -Wall -Wextra -Og 
LFLAGS=-lelf -lssl -lcrypto -lcapstone -lm

all: binScan

binScan: main.o commandLine.o err.o parseElf.o md5.o entropy.o disasm.o unpass.o
	$(CC) main.o commandLine.o err.o parseElf.o md5.o entropy.o disasm.o unpass.o -o binScan $(LFLAGS)

main.o: main.c 
	$(CC) $(CFLAGS) main.c 

commandLine.o: commandLine.c 
	$(CC) $(CFLAGS) commandLine.c

err.o: err.c
	$(CC) $(CFLAGS) err.c

parseElf.o: parseElf.c  
	$(CC) $(CFLAGS) parseElf.c -lcapstone -lelf

md5.o: md5.c
	$(CC) $(CFLAGS) md5.c

entropy.o: entropy.c
	$(CC) $(CFLAGS) entropy.c

disasm.o: disasm.c
	$(CC) $(CFLAGS) disasm.c
 
unpass.o: unpass.asm
	nasm -f elf32 unpass.asm

.PHONY clean:
	rm *.o binScan 
