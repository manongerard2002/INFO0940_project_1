// This is where you should implement most of your code.
// You will have to add function declarations in the header file as well to be
// able to call them from simulation.c.

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "computer.h"
#include "schedulingLogic.h"
#include "utils.h"
#include "schedulingAlgorithms.h"
#include "schedulingReadyQueues.h"

#define NB_WAIT_QUEUES 1

/* --------------------------- struct definitions -------------------------- */

struct Scheduler_t
{
    // This is not the ready queues, but the ready queue algorithms
    SchedulingAlgorithm **readyQueueAlgorithms;
    int readyQueueCount;
    SchedulingReadyQueue **readyQueues; //need to change to a generic queue which acts depending on its type
};

/* ---------------------------- static functions --------------------------- */

/* -------------------------- getters and setters -------------------------- */

int getWaitQueueCount(void)
{
    return NB_WAIT_QUEUES;
}

/* -------------------------- init/free functions -------------------------- */

//ajout de workload
Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount, int nbProcesses)
{
    Scheduler *scheduler = malloc(sizeof(Scheduler));
    if (!scheduler)
    {
        return NULL;
    }

    scheduler->readyQueueAlgorithms = readyQueueAlgorithms;
    scheduler->readyQueueCount = readyQueueCount;
    scheduler->readyQueues = (SchedulingReadyQueue **) malloc(readyQueueCount * sizeof(SchedulingReadyQueue *));
    for (int i=0; i<readyQueueCount; i++)
    {
        scheduler->readyQueues[i] = initSchedulingReadyQueue(nbProcesses);
    }

    return scheduler;
}

void freeScheduler(Scheduler *scheduler)
{
    for (int i = 0; i < scheduler->readyQueueCount; i++)
    {
        free(scheduler->readyQueueAlgorithms[i]);
        free(scheduler->readyQueues[i]);
    }
    free(scheduler->readyQueueAlgorithms);
    free(scheduler->readyQueues);
    free(scheduler);
}

/* -------------------------- scheduling functions ------------------------- */

void putProcessInReadyQueue(Scheduler *scheduler, int queueNbr, int processIndex) {
    if (scheduler->readyQueueAlgorithms[queueNbr]->type == FCFS)
    {
        enqueueSchedulingReadyQueueFCFS(scheduler->readyQueues[queueNbr], processIndex);
    }
}

int topReadyQueue(Scheduler *scheduler) {
    for (int i=0; i <= scheduler->readyQueueCount; i++)
    {
        if (!isEmptySchedulingReadyQueue(scheduler->readyQueues[i]))
            if (scheduler->readyQueueAlgorithms[i]->type == FCFS)
                return topSchedulingReadyQueueFCFS(scheduler->readyQueues[i]);
    }
    return -1;
}

void processArrived(Scheduler *scheduler, Workload *workload, int time)
{
    for (int i = 0; i < getProcessCount(workload); i++)
    {
        int pid = getPIDFromWorkload(workload, i);
        if (getProcessStartTime(workload, pid) == time)
            putProcessInReadyQueue(scheduler, 0, i);
    }
}