#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
int main()
{
   char buf[1024];
   scanf("%s",buf);
   int fd = open(buf,O_RDONLY);
   if (fd < 0) {
      printf("File not exist!\n");
      exit(1);
   }
   int n;
   while ( (n = read(fd,buf,sizeof(buf))) > 0)
      write(1,buf,n);
}
