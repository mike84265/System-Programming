#include "util.h"
#include <stdlib.h>
#include <string.h>

void init(Table* table, int capacity)
{
   table->_rowNum = table->_colNum = 0;
   table->_capacity = capacity;
   table->_data = (double**)malloc(capacity * sizeof(double*));
}

void push(Table* table, char* data, int length)
{
   if (table->_colNum == 0)
      table->_colNum = length;
   if (table->_rowNum == table->_capacity) {
      table->_capacity *= 2;
      table->_data = realloc(table->_data, table->_capacity * sizeof(double));
   }
   table->_data[table->_rowNum] = (double*)malloc(length * sizeof(double));
   char* pch = data;
   pch = strtok(pch," ");
   table->_data[table->_rowNum][0] = atof(pch);
   for (int i=1;i<length;++i) {
      pch = strtok(NULL," ");
      table->_data[table->_rowNum][i] = atof(pch);
   }
   ++table->_rowNum;
}
