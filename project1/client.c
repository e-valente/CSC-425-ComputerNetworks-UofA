/*
  Copyright - emanuelvalente@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>


#define MAXPAYLOAD 10000 

struct header {
  int sec;
  int usec;
  int length;
};

struct packet {
  struct header hdr;
  char payload[MAXPAYLOAD];
};


double calculateDeltaTime(struct timeval start, struct timeval end) {
  double milliseconds;
  int seconds;
  int usecs;
  
  seconds = end.tv_sec - start.tv_sec;
  usecs = end.tv_usec - start.tv_usec;
  
  if(end.tv_usec < start.tv_usec) {
    seconds --;
    usecs += 1000000;
  }
  
  milliseconds = 0.00000;
  milliseconds += seconds * 1000.0;
  milliseconds += usecs * 0.001;
  
  return milliseconds;
}


void error(char *msg){
  perror(msg);
  exit(1);
}

int startTCPclient(char *serverIPAddr, int port)
{
  int sockfd, bytes_sent, bytes_received;
  struct sockaddr_in serv_addr;
  struct packet mypacket, recv_packet;
  struct timeval startTime, endTime;
  double rtt;
  int c;
  
  
  sockfd = socket (AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
    error ("ERROR opening socket\n");
  
  
  //0 is the "constant"
  memset(&serv_addr, 0, sizeof(serv_addr));
  
  serv_addr.sin_family = AF_INET;
  //convert and copy server's ip addr into serv_addr
  if(inet_pton(AF_INET, serverIPAddr, &serv_addr.sin_addr) < 0)
    error ("ERROR, copying ip addr\n");
  
  serv_addr.sin_port = htons(port);
  // connect to server
  if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
    error ("ERROR connecting");
  
  // send message to server
  memset(&mypacket, 0, sizeof(struct packet));
 
  //gets paylod from the user
  scanf("%[^\n]s", mypacket.payload);
  //flush the input buffer
  while ((c = getchar()) != EOF && c != '\n');

  //sends packet
  gettimeofday(&startTime, NULL);
  mypacket.hdr.length = strlen(mypacket.payload);
  mypacket.hdr.sec = startTime.tv_sec;
  mypacket.hdr.usec = startTime.tv_usec;
  bytes_sent = send(sockfd, &mypacket, sizeof(struct packet), 0);
  if (bytes_sent < 0) 
    error ("ERROR writing to socket");
 
  
  memset(&recv_packet, 0, sizeof(struct packet));
 
  // receive the header
  bytes_received = recv(sockfd, &recv_packet.hdr, sizeof(struct header), 0);
  //at theis point we know the payload's size
  //receives payload
  bytes_received = recv(sockfd, recv_packet.payload, sizeof(char) * recv_packet.hdr.length, 0);
  gettimeofday(&endTime, NULL);
  
  startTime.tv_sec = recv_packet.hdr.sec;
  startTime.tv_usec = recv_packet.hdr.usec;
  rtt = calculateDeltaTime(startTime, endTime);
  if (bytes_received < 0) 
    error("ERROR reading from socket");

  //echo the sent message
  fprintf(stdout,"%s\n",recv_packet.payload);
  fprintf(stdout,"%.3lf\n", rtt);

   
  close (sockfd);
  
  return 0;
  
  
}


void startUDPclient(char *serverIPAddr, int port)
{
  int sockfd, bytes_sent, bytes_received;
  struct sockaddr_in serv_addr;
  struct packet mypacket, recv_packet;
  struct timeval startTime, endTime;
  double rtt;
  int c;
  
  sockfd = socket (AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) 
    error ("ERROR opening socket\n");
 
  //0 is the "constant"
  memset(&serv_addr, 0, sizeof(serv_addr));
  
  serv_addr.sin_family = AF_INET;
  //convert and copy server's ip addr into serv_addr
  if(inet_pton(AF_INET, serverIPAddr, &serv_addr.sin_addr) < 0)
    error ("ERROR, copying ip addr\n");
 
  serv_addr.sin_port = htons(port);
  
  //UDP has not to use CONNECT 
  // send message to server
  //gets paylod from the user
  scanf("%[^\n]s", mypacket.payload);
  //flush the input buffer
  while ((c = getchar()) != EOF && c != '\n');
  mypacket.hdr.length = strlen(mypacket.payload);
  gettimeofday(&startTime, NULL);
  mypacket.hdr.sec = startTime.tv_sec;
  mypacket.hdr.usec = startTime.tv_usec;
  
  bytes_sent = sendto(sockfd, &mypacket, sizeof(struct packet), 0, 
		  (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  if (bytes_sent < 0) 
    error ("ERROR writing to socket");
 
  
  bytes_received = recvfrom(sockfd, &recv_packet, sizeof(struct packet), 0, 
	    NULL, NULL);
  
   if (bytes_received < 0) 
    error("ERROR reading from socket");
 
 
  gettimeofday(&endTime, NULL);
  
  //start time is the time we just got from the server (that it echoed from client)
  startTime.tv_sec = recv_packet.hdr.sec;
  startTime.tv_usec = recv_packet.hdr.usec;
  rtt = calculateDeltaTime(startTime, endTime);

  //echo the sent message
  fprintf(stdout, "%s\n",recv_packet.payload);
  fprintf(stdout,"%.3lf\n", rtt);
 
  close (sockfd);
  
}


int main(int argc, char *argv[]) {


  if ((argc != 4) || ((strcmp(argv[1], "tcp") != 0) 
    && (strcmp(argv[1], "udp") != 0))) {
      fprintf(stderr, "Usage: %s <tcp|udp> <server ip> <server port>\n", argv[0]);
    exit (1);
  }

  if(strcmp(argv[1], "tcp") == 0)
    while(1) startTCPclient(argv[2], atoi(argv[3]));
  
  
  if(strcmp(argv[1], "udp") == 0)   
    while(1) startUDPclient(argv[2], atoi(argv[3]));
    
 
  return 0;
}