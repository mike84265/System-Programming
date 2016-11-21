#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.h"
#include <errno.h>
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

   pid_t pid;
   while (1) {
      fgets(buf,sizeof(buf),stdin);
      #ifdef DEBUG
      fprintf(stderr,"judge %d > %s\n", n, buf);
      #endif
      for (int i=0;i<4;++i)
         init(&player[i]);
      sscanf(buf,"%d %d %d %d\n",&player[0].id, &player[1].id, &player[2].id, &player[3].id);
      if (player[0].id == -1)
         break;
      sprintf(str,"judge%s.FIFO",argv[1]);
      if (mkfifo(str,0600) != 0) {
         if (errno == EEXIST) {
            unlink(str);
            mkfifo(str,0600);
         }
      }
      for (int i=0;i<4;++i) {
         sprintf(str,"judge%s_%c.FIFO",argv[1],'A'+i);
         if (mkfifo(str,0600) != 0) {
            if (errno == EEXIST) {
               unlink(str);
               mkfifo(str,0600);
            }
         }
         player[i].ikey = rnGen(65536);
         #ifdef DEBUG
         fprintf(stderr,"player[%d].ikey = %d\n",i,player[i].ikey);
         #endif
         sprintf(player[i].ckey,"%d",player[i].ikey);
      }
      for (int i=0;i<4;++i) {
         if ( (pid = fork()) > 0) {
            // Parent process
            player[i].pid = pid;
         } else if (pid == 0) {
            // Child process
            sprintf(str,"%c",'A'+i);
            execl("./player.out","player.out",argv[1],str,player[i].ckey,(char*)0);
            exit(0);
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
         sleep(1);
         #ifdef DEBUG
         fprintf(stderr,"Reading response from %s...\n",argv[1]);
         #endif
         read(rFIFO,buf,sizeof(buf));
         #ifdef DEBUG
         fprintf(stderr,"Response from %s: %s\n",argv[1], buf);
         #endif
         char bufLine[10][128];
         char* pch = strtok(buf,"\n");
         int len = 0;
         while (pch != NULL) {
            strcpy(bufLine[len],pch);
            pch = strtok(NULL,"\n");
            ++len;
         }
         for (int j=0;j<len;++j) {
            char tempChar;
            int tempKey,tempNum;
            sscanf(bufLine[j],"%c %d %d\n",&tempChar,&tempKey,&tempNum);
            #ifdef DEBUG
            fprintf(stderr,"tempChar = %c, tempKey = %d, tempNum = %d\n",tempChar,tempKey,tempNum);
            #endif
            int index = tempChar - 'A';
            assert(index>=0 && index < 4);
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
         sprintf(str,"");
         for (int i=0;i<4;++i)
            sprintf(str,"%s%d ",str,player[i].resNum);
         str[strlen(str)-1] = '\n';
         for (int j=0;j<4;++j)
            write(wFIFO[j],str,strlen(str));
         for (int i=0;i<3;++i) {
            if (cumNum[i] == 1) {
               for (int j=0;j<4;++j) {
                  if (player[j].resNum == 2*i+1)
                     player[j].score += 2*i+1;
               }
            }
         }
         #ifdef DEBUG
         for (int i=0;i<4;++i)
            fprintf(stderr,"%c->%d/%d ",'A'+i, player[i].resNum,player[i].score);
         fprintf(stderr,"\n");
         #endif
         for (int j=0;j<4;++j) {
            if (player[j].resNum == 0) {
               fprintf(stderr,"No reply received from player %c.",'A'+j);
               // Should punish this case.
            }
            player[j].resNum = 0;
         }
      } // End of game
      for (int i=0;i<4;++i) {
         pid_t pid = wait(NULL);
         close(wFIFO[i]);
         #ifdef DEBUG
         fprintf(stderr,"Process %d terminates.\n",pid);
         #endif
      }
      close(rFIFO);
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
         #ifdef DEBUG
         fprintf(stderr,"judge %s > big_judge : %d %d\n",argv[1], player[i].id,player[i].rank+1);
         #endif
      }
      sprintf(str,"judge%s.FIFO",argv[1]);
      close(rFIFO);
      unlink(str);
      for (int i=0;i<4;++i) {
         sprintf(str,"judge%s_%c.FIFO",argv[1],'A'+i);
         close(wFIFO[i]);
         unlink(str);
      }
   } // End of while
}
