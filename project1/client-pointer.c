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




struct header {
  int sec;
  int usec;
  int length;
};

struct packet {
  struct header hdr;
  char *payload;
};
void getStringFromUser(char **mystring_) {
  
  char c;
  int i;
  char *mystring;
  
  mystring = (char*)malloc(sizeof(char));
  
  i = 0;
  while(c!='\n') {
    c = getc(stdin);   
     mystring = (char*)realloc(mystring,i+1*sizeof(char));
     mystring[i++] = c; 

  }
  //overwrite \n to \0
   mystring[i-1]='\0';   
   
   *mystring_ = mystring;
   
}


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

void startTCPclient(char *serverIPAddr, int port)
{
  int sockfd, bytes_sent, bytes_received;
  struct sockaddr_in serv_addr;
  //struct header myheader;
  struct packet mypacket, recv_packet;
  struct timeval startTime, endTime;
  char *payload, *recv_buffer;
  double rtt;
  
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
  getStringFromUser(&mypacket.payload);
  //sends packet
  gettimeofday(&startTime, NULL);
  mypacket.hdr.length = strlen(mypacket.payload);
  mypacket.hdr.sec = startTime.tv_sec;
  mypacket.hdr.usec = startTime.tv_usec;
  bytes_sent = send(sockfd, &mypacket, sizeof(struct packet) + sizeof(char) * strlen(mypacket.payload), 0);
  if (bytes_sent < 0) 
    error ("ERROR writing to socket");
 
  
  memset(&recv_packet, 0, sizeof(struct packet));
 
  // receive the header
  bytes_received = recv(sockfd, &recv_packet.hdr, sizeof(struct header), 0);
  //at theis point we know the payload's size
  recv_packet.payload = (char*)malloc(sizeof(char) * recv_packet.hdr.length);
  //receives payload
  bytes_received = recv(sockfd, recv_packet.payload, sizeof(char) * recv_packet.hdr.length, 0);
  gettimeofday(&endTime, NULL);
  
  rtt = calculateDeltaTime(startTime, endTime);
  if (bytes_received < 0) 
    error("ERROR reading from socket");

  //echo the sent message
  printf("%d\n",recv_packet.hdr.sec);
  printf("%s\n",recv_packet.payload);
  fprintf(stdout,"%.3lf\n", rtt);
 
  free(mypacket.payload);
  free(recv_packet.payload);
  close (sockfd);
  
}


void startUDPclient(char *serverIPAddr, int port)
{
  int sockfd, bytes_sent, bytes_received;
  struct sockaddr_in serv_addr;
  struct header myheader;
  struct timeval startTime, endTime;
  char *payload, *recv_buffer;
  double rtt;
  
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
  getStringFromUser(&payload);
  recv_buffer = (char*)malloc(sizeof(char) * strlen(payload));
  memset(recv_buffer, 0, sizeof(char) * strlen(payload));
  myheader.length = strlen(payload);
  myheader.sec = startTime.tv_sec;
  myheader.usec = startTime.tv_usec;
  
  bytes_sent = sendto(sockfd, &myheader, sizeof(struct header), 0, 
		  (struct sockaddr*) &serv_addr, sizeof(serv_addr));
  if (bytes_sent < 0) 
    error ("ERROR writing to socket");
 

  //sends the payload
  gettimeofday(&startTime, NULL);
  bytes_sent = sendto(sockfd, payload, sizeof(char) * strlen(payload), 0,
      (struct sockaddr*) &serv_addr, sizeof(serv_addr));

  if (bytes_sent < 0) 
    error ("ERROR writing to socket");
  // receive message from server
  
  bytes_received = recvfrom(sockfd, recv_buffer, sizeof(char) * strlen(payload), 0, 
	    NULL, NULL);
 
  gettimeofday(&endTime, NULL);
  rtt = calculateDeltaTime(startTime, endTime);
  
  if (bytes_received < 0) 
    error("ERROR reading from socket");

  //echo the sent message
  printf("%s\n",recv_buffer);
  fprintf(stdout,"%.3lf\n", rtt);
 
  free(payload);
  close (sockfd);
  
}


int main(int argc, char *argv[]) {


  if ((argc != 4) || ((strcmp(argv[1], "tcp") != 0) 
    && (strcmp(argv[1], "udp") != 0))) {
      fprintf(stderr, "Usage: %s <tcp|udp> <server ip> <server port>\n", argv[0]);
    exit (1);
  }

  if(strcmp(argv[1], "tcp") == 0)   
    startTCPclient(argv[2], atoi(argv[3]));
  
  if(strcmp(argv[1], "udp") == 0)   
    startUDPclient(argv[2], atoi(argv[3]));
    
 
  return 0;
}