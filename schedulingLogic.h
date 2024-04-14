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
 * @return A pointer to the initialized Scheduler object.
 */
//ajout nbProcesses: need to be deleted if LL implemented as no limitation in size anymore
Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount, Workload *workload);


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

Node *dequeueReadyQueue(Scheduler *scheduler);

bool processInReadyQueues(Scheduler *scheduler, int pid);

//debug
void printReadyQueues(Scheduler *scheduler);

void handleSchedulerEvents(Computer *computer, int time, AllStats *stats);

void handleProcessForCPU(Scheduler *scheduler, Node *node);

void handleProcessForDisk(Scheduler *scheduler, Node *node);

void assignProcessesToResources(Computer *computer, Workload *workload);

void putProcessOnCPU(Workload *workload, Computer *computer, int coreIndex, Node *node);

void putProcessOnDisk(Workload *workload, Computer *computer, Node *node);

void advanceSchedulingTime(int time, int next_time, Computer *computer);

#endif // schedulingLogic_h
