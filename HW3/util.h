/*  B03901078  蔡承佑  */
#ifndef UTIL_H
#define UTIL_H
struct Node {
   struct Node* next;
   struct Node* prev;
   int val;
}; 

typedef struct {
   struct Node* head;
   int size;
} List;

typedef struct {
   char time_string[100];
   int pid;
   char filename[128];
} TimeInfo;

void init(List* list);
void push(List* list, int val);
int erase(List* list, int val);
int empty(List* list);

#endif
