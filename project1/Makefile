#emanuelvalente@gmail.com
CC=gcc
CFLAGS=  -c

all: server client

server: server.o
	$(CC) server.o -o server

client: client.o
	$(CC) client.o -o client

server.o: server.c
	$(CC) $(CFLAGS) server.c

hello.o: client.c
	$(CC) $(CFLAGS)  client.c

clean:
	rm *o server client
