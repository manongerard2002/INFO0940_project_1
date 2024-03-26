#ifndef schedulingLogic_h
#define schedulingLogic_h

#include <stdbool.h>

#include "graph.h"
#include "stats.h"
#include "simulation.h"
#include "schedulingReadyQueues.h"

// Cannot include computer.h because of circular dependency
// -> forward declaration of Computer
typedef struct Computer_t Computer;


/* ---------------------------- Scheduler struct ---------------------------- */

typedef struct Scheduler_t Scheduler;


/* -------------------------- getters and setters -------------------------- */

int getWaitQueueCount(void);

/* -------------------------- init/free functions -------------------------- */

//ajout nbProcesses
Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount, int nbProcesses);
void freeScheduler(Scheduler *scheduler);


/* -------------------------- scheduling functions ------------------------- */

/**
 * Put the process in the queue number queueNbr
*/
void putProcessInReadyQueue(Scheduler *scheduler, int queueNbr, int processIndex);

/**
 * Get the first process index from the given queues, without dequeuining it.
 *
 * @param SchedulingReadyQueue The SchedulingReadyQueue from which to get the process index.
 * 
 * @return The process index, or -1 if the SchedulingReadyQueue is empty.
 */
int topReadyQueue(Scheduler *scheduler);

/**
 * Put the processes that arrived in the simulation at that time in the readyqueue
*/
void processArrived(Scheduler *scheduler, Workload* workload, int time);

#endif // schedulingLogic_h
