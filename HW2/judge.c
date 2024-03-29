/*   b03901078  蔡承佑   */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <signal.h>
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
   FILE *rfFIFO;
   Player player[4];

   pid_t pid;
   while (1) {
      fgets(buf,sizeof(buf),stdin);
      #if DEBUG>=2
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
         #if DEBUG>=3
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
            execl("./player","player",argv[1],str,player[i].ckey,(char*)0);
            exit(0);
         }
      }
      sprintf(str,"judge%s.FIFO",argv[1]);
      rFIFO = open(str, O_RDONLY);
      rfFIFO = fdopen(rFIFO,"r");
      // set rfFIFO to unbuffered
      setbuf(rfFIFO,NULL);
      for (int i=0;i<4;++i) {
         sprintf(str,"judge%s_%c.FIFO",argv[1],'A'+i);
         wFIFO[i] = open(str, O_WRONLY);
      }
      // Game start
      int cumNum[3];
      int validPlayer = 4;
      for (int i=0;i<20;++i) {
         cumNum[0] = cumNum[1] = cumNum[2] = 0;
         char bufLine[10][128];
         char tempChar;
         int tempKey,tempNum;
         fd_set rset;
         struct timeval tv;
         tv.tv_sec = 3;
         tv.tv_usec = 0;
         for (int i=0;i<validPlayer;++i) {
            #if DEBUG>=3
            fprintf(stderr,"Reading response from %s...\n",argv[1]);
            #endif
            FD_ZERO(&rset);
            FD_SET(rFIFO,&rset);
            int n;
            n = select(100,&rset,NULL,NULL,&tv);
            if (n == 0) {
               validPlayer = i;
               break;
            }
            fgets(bufLine[i],sizeof(bufLine[i]),rfFIFO);
            sscanf(bufLine[i],"%c %d %d\n",&tempChar, &tempKey, &tempNum);
            #if DEBUG>=3
            fprintf(stderr,"n = %d, tempChar = %c, tempKey = %d, tempNum = %d\n",n,tempChar,tempKey,tempNum);
            // fprintf(stderr,"tempChar = %c, tempKey = %d, tempNum = %d\n",tempChar,tempKey,tempNum);
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
         strcpy(str,"");
         for (int i=0;i<4;++i) {
            if (player[i].resNum == 0) {
               if (player[i].pid != 0) {
                  fprintf(stderr,"No response from player %c.\n",'A'+i);
                  fprintf(stderr,"Kill %d\n",player[i].pid);
                  kill(player[i].pid,SIGABRT);
                  wait(NULL);
                  player[i].pid = 0;
               }
            }
         }
         for (int i=0;i<4;++i) {
            char buf2[64];
            sprintf(buf2,"%d ",player[i].resNum);
            strcat(str,buf2);
         }
         str[strlen(str)-1] = '\n';
         for (int j=0;j<4;++j)
            if (player[j].resNum != 0)
               write(wFIFO[j],str,strlen(str));
         for (int i=0;i<3;++i) {
            if (cumNum[i] == 1) {
               for (int j=0;j<4;++j) {
                  if (player[j].resNum == 2*i+1)
                     player[j].score += 2*i+1;
               }
            }
         }
         #if DEBUG>=2
         for (int i=0;i<4;++i)
            fprintf(stderr,"%s_%c->%d/%d ",argv[1],'A'+i, player[i].resNum,player[i].score);
         fprintf(stderr,"\n");
         #endif
         for (int j=0;j<4;++j) {
            player[j].resNum = 0;
         }
      } // End of game
      for (int i=0;i<validPlayer;++i) {
         pid_t pid = wait(NULL);
         #if DEBUG>=4
         fprintf(stderr,"Process %d terminates.\n",pid);
         #endif
      }
      close(rFIFO);
      fclose(rfFIFO);
      int cmpScore[4];
      for (int i=0;i<4;++i) {
         cmpScore[i] = player[i].score;
         close(wFIFO[i]);
      }
      qsort(cmpScore,4,sizeof(int),compareInt);
      for (int i=0;i<4;++i) {
         for (int j=3;j>=0;--j) {
            if (cmpScore[j] == player[i].score) {
               player[i].rank = j;
               break;
            }
         }
         // j = [0..3], rank = [1..4]
      }
      sprintf(buf,"%d %d\n%d %d\n%d %d\n%d %d\n",
         player[0].id, player[0].rank+1,
         player[1].id, player[1].rank+1,
         player[2].id, player[2].rank+1,
         player[3].id, player[3].rank+1);
      #if DEBUG>=2
      fprintf(stderr,"judge %s > big_judge : %s\n",argv[1],buf);
      #endif
      printf("%s",buf);
      fflush(stdout);
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
