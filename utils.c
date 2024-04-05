// You can use these methods and add new ones not related to the simulation if
// you want.

#include "utils.h"

void printVerbose(const char *str)
{
    if (VERBOSE)
    {
        printf("%s", str);
    }
}

int min(int a, int b)
{
    return a < b ? a : b;
}

int min3(int a, int b, int c)
{
    return min(min(a, b), c);
}


Node *CreateNode(int data, int priority)
{
   Node *node = malloc(sizeof(Node));

   if (node == NULL)
   {
      fprintf(stderr, "Memory allocation failed\n");
      return NULL;
   }
   node->data = data;
   node->priority = priority;
   node->next = NULL;
   node->prev = NULL;

   return node;
}

void enqueue(Node **head, int newData, int priority)
{
    Node *newNode = CreateNode(newData, priority);
    if (!newNode)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    Node *last = *head;
    newNode->data = newData;
    newNode->priority = priority;
    newNode->next = NULL;

    if (*head == NULL)
    {
        newNode->prev = NULL;
        *head = newNode;
        return;
    }

    while (last->next != NULL)
    {
        last = last->next;
    }
    last->next = newNode;
    newNode->prev = last;
    return; 
}


