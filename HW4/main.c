#include <stdio.h>
#include <stdlib.h>
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
   while (fgets(buffer,1024,train_data) != NULL)
      push(&table,buffer,34);
   return 0;
}
