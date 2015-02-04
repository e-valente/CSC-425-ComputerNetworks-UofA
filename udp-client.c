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
  // socket file descriptor, bytes sent and received
  int sockfd, bytes_sent, bytes_received;
  // port number the server is listening on
  int portnum = 8888;
  // structure that contains info about internet addr
  struct sockaddr_in serv_addr;

  // message to send to server
  char * send_buffer = "Hello server";
  // buffer used to receive a message from server
  char recv_buffer [256];

  if (argc < 2) {
    printf ("Usage: client server_ip\n");
    exit (0);
  }

  // create socket
  sockfd = socket (AF_INET, SOCK_STREAM, 0);
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
  serv_addr.sin_port = htons (portnum);
  // connect to server
  if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) 
    error ("ERROR connecting");
  // send message to server
  bytes_sent = send (sockfd, send_buffer, strlen (send_buffer), 0);

  if (bytes_sent < 0) 
    error ("ERROR writing to socket");
  // 0 out receive buffer
  memset (recv_buffer, 0, sizeof (char) * 256);
  // receive message from server
  bytes_received = recv (sockfd, recv_buffer, sizeof (recv_buffer), 0);
  if (bytes_received < 0) 
    error("ERROR reading from socket");

  printf("Server said: %s\n",recv_buffer);

  close (sockfd);
  return 0;
}