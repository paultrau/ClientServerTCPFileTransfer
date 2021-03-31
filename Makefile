CC=gcc
CFLAGS=-Wall

all: server client

OTHER_OBSS = parallelTools.o

server: server.o $(OTHER_OBSS)

client: client.o $(OTHER_OBSS)

parallelTools.o: parallelTools.c parallelTools.h
	$(CC) -c parallelTools.c

server.o: server.c
	$(CC) -c server.c

client.o: client.c
	$(CC) -c client.c

clean:
	rm -f server client server.o client.o parallelTools.o out.txt