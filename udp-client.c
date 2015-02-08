/*
  This is a simple client program that uses stream sockets (TCP) to communicate to a given server
  who is listening on port 8888. It sends 1 short message to the server and, then receives the
  server's reply.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h> 
#include <unistd.h>

/*
  Prints error message and then exits.
 */
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  int sockfd, bytes_sent, bytes_received;
  char recv_buffer [256];
  // structure that contains info about internet addr
  struct sockaddr_in serv_addr;

  if(argc < 4) {
    fprintf(stderr, "Usage: %s <server addr> <server port> <message>\n",argv[0]);
    exit(1);
  }
  
  char *send_buffer = argv[3];
  
  // create socket
  sockfd = socket (AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error ("ERROR opening socket\n");
  
  // 0 out the struct
  memset (&serv_addr, 0, sizeof (serv_addr));
  // domain is Internet
  serv_addr.sin_family = AF_INET;

  // copy convert and copy server's ip addr into serv_addr
  if(inet_pton(AF_INET, argv [1], &serv_addr.sin_addr) < 0)
    error ("ERROR, copying ip\n");
  // set server's port number, stores it in network byte order
  serv_addr.sin_port = htons(atoi(argv[2]));
  
  // send message to server
  bytes_sent = sendto (sockfd, send_buffer, strlen(send_buffer), 0, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

  if (bytes_sent < 0) 
    error ("ERROR writing to socket");
  // 0 out receive buffer
  memset (recv_buffer, 0, sizeof (char) * 256);
  // receive message from server
  // arg 5 and 6 (src sockaddr and its len) are filled in by recvfrom
  // since there is not need for this to be filled out in the context of this program
  // we just pass in NULL
  bytes_received = recvfrom(sockfd, recv_buffer, sizeof (recv_buffer), 0, NULL, NULL);

  if (bytes_received < 0) 
    error("ERROR reading from socket");

  printf("Server said: %s\n",recv_buffer);

  close (sockfd);
  return 0;
}