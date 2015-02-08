/*
  Copyright - emanuelvalente@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

struct header {
  uint32_t sec;
  uint32_t usec;
  int length;
};


struct packet {
  uint32_t sec;
  uint32_t usec;
  int length;
  //char *msg;
  char msg[256];
};

void error(char *msg){
  perror(msg);
  exit(1);
}

int main(int argc, char * argv[]){
 
  int sockfd, session, bytes_sent, bytes_received;
  struct header myheader;
  char *payload;
  

  socklen_t clientlen;
  char buffer[256];
  struct sockaddr_in serv_addr, client_addr;
  // server's reply
  char *reply = "Hello client";

  if(argc < 2) {
    fprintf(stderr, "Usage: %s port_to_listen\n", argv[0]);
    exit(1);
  }
  // create a stream socket (TCP)
  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error ("ERROR opening socket");
  
  
  memset (&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  // don't need a specific address, any of the network interface will do
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  // set port number, and stores it in network byte order
  serv_addr.sin_port = htons(atoi(argv[1]));

  // bind socket to an address
  if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    error("ERROR on binding");
  
  // listen to socket, set maximum pending connection is 5
  listen (sockfd, 5);
  // set length of client address
  clientlen = sizeof(client_addr);
  // accept first incoming connection on the pending queue, returns a new file descriptor
  session = accept(sockfd, (struct sockaddr *) &client_addr, &clientlen);

  if (session < 0)
    error("ERROR on accept");
  memset (buffer, 0, sizeof (buffer));
  
  // receive header from client 
  bytes_received = recv(session, &myheader, sizeof(struct header), 0);
  if (bytes_received < 0)
    error("ERROR reading from socket");
  
  // receive header from client 
  payload = (char*)malloc(sizeof(char) * myheader.length);
  bytes_received = recv(session, payload, sizeof(char) * myheader.length, 0);
  if (bytes_received < 0)
    error("ERROR reading from socket");
  
  fprintf(stdout, "%d\n", myheader.length);

  
  bytes_sent = send (session, payload, sizeof(char) * strlen(payload), 0);
  if (bytes_sent < 0)
    error("ERROR writing to socket");

  free(payload);
  close (session);
  close (sockfd); 
  return 0; 
}