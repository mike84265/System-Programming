#include <stdio.h>
#ifndef UTIL_H
#define UTIL_H

typedef struct {
   int            _rowNum;
   int            _colNum;
   int            _capacity;
   double**       _data;
} Table;
void init(Table* table, int capacity);
void push(Table* table, char* data, int length);
#endif
