#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include <string.h>
static void print(int* arr, int r, char* ret)
{
   sprintf(ret,"%d", arr[0]);
   for (int i=1;i<r;++i) {
      sprintf(ret,"%s %d",ret,arr[i]);
   }
   sprintf(ret,"%s\n",ret);
}
void combination(const int n, const int r, char*** ret)
{
   if (n<=4) { *ret = NULL; return; }
   int size = n*(n-1)*(n-2)*(n-3)/24;
   *ret = (char**)calloc(size,sizeof(char*));
   for (int i=0;i<size;++i) {
      (*ret)[i] = (char*)calloc(64,sizeof(char));
   }
   int arr[100];
   // initialize
   for (int i=0;i<r;++i)
      arr[i] = i+1;
   print(arr,r, (*ret)[0]);
   /*
    * Print the array
    * if (!the last index reaches the end)
    *    ++last_index
    * else
    *    search for previous indices j that haven't reached the end
    *    reset indices after j
    *    go back to the first line
    * r-1  ->  n
    * r-2  ->  n-1
    * r-3  ->  n-2
    */
   for (int i=1;i<size;++i) {
      if (arr[r-1] < n) {
         ++arr[r-1];
      } else {
         int j=r-1;
         while (j>=0 && arr[j] >= n-(r-j-1))
            --j;
         if (j<0)
            break;
         ++arr[j];
         for (; j<r-1;++j)
            arr[j+1] = arr[j] + 1;
      }
      print(arr,r,(*ret)[i]);
   }
}

int checkPlayer(char* comb, int* pList)
{
   int p[4];
   sscanf(comb,"%d %d %d %d\n",&p[0],&p[1],&p[2],&p[3]);
   return (p[0] | p[1] | p[2] | p[3]);
}
