#include "DoubleLinkedList.h"

Node *CreateNode()
{
   Node *node = malloc(sizeof(Node));

   if (node == NULL)
   {
      fprintf(stderr, "Memory allocation failed\n");
      return NULL;
   }

   node->data = 0;
   node->next = NULL;
   node->prev = NULL;

   return node;
}

void printList(Node *node)
{
   while (node != NULL)
   {
      printf("%d ", node->data);
      node = node->next;
   }
}

void insertFront(Node **head, int newData)
{
   // 1. Create a new node
   Node *newNode = CreateNode(); // Call the improved CreateNode function
   // 2. Check for memory allocation failure
   if (!newNode)
   {
      fprintf(stderr, "Memory allocation failed\n");
      return; // Indicate error without memory leak
   }

   // 3. Set data in the new node
   newNode->data = newData;
   // 4. Update next and prev pointers of the new node
   newNode->next = *head;
   newNode->prev = NULL;
   // 5. Update head pointer if the list was not empty
   if (*head != NULL)
   {
      (*head)->prev = newNode; // Update previous pointer of the old head
   }
   // 6. Update head pointer to point to the new node
   *head = newNode;
}

void insertAfter(Node *head, int nodeData, int newData)
{
   // 1. Check if the list is empty
   if (head == NULL)
   {
      fprintf(stderr, "List is empty, cannot insert after a node\n");
      return;
   }

   // 2. Traverse the list to find the node with the specified nodeData
   Node *current = head;
   while (current != NULL && current->data != nodeData)
   {
      current = current->next;
   }

   // 3. Check if the nodeData was found
   if (current == NULL)
   {
      fprintf(stderr, "Node with data %d not found\n", nodeData);
      return;
   }

   // 4. Create a new node
   Node *newNode = CreateNode(); // Assuming proper memory allocation in CreateNode
   if (!newNode)
   {
      fprintf(stderr, "Memory allocation failed\n");
      return;
   }
   newNode->data = newData;

   // 5. Update next and prev pointers of the new node
   newNode->next = current->next;
   newNode->prev = current;

   // 6. Update prev pointer of the node following the found node (if it exists)
   if (current->next != NULL)
   {
      current->next->prev = newNode;
   }

   // 7. Update next pointer of the found node
   current->next = newNode;
}

void insertBefore(Node **head, int nodeData, int newData)
{
   // 1. Check if the list is empty
   if (*head == NULL)
   {
      fprintf(stderr, "List is empty, cannot insert before a node\n");
      return;
   }

   // 2. Find the node with the specified nodeData
   Node *current = *head;
   while (current != NULL && current->data != nodeData)
   {
      current = current->next;
   }

   // 3. Check if the nodeData was found
   if (current == NULL)
   {
      fprintf(stderr, "Node with data %d not found\n", nodeData);
      return;
   }

   // 4. Create a new node
   Node *newNode = CreateNode(); // Assuming proper memory allocation in CreateNode
   if (!newNode)
   {
      fprintf(stderr, "Memory allocation failed\n");
      return;
   }
   newNode->data = newData;

   // 5. Update next and prev pointers of the new node
   newNode->prev = current->prev;
   newNode->next = current;

   // 6. Update prev pointer of the found node
   current->prev = newNode;

   // 7. Update next pointer of the node preceding the found node (if it exists)
   if (newNode->prev != NULL)
   {
      newNode->prev->next = newNode;
   }
   else
   {
      // If there's no previous node, the new node becomes the head
      *head = newNode;
   }
}

void append(Node **head, int newData)
{

   // 1. Create a new node
   Node *newNode = CreateNode();
   if (!newNode)
   {
      fprintf(stderr, "Memory allocation failed\n");
      return;
   }
   Node *last = *head;
   newNode->data = newData;
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

void deleteNode(Node **head, Node *nodeDelete)
{
   if (*head == NULL || nodeDelete == NULL)
   {
      return;
   }

   if (*head == nodeDelete)
   {
      *head = nodeDelete->next;
   }

   if (nodeDelete->next != NULL)
   {
      nodeDelete->next->prev = nodeDelete->prev;
   }

   if (nodeDelete->prev != NULL)
   {
      nodeDelete->prev->next = nodeDelete->next;
      // Update the prev pointer of the node following the deleted node if it was the last node before the tail
      if (nodeDelete->next == NULL)
      {
         nodeDelete->prev->next = NULL;
      }
   }

   free(nodeDelete);
   return;
}

void deleteNodeBefore(Node **head, int nodeData)
{
   // Check if the list is empty or has only one node
   if (*head == NULL || (*head)->next == NULL)
   {
      // Handle empty or single-node list
      fprintf(stderr, "List is empty or has only one node, cannot delete node before\n");
      return;
   }

   Node *current = *head;
   Node *prev = NULL;

   // Find the node containing nodeData
   while (current != NULL && current->data != nodeData)
   {
      prev = current;
      current = current->next;
   }

   // Check if nodeData wasn't found or if it's the first node
   if (current == NULL || current == *head)
   {
      fprintf(stderr, "No node to delete before the node with data %d\n", nodeData);
      return;
   }

   // Check if the node to delete is the head node
   if (prev == *head)
   {
      *head = current;
   }
   else
   {
      // Update the pointers to remove the node before the one containing nodeData
      prev->prev->next = current;
      current->prev = prev->prev;
   }

   // Free the memory of the node to delete
   free(prev);
}

void deleteNodeAfter(Node **head, int nodeData)
{
   if (*head == NULL)
   {
      fprintf(stderr, "List is empty, cannot delete node after\n");
      return;
   }
   // Find the node with the specified nodeData
   Node *current = *head;
   while (current != NULL && current->data != nodeData)
   {
      current = current->next;
   }
   if (current == NULL)
   {
      fprintf(stderr, "Node with data %d not found\n", nodeData);
      return;
   }
   // Delete the node after the node with nodeData
   Node *nodeToDelete = current->next;
   if (nodeToDelete == NULL)
   {
      fprintf(stderr, "No node to delete after the node with data %d\n", nodeData);
      return;
   }

   current->next = nodeToDelete->next;
   if (nodeToDelete->next != NULL)
   {
      nodeToDelete->next->prev = current;
   }

   free(nodeToDelete);
}

void DeleteList(Node **head)
{
   Node *current = *head;
   while (current != NULL)
   {
      Node *temp = current->next;
      free(current);
      current = temp;
   }
   *head = NULL;
}
