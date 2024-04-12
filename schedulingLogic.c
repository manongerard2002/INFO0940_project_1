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

void putProcessInReadyQueue(Scheduler *scheduler, int queueNbr, PCB *pcb) {
    if (scheduler->readyQueueAlgorithms[queueNbr]->type == FCFS)
    {
        enqueueSchedulingReadyQueueFCFS(scheduler->readyQueues[queueNbr], pcb);
    }
}

PCB *topReadyQueue(Scheduler *scheduler) {
    for (int i=0; i <= scheduler->readyQueueCount; i++)
    {
        if (!isEmptySchedulingReadyQueue(scheduler->readyQueues[i]))
            if (scheduler->readyQueueAlgorithms[i]->type == FCFS)
                return topSchedulingReadyQueueFCFS(scheduler->readyQueues[i]);
    }
    return NULL;
}

PCB *dequeueReadyQueue(Scheduler *scheduler) {
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        if (!isEmptySchedulingReadyQueue(scheduler->readyQueues[i])) {
            if (scheduler->readyQueueAlgorithms[i]->type == FCFS)
                return dequeueSchedulingReadyQueueFCFS(scheduler->readyQueues[i]); }
    }
    return NULL;
}

/*int allProcessesInReadyQueues(Scheduler *scheduler, int* ProcessesInReadyQueues) {
    int indexStart = 0;
    for (int i = 0; i < scheduler->readyQueueCount; i++)
    {
        indexStart = allProcessesInReadyQueue(scheduler->readyQueues[i], ProcessesInReadyQueues, indexStart);
    }
    return indexStart;
}*/

void handleProcessForCPUArrived(Scheduler *scheduler, PCB *pcb)
{
    putProcessInReadyQueue(scheduler, 0, pcb);
}

void assignProcessesToResources(Computer *computer, Workload *workload, int time, ProcessGraph *graph, AllStats *stats)
{
    printf("assignProcessToResources\n");
    //The scheduler will check if a process is ready to be executed and will choose what core it should put it on (or not).
    //here no notion of fairness between cores, always the first cores that get assigned first: could improve that
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        if (computer->cpu->cores[i]->state == IDLE)
        {
            PCB *pcb = dequeueReadyQueue(computer->scheduler);
            if (pcb)
            {
                printf("selected process: %d\n", pcb->pid);
                putProcessOnCPU(workload, computer, i, graph, pcb, time);
            } else {
                printf("no more process in ready queue\n");
                break; //if empty readyQueue: no need to look for any more idle cores
            }
        }
    }
    //The scheduler could also put a process on the disk if it is idle.
    if (computer->disk->state == DISK_IDLE)
    {
        printf("disk idle: still need to deal with it\n");
    }
}

//schedulingLogic should not use the workload, but here thee is no other choice, we need to update the next event
void putProcessOnCPU(Workload *workload, Computer *computer, int coreIndex, ProcessGraph *graph, PCB *pcb, int time)
{
    printf("putProcessOnCPU with index: %d\n", coreIndex);
    if (getSwitchInDuration() > 0)
    {
        computer->cpu->cores[coreIndex]->state = SWITCH_IN;
        computer->cpu->cores[coreIndex]->switchInTimer = 0; // start timer
        pcb->state = READY; //is it really necessary ?
    } else
    {
        computer->cpu->cores[coreIndex]->state = OCCUPIED;
        pcb->state = RUNNING;
    }
    computer->cpu->cores[coreIndex]->pcb = pcb;
    
    advanceNextEvent(workload, pcb->pid);

    printf("end putProcessOnCPU: ");
    printCPUStates(computer->cpu);
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