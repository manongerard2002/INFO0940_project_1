#ifndef queues_h
#define queues_h

#include <stdbool.h>
#include "process.h"

typedef struct Node_t Node;
struct Node_t
{
    int queueNbr; //for multilevel: to be able to be put back in the correct queue
    PCB *pcb;
    int executionTime; // for SJF
    int currentQueueExecutionTime; // for --limit
    int currentQueueWaitingTime; //for --age
    struct Node_t *prev;
    struct Node_t *next;
};

typedef struct Queue_t Queue;
struct Queue_t
{
    Node *head;
    Node *tail;
    int size;
};

/**
 * Initializes a new Node with the specified pcb.
 *
 * @param pcb The pcb of the Node.
 * 
 * @return a pointer to the new Node, or NULL if memory allocation fails.
 */
Node *initNode(PCB *pcb);

/**
 * Initializes an empty Queue.
 * 
 * @return a pointer to the new Queue, or NULL if memory allocation fails.
 */
Queue *initQueue(void);

/**
 * Free the memory allocated for the Node.
 * Note: This does not free the PCB as it was not allocated to create the node
 *
 * @param node The node to free
 */
void freeNode(Node *node);

/**
 * Free the memory allocated for the Queue.
 * Note: This does not free the PCBs as they were not allocated to create the queue
 *
 * @param queue The queue to free
 */
void freeQueue(Queue *queue);

/**
 * Checks if a queue is empty.
 *
 * @param queue The queue to check.
 * 
 * @return True if the queue is empty, false otherwise.
 */
bool isEmptyQueue(Queue *queue);

/**
 * Gets thee size of the queue.
 *
 * @param queue The queue.
 * 
 * @return the size of the queue.
 */
int queueSize(Queue *queue);

/**
 * Adds a process node to the given queue at the back.
 *
 * @param queue The queue to which the node will be added.
 * @param node The process node to be added to the queue.
 */
void enqueueNodeFCFS(Queue *queue, Node *node);

/**
 * Adds a process node to the given queue according to the prriority.
 *
 * @param queue The queue to which the node will be added.
 * @param node The process node to be added to the queue.
 */
void enqueueNodePriority(Queue *queue, Node *node);

/**
 * Adds a process node to the given queue accoding to the execution time.
 *
 * @param queue The queue to which the node will be added.
 * @param node The process node to be added to the queue.
 */
void enqueueNodeSJF(Queue *queue, Node *node);

/**
 * Dequeue the first process Node from the given queue.
 *
 * @param queue The queue from which to dequeue the process node.
 * 
 * @return The process node, or NULL if the queue is empty.
 */
Node *dequeueTopNode(Queue *queue);

/**
 * Dequeue a specific Node from the given queue (if present).
 *
 * @param queue The queue from which to dequeue the process node.
 * @param node The process node to dequeue.
 */
void removeNode(Queue *queue, Node *node);

/**
 * Get the first process node from the given queue, without dequeuining it.
 *
 * @param queue The queue from which to get the first node.
 * 
 * @return The process node, or NULL if the Node is empty.
 */
Node *topNode(Queue *queue);

/**
 * Checks if a process pid is inside the queue.
 *
 * @param queue The queue to check.
 * @param pid The process pid.
 * 
 * @return True if the process pid is inside the queue, false otherwise.
 */
bool processInQueue(Queue *queue, int pid);

//debug:
/**
 * Prints the contents of a given node.
 *
 * @param node The node to be printed.
 */
void printNode(Node *node);

/**
 * Prints the elements of a queue.
 *
 * @param queue The queue to be printed.
 */
void printQueue(Queue *queue);

#endif // queues_h