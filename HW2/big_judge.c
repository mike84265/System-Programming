#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include "util.h"
int main(int argc, char** argv)
{
   if (argc != 3) {
      fprintf(stderr, "usage: %s <judge_num> <player_num>\n", argv[0]);
      exit(1);
   }

   const int numJudge = atoi(argv[1]);
   const int numPlayer = atoi(argv[2]);
   pid_t pid;
   pid_t* judgePID = (pid_t*)calloc(numJudge,sizeof(pid_t));
   int** judgePipe = (int**)calloc(numJudge*2,sizeof(int*));
   int* pList = (int*)calloc(numPlayer, sizeof(int));
   char** comb;
   combination(numPlayer,4,&comb);

   for (int i=0;i<numJudge;++i) {
      // 2i: parent(R), child(W);  2i+1: child(R), parent(W)
      judgePipe[2*i] = (int*)calloc(2,sizeof(int));
      judgePipe[2*i+1] = (int*)calloc(2,sizeof(int));
      pipe(judgePipe[2*i]);
      pipe(judgePipe[2*i+1]);
      if ( (pid = fork()) != 0) {
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
         if (execl("./judge.out","judge.out",str,(char*)0) < 0)
            fprintf(stderr,"exec error\n");
         break;
      } else {
         fprintf(stderr,"fork error!\n");
      }
   }
   #ifdef DEBUG
   if (pid != 0) {
      for (int i=0;i<numJudge;++i) {
         char buf[1024];
         sprintf(buf,"Testing judge %d...\n",i);
         fprintf(stderr,"%s\n",buf);
         if (write(judgePipe[2*i+1][1], buf, strlen(buf)) != strlen(buf))
            fprintf(stderr,"write error\n");
         if (read(judgePipe[2*i][0], buf, sizeof(buf)) <= 0)
            fprintf(stderr,"read error\n");
         fprintf(stderr,"Response from judge %d: %s",i,buf);
      }
   }
   #endif
   for (int i=0;i<numJudge;++i) {
      pid_t p = wait(NULL);
      fprintf(stderr,"Process %d terminates.\n",p);
   }
}
