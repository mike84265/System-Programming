#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>

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

int rnGen(const int range) 
{
   srand(getpid() * time(NULL) * rand());
   return (int)(range * ( (double)(rand()) / INT_MAX) );
}

double impurity(const Table* table, int row)
{
   int cnt_0=0, cnt_1=0, i;
   double rate, ret = 0;
   for (i=0;i<row;++i) {
      if (table->_data[i][34] == 0)
         ++cnt_0;
      else
         ++cnt_1;
   }
   rate = (double)cnt_0/(cnt_0+cnt_1);
   ret += rate * (1-rate);
   cnt_0 = cnt_1 = 0;
   for (; i<table->_rowNum;++i) {
      if (table->_data[i][34] == 0)
         ++cnt_0;
      else
         ++cnt_1;
   }
   rate = (double)cnt_1/(cnt_0+cnt_1);
   ret += rate * (1-rate);
   return ret;
}

int cmp_1(const void* pA, const void* pB)
{ return ( (*(double**)pA)[1] - (*(double**)pB)[1]); }
int cmp_2(const void* pA, const void* pB)
{ return ( (*(double**)pA)[2] - (*(double**)pB)[2]); }
int cmp_3(const void* pA, const void* pB)
{ return ( (*(double**)pA)[3] - (*(double**)pB)[3]); }
int cmp_4(const void* pA, const void* pB)
{ return ( (*(double**)pA)[4] - (*(double**)pB)[4]); }
int cmp_5(const void* pA, const void* pB)
{ return ( (*(double**)pA)[5] - (*(double**)pB)[5]); }
int cmp_6(const void* pA, const void* pB)
{ return ( (*(double**)pA)[6] - (*(double**)pB)[6]); }
int cmp_7(const void* pA, const void* pB)
{ return ( (*(double**)pA)[7] - (*(double**)pB)[7]); }
int cmp_8(const void* pA, const void* pB)
{ return ( (*(double**)pA)[8] - (*(double**)pB)[8]); }
int cmp_9(const void* pA, const void* pB)
{ return ( (*(double**)pA)[9] - (*(double**)pB)[9]); }
int cmp_10(const void* pA, const void* pB)
{ return ( (*(double**)pA)[10] - (*(double**)pB)[10]); }
int cmp_11(const void* pA, const void* pB)
{ return ( (*(double**)pA)[11] - (*(double**)pB)[11]); }
int cmp_12(const void* pA, const void* pB)
{ return ( (*(double**)pA)[12] - (*(double**)pB)[12]); }
int cmp_13(const void* pA, const void* pB)
{ return ( (*(double**)pA)[13] - (*(double**)pB)[13]); }
int cmp_14(const void* pA, const void* pB)
{ return ( (*(double**)pA)[14] - (*(double**)pB)[14]); }
int cmp_15(const void* pA, const void* pB)
{ return ( (*(double**)pA)[15] - (*(double**)pB)[15]); }
int cmp_16(const void* pA, const void* pB)
{ return ( (*(double**)pA)[16] - (*(double**)pB)[16]); }
int cmp_17(const void* pA, const void* pB)
{ return ( (*(double**)pA)[17] - (*(double**)pB)[17]); }
int cmp_18(const void* pA, const void* pB)
{ return ( (*(double**)pA)[18] - (*(double**)pB)[18]); }
int cmp_19(const void* pA, const void* pB)
{ return ( (*(double**)pA)[19] - (*(double**)pB)[19]); }
int cmp_20(const void* pA, const void* pB)
{ return ( (*(double**)pA)[20] - (*(double**)pB)[20]); }
int cmp_21(const void* pA, const void* pB)
{ return ( (*(double**)pA)[21] - (*(double**)pB)[21]); }
int cmp_22(const void* pA, const void* pB)
{ return ( (*(double**)pA)[22] - (*(double**)pB)[22]); }
int cmp_23(const void* pA, const void* pB)
{ return ( (*(double**)pA)[23] - (*(double**)pB)[23]); }
int cmp_24(const void* pA, const void* pB)
{ return ( (*(double**)pA)[24] - (*(double**)pB)[24]); }
int cmp_25(const void* pA, const void* pB)
{ return ( (*(double**)pA)[25] - (*(double**)pB)[25]); }
int cmp_26(const void* pA, const void* pB)
{ return ( (*(double**)pA)[26] - (*(double**)pB)[26]); }
int cmp_27(const void* pA, const void* pB)
{ return ( (*(double**)pA)[27] - (*(double**)pB)[27]); }
int cmp_28(const void* pA, const void* pB)
{ return ( (*(double**)pA)[28] - (*(double**)pB)[28]); }
int cmp_29(const void* pA, const void* pB)
{ return ( (*(double**)pA)[29] - (*(double**)pB)[29]); }
int cmp_30(const void* pA, const void* pB)
{ return ( (*(double**)pA)[30] - (*(double**)pB)[30]); }
int cmp_31(const void* pA, const void* pB)
{ return ( (*(double**)pA)[31] - (*(double**)pB)[31]); }
int cmp_32(const void* pA, const void* pB)
{ return ( (*(double**)pA)[32] - (*(double**)pB)[32]); }
int cmp_33(const void* pA, const void* pB)
{ return ( (*(double**)pA)[33] - (*(double**)pB)[33]); }

void initCmp(int (*cmp[])(const void*, const void*))
{
   cmp[1] = cmp_1;
   cmp[2] = cmp_2;
   cmp[3] = cmp_3;
   cmp[4] = cmp_4;
   cmp[5] = cmp_5;
   cmp[6] = cmp_6;
   cmp[7] = cmp_7;
   cmp[8] = cmp_8;
   cmp[9] = cmp_9;
   cmp[10] = cmp_10;
   cmp[11] = cmp_11;
   cmp[12] = cmp_12;
   cmp[13] = cmp_13;
   cmp[14] = cmp_14;
   cmp[15] = cmp_15;
   cmp[16] = cmp_16;
   cmp[17] = cmp_17;
   cmp[18] = cmp_18;
   cmp[19] = cmp_19;
   cmp[20] = cmp_20;
   cmp[21] = cmp_21;
   cmp[22] = cmp_22;
   cmp[23] = cmp_23;
   cmp[24] = cmp_24;
   cmp[25] = cmp_25;
   cmp[26] = cmp_26;
   cmp[27] = cmp_27;
   cmp[28] = cmp_28;
   cmp[29] = cmp_29;
   cmp[30] = cmp_30;
   cmp[31] = cmp_31;
   cmp[32] = cmp_32;
   cmp[33] = cmp_33;
}
