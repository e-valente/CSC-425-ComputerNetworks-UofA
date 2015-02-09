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


void error(char *msg){
  perror(msg);
  exit(1);
}

void startTCPserver(int port) {
  
  int sockfd, session, bytes_sent, bytes_received;
  struct header myheader;
  char *payload;
  socklen_t clientlen;
  struct sockaddr_in serv_addr, client_addr;
 
  // create a stream socket (TCP)
  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    error ("ERROR opening socket");
  
  
  memset (&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  // don't need a specific address, any of the network interface will do
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  // set port number, and stores it in network byte order
  serv_addr.sin_port = htons(port);

  // bind socket to an address
  if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    error("ERROR on binding");
  
  // listen to socket, set maximum pending connection is 5
  listen (sockfd, 5);
  // set length of client address
  clientlen = sizeof(client_addr);
  // accept first incoming connection on the pending queue, returns a new file descriptor
  session = accept(sockfd, (struct sockaddr *) &client_addr, &clientlen);

  if(session < 0)
    error("ERROR on accept");
  memset(&myheader, 0, sizeof(struct header));
  
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
  
}

void startUDPserver(int port) {
  
  int sockfd, bytes_sent, bytes_received;
  struct header myheader;
  char *payload;
  socklen_t clientlen;
  struct sockaddr_in serv_addr, client_addr;
 
  // create a stream socket (UDP)
  sockfd = socket (AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0)
    error ("ERROR opening socket");
  
  
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  // don't need a specific address, any of the network interface will do
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  // set port number, and stores it in network byte order
  serv_addr.sin_port = htons(port);

  // bind socket to an address
  if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    error("ERROR on binding");
 
   //zeroing our buffer to receive data
  memset(&myheader, 0, sizeof(struct header));
  // set length of client address
  clientlen = sizeof(client_addr);
 
  // receive header from client 
  bytes_received = recvfrom(sockfd, &myheader, sizeof(struct header), 0,
    (struct sockaddr*)&client_addr, &clientlen);
  
  if (bytes_received < 0)
    error("ERROR reading from socket");
  
  // receive header from client 
  payload = (char*)malloc(sizeof(char) * myheader.length);
  bytes_received = recvfrom(sockfd, payload, sizeof(char) * myheader.length, 0,
     (struct sockaddr*)&client_addr, &clientlen);
  
  if (bytes_received < 0)
    error("ERROR reading from socket");
  
  fprintf(stdout, "%d\n", myheader.length);

  
  bytes_sent = sendto(sockfd, payload, sizeof(char) * strlen(payload), 0,
    (struct sockaddr*)&client_addr, clientlen);
  if (bytes_sent < 0)
    error("ERROR writing to socket");

  free(payload);
  close (sockfd); 
  
}


int main(int argc, char * argv[]){

  if(argc != 3) {
    fprintf(stderr, "Usage: %s <tcp|udp> port_to_listen\n", argv[0]);
    exit(1);
  }

  if(strcmp(argv[1], "tcp") == 0)   
    startTCPserver(atoi(argv[2]));
  
  if(strcmp(argv[1], "udp") == 0)   
    startUDPserver(atoi(argv[2]));
    
  return 0; 
}