CC=gcc
CFLAGS=-Wall -std=gnu99

all:tcp-client tcp-server udp-client
tcp-client:
	$(CC) $(CFLAGS) -o tcp-client tcp-client.c
	
tcp-server:
	$(CC) $(CFLAGS) -o tcp-server tcp-server.c


udp-client:
	$(CC) $(CFLAGS) -o udp-client udp-client.c
	
udp-server:
	$(CC) $(CFLAGS) -o udp-server udp-server.c
		
debug:clean
	$(CC) $(CFLAGS) -g -o network-src main.c
stable:clean
	$(CC) $(CFLAGS) -o network-src main.c
clean:
	rm -vfr *~ tcp-client tcp-server
