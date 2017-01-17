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
int rnGen(const int range);
double impurity(const Table* table, int row);

int cmp_1(const void* pA, const void* pB);
int cmp_2(const void* pA, const void* pB);
int cmp_3(const void* pA, const void* pB);
int cmp_4(const void* pA, const void* pB);
int cmp_5(const void* pA, const void* pB);
int cmp_6(const void* pA, const void* pB);
int cmp_7(const void* pA, const void* pB);
int cmp_8(const void* pA, const void* pB);
int cmp_9(const void* pA, const void* pB);
int cmp_10(const void* pA, const void* pB);
int cmp_11(const void* pA, const void* pB);
int cmp_12(const void* pA, const void* pB);
int cmp_13(const void* pA, const void* pB);
int cmp_14(const void* pA, const void* pB);
int cmp_15(const void* pA, const void* pB);
int cmp_16(const void* pA, const void* pB);
int cmp_17(const void* pA, const void* pB);
int cmp_18(const void* pA, const void* pB);
int cmp_19(const void* pA, const void* pB);
int cmp_20(const void* pA, const void* pB);
int cmp_21(const void* pA, const void* pB);
int cmp_22(const void* pA, const void* pB);
int cmp_23(const void* pA, const void* pB);
int cmp_24(const void* pA, const void* pB);
int cmp_25(const void* pA, const void* pB);
int cmp_26(const void* pA, const void* pB);
int cmp_27(const void* pA, const void* pB);
int cmp_28(const void* pA, const void* pB);
int cmp_29(const void* pA, const void* pB);
int cmp_30(const void* pA, const void* pB);
int cmp_31(const void* pA, const void* pB);
int cmp_32(const void* pA, const void* pB);
int cmp_33(const void* pA, const void* pB);

void initCmp(int (*cmp[])(const void*, const void*));
#endif
