CC=gcc
CFLAGS=-std=c11 -O3 -I../unix

KEY=00112233445566778899aabbccddeeff
IV=00112233445566778899aabbccddeeff

all: client

client: client.o aes.o currentprobe-client.o
	$(CC) $(CFLAGS) client.o aes.o currentprobe-client.o -o client

client.o: client.c
	$(CC) $(CFLAGS) -o $@ -c $<	

aes.o: ../unix/aes.c
	$(CC) $(CFLAGS) -c ../unix/aes.c -o aes.o

currentprobe-client.o: currentprobe-client.c currentprobe-client.h
	$(CC) -c $(CFLAGS) -o currentprobe-client.o currentprobe-client.c

run: client
	./client $(KEY) $(IV) ../unix/plaintext output.txt

clean:
	rm -r ./*.o
	rm client
