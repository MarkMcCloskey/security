CC=gcc
CFLAGS=-c -Wall -Wextra 
LFLAGS=-lelf -lssl -lcrypto -lcapstone -lm

all: binScan

binScan: main.o commandLine.o err.o parseElf.o md5.o entropy.o disasm.o
	$(CC) main.o commandLine.o err.o parseElf.o md5.o entropy.o disasm.o -o binScan $(LFLAGS)

main.o: ../src/main.c 
	$(CC) $(CFLAGS) ../src/main.c 

commandLine.o: ../src/commandLine.c
	$(CC) $(CFLAGS) ../src/commandLine.c 

err.o: ../src/err.c
	$(CC) $(CFLAGS) ../src/err.c

parseElf.o: ../src/parseElf.c  
	$(CC) $(CFLAGS) ../src/parseElf.c -lcapstone -lelf

md5.o: ../src/md5.c
	$(CC) $(CFLAGS) ../src/md5.c

entropy.o: ../src/entropy.c
	$(CC) $(CFLAGS) ../src/entropy.c

disasm.o: ../src/disasm.c
	$(CC) $(CFLAGS) ../src/disasm.c

.PHONY clean:
	rm *.o binScan 
