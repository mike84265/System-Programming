#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
int main()
{
   char buf[1024];
   char filename[1024];
   char* c;
   read(0,buf,sizeof(buf));
   if ((c = strstr(buf,"\n")) != 0)
      *c = '\0';
   if ( (c = strstr(buf,"filename=")) == 0) {
      fprintf(stderr,"query format error!\n");
      exit(1);
   } else {
      memmove(filename,c+9,strlen(c)); 
   }
   int fd = open(filename,O_RDONLY);
   if (fd < 0) {
      fprintf(stderr,"File not exist!\n");
      exit(2);
   }
   int n;
   while ( (n = read(fd,buf,sizeof(buf))) > 0) {
      #ifdef SLOW
      sleep(1);
      #endif
      write(1,buf,n);
   }
   close(fd);
}
