#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
int main(int argc, char** argv)
{
   if (argc != 2) {
      fprintf(stderr,"Usage: %s <judge_id>\n", argv[0]);
      exit(1);
   }
   int n = atoi(argv[1]);
   char buf[1024];
   if (fgets(buf,sizeof(buf),stdin) != NULL) {
      printf("judge %d > %s\n", n, buf);
   }
}
