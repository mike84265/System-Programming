#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include "util.h"
int main()
{
   char buf[1024];
   char filename[1024];
   char time_string[100];
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
   int timefd = open("time_info",O_RDWR | O_TRUNC);
   lseek(timefd, sizeof(TimeInfo), SEEK_SET);
   write(timefd,"",1);
   TimeInfo* info = (TimeInfo*)mmap(0, sizeof(TimeInfo), PROT_READ | PROT_WRITE, MAP_SHARED, timefd, 0);
   close(timefd);
   time_t current_time = time(NULL);
   strcpy(time_string, ctime(&current_time));
   memcpy(info->time_string, &time_string, sizeof(time_string)); 
   munmap(info, sizeof(TimeInfo));
   return 0;
}
