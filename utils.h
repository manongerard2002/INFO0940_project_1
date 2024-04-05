#include <stdio.h>
#include <stdbool.h>

#define VERBOSE 0

typedef struct Node_t Node;


struct Node_t {
    int data; //Data to be stored in the node
    int priority; //Priority of the node
    Node *next; //Pointer to next node
    Node *prev; //Pointer to previous node
};



/**
 * Prints a verbose message to the console.
 *
 * @param str The string to be printed.
 */
void printVerbose(const char *str);


/**
 * Returns the minimum of two integers.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @return The minimum of the two integers.
 */
int min(int a, int b);


/**
 * Returns the minimum value among three integers.
 *
 * @param a The first integer.
 * @param b The second integer.
 * @param c The third integer.
 * @return The minimum value among the three integers.
 */
int min3(int a, int b, int c);


/**
 * Creates a new node with the given data and priority.
 *
 * @param data The data value to be stored in the node.
 * @param priority The priority value associated with the node.
 * @return A pointer to the newly created node.
 */
Node *CreateNode(int data, int priority);


/**
 * Adds a new node with the specified data and priority to the end of the linked list.
 *
 * @param head A pointer to the head of the linked list.
 * @param newData The data to be added to the new node.
 * @param priority The priority of the new node.
 */
void enqueue(Node **head, int newData, int priority);
