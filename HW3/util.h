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
} TimeInfo;

void init(List* list);
void push(List* list, int val);
int erase(List* list, int val);
#endif
