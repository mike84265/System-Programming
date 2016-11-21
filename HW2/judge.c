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
   int n = atoi(argv[1]);
   char buf[1024];
   char str[32];
   int rFIFO, wFIFO[4];
   Player player[4];
   sprintf(str,"judge%s.FIFO",argv[1]);
   mkfifo(str,0600);
   for (int i=0;i<4;++i) {
      sprintf(str,"judge%s_%c.FIFO",argv[1],'A'+i);
      mkfifo(str,0600);
      player[i].ikey = rnGen(65536);
      #ifdef DEBUG
      fprintf(stderr,"player[%d].ikey = %d\n",i,player[i].ikey);
      #endif
      sprintf(player[i].ckey,"%d",player[i].ikey);
   }

   pid_t pid;
   while (1) {
      fgets(buf,sizeof(buf),stdin);
      #ifdef DEBUG
      fprintf(stderr,"judge %d > %s\n", n, buf);
      #endif
      sscanf(buf,"%d %d %d %d\n",&player[0].id, &player[1].id, &player[2].id, &player[3].id);
      if (player[0].id == -1)
         break;
      for (int i=0;i<4;++i) {
         if ( (pid = fork()) > 0) {
            // Parent process
            player[i].pid = pid;
         } else if (pid == 0) {
            // Child process
            sprintf(str,"%c",'A'+i);
            execl("./player.out","player.out",argv[1],str,player[i].ckey,(char*)0);
            break;
         }
      }
      sprintf(str,"judge%s.FIFO",argv[1]);
      rFIFO = open(str, O_RDONLY);
      for (int i=0;i<4;++i) {
         sprintf(str,"judge%s_%c.FIFO",argv[1],'A'+i);
         wFIFO[i] = open(str, O_WRONLY);
      }
      // Game start
      int cumNum[3];
      for (int i=0;i<20;++i) {
         cumNum[0] = cumNum[1] = cumNum[2] = 0;
         sleep(3);
         for (int j=0;j<4;++j) {
            #ifdef DEBUG
            fprintf(stderr,"Reading response from %s...\n",argv[1]);
            #endif
            read(rFIFO,buf,sizeof(buf));
            #ifdef DEBUG
            fprintf(stderr,"Response from %s: \"%s\"\n",argv[1], buf);
            #endif
            char tempChar;
            int tempKey,tempNum;
            sscanf(buf,"%c %d %d\n",&tempChar,&tempKey,&tempNum);
            #ifdef DEBUG
            fprintf(stderr,"tempChar = %c, tempKey = %d, tempNum = %d\n",tempChar,tempKey,tempNum);
            #endif
            int index = tempChar - 'A';
            player[index].resNum = tempNum;
            if (tempKey != player[index].ikey) {
               fprintf(stderr,"Key error: player%c, expected %d, got %d.\n",'A'+index,player[index].ikey,tempKey);
            }
            switch (player[index].resNum) {
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
         #ifdef DEBUG
         fprintf(stderr,"A->%d B->%d C->%d D->%d\n",player[0].resNum,player[1].resNum,player[2].resNum,player[3].resNum);
         #endif
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
      } // End of game
      for (int i=0;i<4;++i) {
         pid_t pid = wait(NULL);
         #ifdef DEBUG
         fprintf(stderr,"Process %d terminates.\n",pid);
         #endif
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
         // j = [0..3], rank = [1..4]
         printf("%d %d\n",player[i].id,player[i].rank+1);
      }
   }
   sprintf(str,"judge%s.FIFO",argv[1]);
   close(rFIFO);
   unlink(str);
   for (int i=0;i<4;++i) {
      sprintf(str,"judge%s_%c.FIFO",argv[1],'A'+i);
      close(wFIFO[i]);
      unlink(str);
   }
}
