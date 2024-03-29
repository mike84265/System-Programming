/*   b03901078  蔡承佑   */
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include "util.h"
int main(int argc, char** argv)
{
   if (argc != 3) {
      fprintf(stderr, "usage: %s <judge_num> <player_num>\n", argv[0]);
      exit(1);
   }

   const int numJudge = atoi(argv[1]);
   const int numPlayer = atoi(argv[2]);
   if (numPlayer < 4) {
      fprintf(stderr, "Number of players cannot less than 4.\n");
      exit(1);
   }
   char buf[1024];
   pid_t pid;
   pid_t* judgePID = (pid_t*)calloc(numJudge+1,sizeof(pid_t));
   int** judgePipe = (int**)calloc((numJudge+1)*2,sizeof(int*));
   int* pList = (int*)calloc(numPlayer+1, sizeof(int));
   int* jList = (int*)calloc(numJudge+1, sizeof(int));
   int* score = (int*)calloc(numPlayer+1, sizeof(int));
   char** comb;
   combination(numPlayer,4,&comb);
   int num_of_comb = 1;
   for (int i=0;i<4;++i)
      num_of_comb *= (numPlayer-i);
   num_of_comb /= 24;

   for (int i=1;i<=numJudge;++i) {
      // 2i: parent(R), child(W);  2i+1: child(R), parent(W)
      judgePipe[2*i] = (int*)calloc(2,sizeof(int));
      judgePipe[2*i+1] = (int*)calloc(2,sizeof(int));
      pipe(judgePipe[2*i]);
      pipe(judgePipe[2*i+1]);
      if ( (pid = fork()) > 0) {
         // Parent process
         judgePID[i] = pid;
         close(judgePipe[2*i][1]);
         close(judgePipe[2*i+1][0]);
      } else if (pid == 0) {
         // Child process
         close(judgePipe[2*i][0]);
         close(judgePipe[2*i+1][1]);
         if (dup2(judgePipe[2*i][1], STDOUT_FILENO) != STDOUT_FILENO)
            fprintf(stderr,"dup2 STDOUT error\n");
         if (dup2(judgePipe[2*i+1][0], STDIN_FILENO) != STDIN_FILENO)
            fprintf(stderr,"dup2 STDIN error\n");
         char str[4];
         sprintf(str,"%d",i);
         if (execl("./judge","judge",str,(char*)0) < 0)
            fprintf(stderr,"exec error\n");
         exit(0);
      } else {
         fprintf(stderr,"fork error!\n");
      }
   }
   // Assigning judges:
   // 1. Check the judge is free (by record)
   // 2. Check the players of that combination is all free
   // 3. Assign and record on pList and jList
   // 4. Using select to get responses from child processes.
   fd_set rset;
   FD_ZERO(&rset);
   while (1) {
      int end = 1;
      for (int i=1;i<=numJudge;++i) {
         FD_SET(judgePipe[2*i][0],&rset);
      }
      for (int i=1;i<=numJudge;++i) {
         if (jList[i] == 1) continue;
         for (int j=0;j<num_of_comb;++j) {
            if (comb[j] != NULL)
               end = 0;
            else
               continue;
            if (checkPlayer(comb[j],pList) == 0) {
               int p[4];
               sscanf(comb[j],"%d %d %d %d\n",&p[0],&p[1],&p[2],&p[3]);
               for (int k=0;k<4;++k)
                  pList[p[k]] = 1;
               jList[i] = 1;
               #if DEBUG>=1
               printf("big_judge > %d : %s",i,comb[j]);
               #endif
               write(judgePipe[2*i+1][1], comb[j], strlen(comb[j]));
               comb[j] = NULL;
               break;
            }
         }
      }
      if (end == 1) break;
      assert(select(1024,&rset,NULL,NULL,NULL) != -1);
      // One of judge has over the game:
      // 1. Get scores from the pipe.
      // 2. Reset all status:
      //    (1) pList
      //    (2) jList
      for (int i=1;i<=numJudge;++i) {
         if (FD_ISSET(judgePipe[2*i][0],&rset)) {
            read(judgePipe[2*i][0], buf, sizeof(buf));
            #if DEBUG>=1
            fprintf(stderr,"big_judge < %s",buf);
            #endif
            char bufLine[4][64];
            char* pch = strtok(buf,"\n");
            int len = 0;
            while (pch != NULL && len<4) {
               strcpy(bufLine[len],pch);
               pch = strtok(NULL,"\n");
               ++len;
            }
            for (int j=0;j<4;++j) {
               int id, rank;
               sscanf(bufLine[j],"%d %d\n",&id,&rank);
               #if DEBUG>=1
               fprintf(stderr,"id=%d, rank=%d\n",id,rank);
               #endif
               assert(pList[id] == 1);
               pList[id] = 0;
               score[id] += 4-rank; 
            }
            jList[i] = 0;
            break;
         }
      }
   }
   for (int i=1;i<=numJudge;++i) {
      sprintf(buf,"-1 -1 -1 -1\n");
      write(judgePipe[2*i+1][1],buf,strlen(buf));
      pid_t p = wait(NULL);
      #if DEBUG>=1
      fprintf(stderr,"Process %d terminates.\n",p);
      #endif
   }
   Record record[30];
   for (int i=1;i<=numPlayer;++i) {
      record[i].id = i;
      record[i].score = score[i];
   }
   qsort((record+1),numPlayer,sizeof(Record),compareRecord);
   for (int i=1;i<=numPlayer;++i) {
      printf("%d %d\n",record[i].id,record[i].score);
   }
   free(judgePID);
   for (int i=0;i<(numJudge+1)*2;++i)
      free(judgePipe[i]);
   free(judgePipe);
   free(pList);
   free(jList);
   free(score);
   for (int i=0;i<num_of_comb;++i)
      free(comb[i]);
   free(comb);
}
