#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "util.h"
int main(int argc, char** argv)
{
   if (argc != 4) {
      fprintf(stderr, "Usage: %s <judge_id> <player_index> <random_key>\n",argv[0]);
      exit(1);
   }
   #ifdef DEBUG
   fprintf(stderr, "%s %s %s %s executed\n", argv[0], argv[1], argv[2], argv[3]);
   #endif
   char buf[32];
   int fin, fout;
   sprintf(buf,"judge%s.FIFO",argv[1]);
   if ( (fout = open(buf,O_WRONLY)) < 0) {
      fprintf(stderr, "Opening %s error\n", buf);
      exit(1);
   }
   sprintf(buf,"judge%s_%s.FIFO",argv[1],argv[2]);
   if ( (fin = open(buf,O_RDONLY)) < 0) {
      fprintf(stderr, "Opening %s error\n", buf);
      exit(1);
   }
   for (int i=0;i<20;++i) {
      // Format: index key num
      sprintf(buf,"%s %s %d\n",argv[2],argv[3],rnGen(3)*2+1);
      while (write_lock(fout) == -1) {
         pid_t pid = lock_test(fout);
         #ifdef DEBUG
         fprintf(stderr,"Waiting for %d to write...\n",pid);
         #endif
      }
      write(fout,buf,strlen(buf));
      un_lock(fout);
      #ifdef DEBUG
      fprintf(stderr,"%s_%s > %s",argv[1],argv[2],buf);
      #endif
      read(fin,buf,sizeof(buf));
      #ifdef DEBUG
      fprintf(stderr,"%s_%s < %s",argv[1],argv[2],buf);
      #endif
   }
}
