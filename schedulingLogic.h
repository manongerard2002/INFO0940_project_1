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

/**
 * Initializes a scheduler with the given ready queue algorithms.
 *
 * @param readyQueueAlgorithms An array of pointers to SchedulingAlgorithm objects representing the ready queue algorithms.
 * @param readyQueueCount The number of ready queue algorithms in the array.
 * @return A pointer to the initialized Scheduler object.
 */
//ajout nbProcesses: need to be deleted if LL implemented as no limitation in size anymore
Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount, int nbProcesses);


/**
 * Frees the memory allocated for a Scheduler object.
 *
 * @param scheduler The Scheduler object to be freed.
 */
void freeScheduler(Scheduler *scheduler);


/* -------------------------- scheduling functions ------------------------- */

/**
 * Put the process in the queue number queueNbr
*/
void putProcessInReadyQueue(Scheduler *scheduler, int queueNbr, PCB *pcb);

/**
 * Get the first process pid from the given queues, without dequeuining it.
 *
 * @param scheduler The scheduler.
 * 
 * @return The process PCB, or NULL if the SchedulingReadyQueue is empty.
 */
PCB *topReadyQueue(Scheduler *scheduler);

PCB *dequeueReadyQueue(Scheduler *scheduler);

//int allProcessesInReadyQueues(Scheduler *scheduler, int* allProcesses);

void handleProcessForCPUArrived(Scheduler *scheduler, PCB *pcb);

void assignProcessesToResources(Computer *computer, Workload *workload, int time, ProcessGraph *graph, AllStats *stats);

void putProcessOnCPU(Workload *workload, Computer *computer, int coreIndex, ProcessGraph *graph, PCB *pcb, int time);
/**
 * Performs the First-Come, First-Served (FCFS) scheduling algorithm on the given computer.
 *
 * @param computer The computer on which the scheduling algorithm will be performed.
 * @return The total time taken to complete all processes in the computer.
 */
int FCFSalgo(Computer *computer);

#endif // schedulingLogic_h
