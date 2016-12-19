#include "util.h"
#include <stdlib.h>
void init(List* list)
{
   list->size = 0;
   list->head = (struct Node*)malloc(sizeof(struct Node));
   list->head->prev = list->head->next = list->head;
   // head is a dummy node
}

void push(List* list, int val)
{
   struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
   newNode->val = val;
   newNode->prev = list->head->prev;
   newNode->next = list->head;
   list->head->prev->next = newNode;
   list->head->prev = newNode;
   ++list->size;
}

int erase(List* list, int val)
{
   if (list->size == 0)
      return -1;
   struct Node* np = list->head->next;
   while (np->val != val) {
      np = np->next;
      if (np == list->head)
         return -1;
   }
   np->next->prev = np->prev;
   np->prev->next = np->next;
   free(np);
   return 0;
}
