#ifndef schedulingLogic_h
#define schedulingLogic_h

#include <stdbool.h>

#include "graph.h"
#include "stats.h"
#include "simulation.h"
#include "queues.h"

// Cannot include computer.h because of circular dependency
// -> forward declaration of Computer
typedef struct Computer_t Computer;


/* ---------------------------- Scheduler struct ---------------------------- */

typedef struct Scheduler_t Scheduler;


/* -------------------------- getters and setters -------------------------- */

int getWaitQueueCount(void);

/* -------------------------- init/free functions -------------------------- */

/**
 * Initializes a scheduler with the given ready queue algorithms.
 *
 * @param readyQueueAlgorithms An array of pointers to SchedulingAlgorithm objects representing the ready queue algorithms.
 * @param readyQueueCount The number of ready queue algorithms in the array.
 * @param workload The workload: necessary in case of SJF.
 * @param cpuCoreCount The number of cores of the CPU.
 * @return A pointer to the initialized Scheduler object.
 */
Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount, Workload *workload, int cpuCoreCount);


/**
 * Frees the memory allocated for a Scheduler object.
 *
 * @param scheduler The Scheduler object to be freed.
 */
void freeScheduler(Scheduler *scheduler);


/* -------------------------- scheduling functions ------------------------- */

/**
 * Adds a process to the given queue number queueNbr.
 *
 * @param scheduler The Scheduler object where the queues are.
 * @param queueNbr The index of the queue.
 * @param pcb The process pcb to be added to the queue.
 */
void putprocessInQueue(Scheduler *scheduler, int queueNbr, Node *node);

/**
 * Get the first process node from the given queues, without dequeuining it.
 *
 * @param scheduler The scheduler.
 * 
 * @return The process Node, or NULL if the queue is empty.
 */
Node *topReadyQueue(Scheduler *scheduler);

/**
 * Dequeues the first process node from the given queues.
 *
 * @param scheduler The scheduler.
 * 
 * @return The process Node, or NULL if the queue is empty.
 */
Node *dequeueTopReadyQueue(Scheduler *scheduler);

/**
 * Removes a process node from the given queue.
 *
 * @param scheduler The scheduler.
 * @param queueNbr The index of the queue.
 * @param node The process node to remove.
 */
void removeReadyQueueNode(Scheduler *scheduler, int queueNbr, Node *node);

/**
 * Processes the ready queues.
 *
 * @param scheduler The scheduler.
 * @param pid The pid of the process to process.
 * 
 * @return True if the process was found and processed, false otherwise.
 */
bool processInReadyQueues(Scheduler *scheduler, int pid);


//debug
/**
 * Print the ready queues to the console.
 *
 * @param scheduler The scheduler.
 */
void printReadyQueues(Scheduler *scheduler);


/**
 * Print the ready queues to the console.
 *
 * @param scheduler The scheduler.
 */
void handleSchedulerEvents(Computer *computer, int time, AllStats *stats);

/**
 * Handles the process for the CPU.
 *
 * @param scheduler The scheduler.
 * @param node The process node to handle.
 */
void handleProcessForCPU(Scheduler *scheduler, Node *node);

/**
 * Handles the process for the disk.
 *
 * @param scheduler The scheduler.
 * @param node The process node to handle.
 */
void handleProcessForDisk(Scheduler *scheduler, Node *node);

/**
 * Assigns processes to resources.
 *
 * @param computer The computer.
 */
void assignProcessesToResources(Computer *computer);

/**
 * Handles the process for the CPU.
 *
 * @param computer The computer.
 */
void putProcessOnCPU(Computer *computer, int coreIndex, Node *node);

/**
 * Handles the process for the disk.
 *
 * @param computer The computer.
 */
void putProcessOnDisk(Computer *computer, Node *node);

/**
 * Handles the process for the CPU.
 *
 * @param computer The computer.
 */
void advanceSchedulingTime(int time, int nextTime, Computer *computer);

#endif // schedulingLogic_h
