/*   b03901078  蔡承佑   */
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
   #if DEBUG>=3
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
   FILE *infile,*outfile;
   infile = fdopen(fin,"r");
   outfile = fdopen(fout,"w");
   int num;
   for (int i=0;i<20;++i) {
      // Format: index key num
      #if DEBUG>=4
      if (strcmp(argv[2],"A") == 0)
         num = 1;
      else if (strcmp(argv[2],"B") == 0)
         num = 3;
      else if (strcmp(argv[2],"C") == 0)
         num = 5;
      else if (strcmp(argv[2],"D") == 0) 
         sleep(5);
      #else
      num = 2*rnGen(3)+1;
      #endif
      fprintf(outfile,"%s %s %d\n",argv[2],argv[3],num);
      fflush(outfile);
      #if DEBUG>=3
      sprintf(buf,"%s %s %d\n",argv[2],argv[3],num);
      fprintf(stderr,"%s_%s > %s",argv[1],argv[2],buf);
      #endif
      read(fin,buf,sizeof(buf));
      #if DEBUG>=3
      fprintf(stderr,"%s_%s < %s",argv[1],argv[2],buf);
      #endif
   }
}
