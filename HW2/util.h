#include <stdio.h>
#include <stdlib.h>
void combination(const int n, const int r, char*** ret);
int checkPlayer(char* comb, int* pList);
void err_exit(const char* msg);

typedef struct {
   int score;
   int id;
   char ckey[8];
   int ikey;
   int pid;
   char resMsg[32];
   int resNum;
   int rank;
} Player;
void init(Player*);
int comparePlayer(const void* A, const void* B);
int compareInt(const void* A, const void* B);
int rnGen(const int range);
