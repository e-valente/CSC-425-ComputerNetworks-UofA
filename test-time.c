#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

getStringFromUser(char **mystring_) {
  
  char c;
  int i;
  char *mystring;
  
  mystring = (char*)malloc(sizeof(char));
  
  i = 0;
  while(c!='\n') {
    c = getc(stdin);     //read the input from keyboard standard input
    //re-allocate (resize) memory for character read to be stored
     mystring = (char*)realloc(mystring,i+1*sizeof(char));
     mystring[i++] = c;  //store read character by making pointer point to c

  }
  //overwrite \n to \0
   mystring[i-1]='\0';   //at the end append null character to mark end of string
  
   fprintf(stdout,"you typed inside: %s\n", mystring);
   
   *mystring_ = mystring;
   

}
  
int main(int argc, char* argv[]) {
  
  struct timeval startTime, endTime;
  int seconds, micro;
  char *buffer;
  
  fprintf(stdout, "Before...\n\n");
  gettimeofday(&startTime, NULL);
  getStringFromUser(&buffer);
  
  //fprintf(stdout, "seconds %d\nmicro %d\n", startTime.tv_sec, startTime.tv_usec);
  gettimeofday(&endTime, NULL);
  
  seconds = endTime.tv_sec - startTime.tv_sec;
  micro = endTime.tv_usec - endTime.tv_usec;
  
  fprintf(stdout,"you typed: %s\n", buffer);
  fprintf(stdout, "seconds %d\nmicro %d\n", seconds, micro);
  
  free(buffer);
  fprintf(stdout, "%d\n", strlen("Hello World"));
  
  
  
  
 return 0; 
}