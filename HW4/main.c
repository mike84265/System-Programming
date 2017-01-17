#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"
#define DATA_SIZE 30000
int (*cmp[34])(const void*, const void*);

int main(int argc, char** argv)
{
   if (argc < 2) {
      fprintf(stderr,"%s <train_data>\n", argv[0]);
      exit(1);
   }
   Table table;
   init(&table, DATA_SIZE,35);
   FILE* train_data = fopen(argv[1], "r");
   char buffer[1024];
   int i, j;
   initCmp(cmp);

   while (fgets(buffer,1024,train_data) != NULL)
      push(&table,buffer);
   #ifdef DEBUG
   printf("size = %d\n",table._rowNum);
   #endif

   Table sample;
   init(&sample,DATA_SIZE,35);
   for (i=0;i<table._rowNum;++i)
      push_double(&sample, table._data[rnGen(table._rowNum)]);

   Node* root = (Node*)malloc(sizeof(Node));
   root->_table = sample;

   findCut(&root->_table, &root->_dimension, &root->_threshold);
   printf("dim = %d, thr = %f", root->_dimension, root->_threshold);
   free(root);
   // int pthread_create(pthread_t *restrict tidp, const pthread_attr_t *restrict attr,
   //    void *(*start_rtn)(void *), void *restrict arg); 
}
