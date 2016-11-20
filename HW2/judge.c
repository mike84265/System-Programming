#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"
int main(int argc, char** argv)
{
   if (argc != 2) {
      fprintf(stderr,"Usage: %s <judge_id>\n", argv[0]);
      exit(1);
   }
   #ifdef DEBUG
   int n = atoi(argv[1]);
   char buf[1024];
   if (fgets(buf,sizeof(buf),stdin) != NULL) {
      printf("judge %d > %s\n", n, buf);
   }
   #endif
   char str[32];
   int rFIFO, wFIFO[4];
   sprintf(str,"judge%s.FIFO",argv[1]);
   rFIFO = mkfifo(str,O_CREAT | O_RDONLY);
   for (int i=0;i<4;++i) {
      sprintf(str,"judge%s_%c.FIFO",argv[1],'A'+i);
      wFIFO[i] = mkfifo(str,O_CREAT | O_WRONLY);
   }

   pid_t pid;
   Player player[4];
   while (1) {
      scanf("%d %d %d %d\n",&player[0].id, &player[1].id, &player[2].id, &player[3].id);
      if (player[0].id == -1)
         exit(0);
      for (int i=0;i<4;++i) {
         if ( (pid = fork()) > 0) {
            // Parent process
            player[i].pid = pid;
         } else if (pid == 0) {
            // Child process
            sprintf(str,"%c",'A'+i);
            player[i].ikey = rnGen(65536);
            sprintf(player[i].ckey,"%d",player[i].ikey);
            execl("./player.out","player.out",argv[1],str,player[i].ckey,(char*)0);
            break;
         }
      }
      // Game start
      int cumNum[3];
      for (int i=0;i<20;++i) {
         cumNum[0] = cumNum[1] = cumNum[2] = 0;
         for (int j=0;j<4;++j) {
            #ifdef DEBUG
            fprintf(stderr,"Reading response from %s_%c...\n",argv[1],'A'+i);
            #endif
            read(rFIFO,player[i].resMsg,32);
            #ifdef DEBUG
            fprintf(stderr,"Response from %s_%c: %s --\n",argv[1],'A'+i,player[i].resMsg);
            #endif
            int temp;
            sscanf(player[i].resMsg,"%s %d %d",str,&temp,&player[i].resNum);
            if (temp != player[i].ikey) {
               fprintf(stderr,"Key error: player%c, expected %d, got %d.\n",'A'+i,player[i].ikey,temp);
            }
            switch (player[i].resNum) {
             case 1:
               ++cumNum[0];
               break;
             case 3:
               ++cumNum[1];
               break;
             case 5:
               ++cumNum[2];
               break;
             default:
               fprintf(stderr,"player %c at judge %s error: %d\n",'A'+i,argv[1],player[i].resNum);
               break;
            }
         }
         sprintf(str,"");
         for (int i=0;i<4;++i)
            sprintf(str,"%s%d ",str,player[i].resNum);
         for (int j=0;j<4;++j)
            write(wFIFO[i],str,strlen(str));
         for (int i=0;i<3;++i) {
            if (cumNum[i] == 1) {
               for (int j=0;j<4;++j) {
                  if (player[j].resNum == 2*i+1)
                     player[j].score += 2*i+1;
               }
            }
         }
      }
      int cmpScore[4];
      for (int i=0;i<4;++i)
         cmpScore[i] = player[i].score;
      qsort(cmpScore,4,sizeof(int),compareInt);
      for (int i=0;i<4;++i) {
         for (int j=3;j>=0;--j) {
            if (cmpScore[j] == player[i].score) {
               player[i].rank = j;
               break;
            }
         }
         printf("%d %d\n",player[i].id,player[i].rank);
      }
   }
}
