#ifndef schedulingReadySchedulingReadyQueues_h
#define schedulingReadySchedulingReadyQueues_h

#include <stdbool.h>

//Probably need a linked list to be able to add in the "middle" when there is priority
typedef struct SchedulingReadyQueue_t SchedulingReadyQueue;
struct SchedulingReadyQueue_t
{
    int *processesIndex;
    int head;
    int tail;
    int size; //can fill up to size - 1 to still be able to distinguish fulll and empty
};

/**
 * Initializes a new SchedulingReadyQueue with the specified size.
 *
 * @param size: the size of the SchedulingReadyQueue.
 * 
 * @return a pointer to the new SchedulingReadyQueue, or NULL if memory allocation fails.
 */
SchedulingReadyQueue *initSchedulingReadyQueue(int size);

/**
 * Free the memory allocated for the SchedulingReadyQueue.
 *
 * @param readyQueue: the SchedulingReadyQueue to free
 */
void freeSchedulingReadyQueue(SchedulingReadyQueue *readyQueue);

/**
 * Checks if a SchedulingReadyQueue is empty.
 *
 * @param SchedulingReadyQueue The SchedulingReadyQueue to check.
 * 
 * @return True if the SchedulingReadyQueue is empty, false otherwise.
 */
bool isEmptySchedulingReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue);

/**
 * Checks if a SchedulingReadyQueue is full.
 *
 * @param SchedulingReadyQueue The SchedulingReadyQueue to check.
 * 
 * @return True if the SchedulingReadyQueue is full, false otherwise.
 */
bool isFullSchedulingReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue);

/*FCFS*/
/**
 * Adds a process index to the specified SchedulingReadyQueue.
 *
 * @param SchedulingReadyQueue The SchedulingReadyQueue to which the process will be added.
 * @param processIndex The process index to be added to the SchedulingReadyQueue.
 * 
 * @return True if the enqueue suceeded
 */
bool enqueueSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue, int processIndex);

/**
 * Dequeue SchedulingReadyQueue and returns the first process index from the given SchedulingReadyQueue.
 *
 * @param SchedulingReadyQueue The SchedulingReadyQueue from which to dequeue the process index.
 * 
 * @return The process index, or -1 if the SchedulingReadyQueue is empty.
 */
int dequeueSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue);

/**
 * Get the first process index from the given SchedulingReadyQueue, without dequeuining it.
 *
 * @param SchedulingReadyQueue The SchedulingReadyQueue from which to get the process index.
 * 
 * @return The process index, or -1 if the SchedulingReadyQueue is empty.
 */
int topSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue);

#endif // schedulingReadySchedulingReadyQueues_h