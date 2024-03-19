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

/**
 * First-Come, First-Served (FCFS) scheduling algorithm.
 *
 * This function implements the FCFS scheduling algorithm, which is a non-preemptive
 * scheduling algorithm that schedules processes in the order they arrive.
 *
 * @param scheduler The scheduler object.
 * @param workload The workload object containing the list of processes.
 * @param timeQuantum The time quantum for the scheduling algorithm (not used in FCFS).
 */
void FCFS(Scheduler *scheduler, Workload *workload, int timeQuantum);

/**
 * Compares two processes based on their workload.
 *
 * This function compares two processes based on their workload and returns an integer value
 * indicating the result of the comparison. The comparison is performed by comparing the workload
 * values of the two processes.
 *
 * @param w     Pointer to the Workload structure containing the workload information.
 * @param pida  The ID of the first process to compare.
 * @param pidb  The ID of the second process to compare.
 *
 * @return      An integer value indicating the result of the comparison:
 *              - A negative value if the workload of process A is less than the workload of process B.
 *              - Zero if the workload of process A is equal to the workload of process B.
 *              - A positive value if the workload of process A is greater than the workload of process B.
 */
int compareProcesses(Workload *w, int pida, int pidb);

/**
 * Shortest Job First (SJF) scheduling algorithm.
 *
 * This function implements the SJF scheduling algorithm, which selects the
 * process with the shortest burst time from the workload and executes it.
 * If multiple processes have the same burst time, the one that arrived first
 * is selected.
 *
 * @param scheduler The scheduler object.
 * @param workload The workload containing the processes.
 * @param timeQuantum The time quantum for the scheduler (not used in SJF).
 */
void SJF(Scheduler *scheduler, Workload *workload, int timeQuantum);

#endif // schedulingLogic_h
