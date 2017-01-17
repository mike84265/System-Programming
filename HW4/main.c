#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"
#define DATA_SIZE 30000

int main(int argc, char** argv)
{
   if (argc < 2) {
      fprintf(stderr,"%s <train_data>\n", argv[0]);
      exit(1);
   }
   Table table;
   init(&table, DATA_SIZE);
   FILE* train_data = fopen(argv[1], "r");
   char buffer[1024];
   int i, j;
   int (*cmp[34])(const void*, const void*);
   initCmp(cmp);
   while (fgets(buffer,1024,train_data) != NULL)
      push(&table,buffer,35);
   printf("size = %d\n",table._rowNum);
   qsort(table._data, table._rowNum, sizeof(double*), cmp[31]);
   for (int i=0;i<table._rowNum;++i) {
      printf("%f\t%f\t%f\n",table._data[i][31], table._data[i][34], 
         impurity(&table,i));
   }
   // int pthread_create(pthread_t *restrict tidp, const pthread_attr_t *restrict attr,
   //    void *(*start_rtn)(void *), void *restrict arg); 
}
