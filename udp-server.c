/*
  This is a simple server program that uses datagram sockets (UDP). It listens on port
  8888, receives 1 short message from a client, then replies to client and exits.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

/*
 Prints error message and exits.
 */
void error(char *msg){
  perror(msg);
  exit(1);
}

int main(int argc, char * argv[]){
  
  int sockfd, bytes_sent, bytes_received;
  // stores the length of client's sockaddr struct
  socklen_t clientlen;
  // receive buffer
  char buffer[256];
  // structure that contains info about internet addr
  struct sockaddr_in serv_addr, client_addr;
  // server's reply
  char *reply = "Hello client";
  
  if(argc < 2) {
    fprintf(stderr, "Usage: %s port_to_listen\n", argv[0]);
    exit(1);
  }

  // create a datagram socket (UDP)
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if(sockfd < 0)
    error ("ERROR opening socket");
 
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  // don't need a specific address, any of the network interface will do
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  // set port number, and stores it in network byte order
  serv_addr.sin_port = htons(atoi(argv[1]));

  // bind socket to an address
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) 
    error("ERROR on binding");
  memset(&client_addr, 0, sizeof(client_addr));
  // set length of client address
  clientlen = sizeof(client_addr);
  // 0 out the receive buffer
  memset(buffer, 0, sizeof(buffer));

  // UDP is connectionless, don't need to listen and accept

  // receive data from client and puts it in buffer
  // let recvfrom fill out where the packet came from and put it in client_addr
  bytes_received = recvfrom (sockfd, buffer, sizeof (buffer), 0, (struct sockaddr *) &client_addr, &clientlen);
  if (bytes_received < 0)
    error("ERROR reading from socket");
  
  printf ("Client said: %s\n",buffer);
  // send client a reply with newly filled out client_addr
  bytes_sent = sendto(sockfd, reply, sizeof(char) * strlen(reply), 0, (struct sockaddr *) &client_addr, clientlen);
  if (bytes_sent < 0)
    error("ERROR writing to socket");

  close (sockfd); 
  return 0; 
}