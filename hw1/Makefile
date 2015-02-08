# I am a comment, and I want to say that the variable CC will be
# the compiler to use.
CC=gcc
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler.
CFLAGS= -Wall -c

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