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

void putProcessInReadyQueue(Scheduler *scheduler, int queueNbr, int pid) {
    if (scheduler->readyQueueAlgorithms[queueNbr]->type == FCFS)
    {
        enqueueSchedulingReadyQueueFCFS(scheduler->readyQueues[queueNbr], pid);
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

int dequeueReadyQueue(Scheduler *scheduler) {
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        if (!isEmptySchedulingReadyQueue(scheduler->readyQueues[i])) {
            if (scheduler->readyQueueAlgorithms[i]->type == FCFS)
                return dequeueSchedulingReadyQueueFCFS(scheduler->readyQueues[i]); }
    }
    return -1;
}

//Here or in simulation.c
void handleEvents(Computer *computer, Workload *workload, int time, ProcessGraph *graph, AllStats *stats)
{
    //1. Handle event(s): simulator and the scheduler check if an event is triggered at the current time unit and handle it
    //Ex: if a process arrives in the system, the simulator will call the scheduler to put the process in the ready queue.
    printf("beginning of handleEvents\n");
    processArrived(computer->scheduler, workload, time, graph, stats);
    
    printReadyQueue(computer->scheduler->readyQueues[0]);
    //Ex: event = scheduling events, such as a process needing to move to an upper queue because of aging

    for (int j = 0; j < getProcessCount(workload); j++)
    {
        int pid = getPIDFromWorkload(workload, j);
        if (getProcessStartTime(workload, pid) > time)
        {
            int remainingEventTime = getProcessCurEventTimeLeft(workload, pid);
            if (remainingEventTime <= 0)
            {
                printf("need to deal with finish op for pid:%d\n", pid);
            }
        }
    }

    //cpu: switch-in/out
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        if (computer->cpu->cores[i]->switchInTimer == getSwitchInDuration())
        {
            printf("switch-in finshed\n");
            //computer->cpu->cores[i]->state = OCCUPIED;
            addProcessEventToGraph(graph, computer->cpu->cores[i]->PID, time, RUNNING, i);
        }
        else if (computer->cpu->cores[i]->switchOutTimer == getSwitchOutDuration())
        {
            printf("switch-out finshed\n");
            computer->cpu->cores[i]->state = IDLE;
            //put it were it should go
        }
    }
    //Ex: event = hardware events, such as the triggering of an interrupt.
}

void assignProcessesToResources(Computer *computer, Workload *workload, int time, ProcessGraph *graph, AllStats *stats, int *cpuCoresIDLE)
{
    printf("assignProcessToResources\n");
    //The scheduler will check if a process is ready to be executed and will choose what core it should put it on (or not).
    //can't rememeber how to do better in c:
    int nb = 0;
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        cpuCoresIDLE[i] = -1; //reset: could be skipped
        if (computer->cpu->cores[i]->state == IDLE)
        {
            cpuCoresIDLE[nb] = i;
            nb++;
        }
    }
    //printf("number of core idle: %d\n", nb);
    for (int i = 0; i < nb; i++)
    {
        int pid = dequeueReadyQueue(computer->scheduler);
        if (pid != -1)
        {
            printf("selected process: %d\n", pid);
            putProcessOnCPU(computer, cpuCoresIDLE[i], graph, pid, time);
        }
    }
    //The scheduler could also put a process on the disk if it is idle.
    if (computer->disk->state == DISK_IDLE)
    {
        printf("disk isIdle\n");
    }
}

void putProcessOnCPU(Computer *computer, int coreIndex, ProcessGraph *graph, int pid, int time)
{
    printf("putProcessOnCPU with index: %d\n", coreIndex);
    computer->cpu->cores[coreIndex]->state = OCCUPIED;
    computer->cpu->cores[coreIndex]->switchInTimer = 0; // start timer
    addProcessEventToGraph(graph, pid, time, READY, coreIndex);
}

//Here or in simulation.c
int getNextSchedulingEventTime(Computer *computer, Workload *workload, Scheduler *scheduler)
{
    int time = -1;
    return time;
}

/*int FCFSalgo(Computer *computer)
{
    if(computer == NULL){
        return EXIT_FAILURE;
    }

    Workload *workload;

    for(int i = 0; i < getProcessCount(computer->scheduler->workload); i++){
        enqueue(computer->scheduler, getPIDFromWorkload(computer->scheduler->workload, i), computer->scheduler->time);
    }

    return EXIT_SUCCESS;
*/