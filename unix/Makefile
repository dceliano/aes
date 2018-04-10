CC=gcc
CFLAGS=-O3 -std=c11

KEY=00112233445566778899aabbccddeeff
IV=00112233445566778899aabbccddeeff


all: aes

aes: main.o aes.o
	$(CC) -o aes main.o aes.o

main.o: main.c aes.h
	$(CC) -c $(CFLAGS) -o main.o main.c

aes.o: aes.c aes.h
	$(CC) -c $(CFLAGS) -o aes.o aes.c

run: aes
	./aes $(KEY) $(IV) plaintext output.txt

.PHONY: clean
clean:
	rm -f aes *.o

