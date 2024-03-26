#ifndef DOUBLELINKEDLIST_H
#define DOUBLELINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>


typedef struct Node_t Node;

struct Node_t {
   int data;
   Node *next; //Pointer to next node
   Node *prev; //Pointer to previous node
};

/**
 * @brief Creates a new node for a double linked list.
 *
 * This function creates a new node for a double linked list and initializes it with the given number of nodes.
 *
 * @param node A pointer to the node structure.
 * @param nbNodes The number of nodes to be created.
 * @return A pointer to the newly created node.
 */
Node *CreateNode();

/**
 * Prints the elements of a linked list.
 *
 * @param node The head node of the linked list.
 */
void printList(Node *node);

/**
 * @brief Inserts a new node at the front of the list.
 *
 * This function inserts a new node at the front of the list.
 *
 * @param head A pointer to the head of the list.
 * @param newData The data to be inserted in the new node.
 */
void insertFront(Node **head, int newData);

/**
 * Inserts a new node with the specified data after the node with the given data.
 *
 * @param head The head node of the linked list.
 * @param nodeData The data of the node after which the new node should be inserted.
 * @param newData The data to be stored in the new node.
 */
void insertAfter(Node *head, int nodeData, int newData);

/**
 * Inserts a new node with the specified data before the node with the given data.
 *
 * @param head The head node of the linked list.
 * @param nodeData The data of the node before which the new node should be inserted.
 * @param newData The data to be stored in the new node.
 */
void insertBefore(Node** head, int nodeData, int newData);

/**
 * Appends a new node with the given data to the end of the linked list.
 *
 * @param head A pointer to the head of the linked list.
 * @param newData The data to be added to the new node.
 */
void append(Node **head, int newData);

/**
 * Deletes a node from a double linked list.
 *
 * @param head The pointer to the head of the double linked list.
 * @param nodeDelete The node to be deleted.
 */
void deleteNode(Node **head, Node *nodeDelete);

/**
 * Deletes the node before the specified node in a double linked list.
 *
 * @param head Pointer to the head of the double linked list.
 * @param nodeData The data value of the node after which the node should be deleted.
 */
void deleteNodeBefore(Node **head, int nodeData);

/**
 * Deletes the node after the specified node in a double linked list.
 *
 * @param head A pointer to the head of the double linked list.
 * @param nodeData The data of the node after which the node should be deleted.
 */
void deleteNodeAfter(Node **head, int nodeData);

/**
 * @brief Deletes the entire linked list.
 * 
 * This function deletes all the nodes in the linked list and frees the memory allocated for them.
 * 
 * @param head A double pointer to the head of the linked list.
 */
void DeleteList(Node **head);

#endif // DOUBLELINKEDLIST_H
