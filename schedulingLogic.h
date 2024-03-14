#ifndef schedulingLogic_h
#define schedulingLogic_h

#include <stdbool.h>

#include "graph.h"
#include "stats.h"
#include "simulation.h"

// Cannot include computer.h because of circular dependency
// -> forward declaration of Computer
typedef struct Computer_t Computer;

/**
 * @brief Definition of the Queue structure.
 * 
 * This structure represents a queue in the scheduling logic.
 * It is used to store and manage the order of tasks to be executed.
 */
typedef struct Queue_t Queue;

/* ---------------------------- Scheduler struct ---------------------------- */

typedef struct Scheduler_t Scheduler;


/* -------------------------- getters and setters -------------------------- */

int getWaitQueueCount(void);

/* -------------------------- init/free functions -------------------------- */

Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount);
void freeScheduler(Scheduler *scheduler);

/* -------------------------- scheduling functions ------------------------- */


/**
 * Initializes a new queue with the specified size.
 *
 * @param size The size of the queue.
 * @return A pointer to the newly initialized queue, or NULL if memory allocation fails.
 */
Queue *initQueue(int size);

/**
 * Checks if a queue is empty.
 *
 * @param queue The queue to check.
 * @return True if the queue is empty, false otherwise.
 */
bool isEmpty(Queue *queue);

/**
 * Adds a process to the specified queue.
 *
 * @param queue The queue to which the process will be added.
 * @param process The process to be added to the queue.
 */
void enqueue(Queue *queue, PCB *process);

/**
 * Dequeues and returns the first PCB (Process Control Block) from the given queue.
 *
 * @param queue The queue from which to dequeue the PCB.
 * @return The dequeued PCB, or NULL if the queue is empty.
 */
PCB *dequeue(Queue *queue);

#endif // schedulingLogic_h
