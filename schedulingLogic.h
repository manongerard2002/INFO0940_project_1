#ifndef schedulingLogic_h
#define schedulingLogic_h

#include <stdbool.h>

#include "graph.h"
#include "stats.h"
#include "simulation.h"

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
Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount, int time);


/**
 * Frees the memory allocated for a Scheduler object.
 *
 * @param scheduler The Scheduler object to be freed.
 */
void freeScheduler(Scheduler *scheduler);


/* -------------------------- scheduling functions ------------------------- */

/**
 * Performs the First-Come, First-Served (FCFS) scheduling algorithm on the given computer.
 *
 * @param computer The computer on which the scheduling algorithm will be performed.
 * @return The total time taken to complete all processes in the computer.
 */
int FCFS(Computer *computer);

#endif // schedulingLogic_h
