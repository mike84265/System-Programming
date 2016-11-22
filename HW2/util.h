/*   b03901078  蔡承佑   */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#ifndef UTIL_H
#define UTIL_H
void combination(const int n, const int r, char*** ret);
int checkPlayer(char* comb, int* pList);

typedef struct {
   int score;
   int id;
   char ckey[8];
   int ikey;
   int pid;
   int resNum;
   int rank;
} Player;

typedef struct {
   int id;
   int score;
} Record;

void init(Player*);
int comparePlayer(const void* A, const void* B);
int compareInt(const void* A, const void* B);
int compareRecord(const void* A, const void* B);
int rnGen(const int range);
#endif
