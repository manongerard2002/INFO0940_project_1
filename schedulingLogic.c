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

#define NB_WAIT_QUEUES 1

/* --------------------------- struct definitions -------------------------- */

struct Scheduler_t
{
    // This is not the ready queues, but the ready queue algorithms
    SchedulingAlgorithm **readyQueueAlgorithms;
    int readyQueueCount;
};

struct Queue_t
{
    PCB **processes;
    int head;
    int tail;
    int size;
};

Queue *initQueue(int size)
{
    Queue *queue = malloc(sizeof(Queue));
    if (!queue)
    {
        return NULL;
    }

    queue->processes = (PCB **)malloc(size * sizeof(PCB *));
    if (!queue->processes)
    {
        free(queue);
        return NULL;
    }

    queue->head = 0;
    queue->tail = 0;
    queue->size = size;

    return queue;
}

bool isEmpty(Queue *queue)
{
    return queue->head == -1;
}

void enqueue(Queue *queue, PCB *process)
{
    if (queue->tail == queue->size)
    {
        // Queue is full, cannot enqueue more elements
        return;
    }

    if (isEmpty(queue))
    {
        // Queue is empty, insert the element at the head
        queue->processes[0] = process;
        queue->head = 0;
        queue->tail = 1;
    }
    else
    {
        // Queue is not empty, insert the element at the tail(end of Queue)
        queue->processes[queue->tail] = process;
        queue->tail++;
    }
}

PCB *dequeue(Queue *queue)
{
    if (isEmpty(queue))
    {
        return NULL;
    }

    PCB *process = queue->processes[queue->head];
    // Update front considering wrapping around
    queue->head = (queue->head + 1) % queue->size;

    // If the queue becomes empty after dequeue
    if (isEmpty(queue))
    {
        queue->head = -1;
        queue->tail = -1;
    }

    return process;
}

void FCFS(Scheduler *scheduler, Workload *workload, int timeQuantum)
{
    Queue *readyQueue = initQueue(getProcessCount(workload));
    if (!readyQueue)
    {
        fprintf(stderr, "Failed to initialize queue.\n");
        return;
    }
    for (int i = 0; i < getProcessCount(workload); i++)
    {
        int pid = getPIDFromWorkload(workload, i);
        int duration = getProcessDuration(workload, pid);
        enqueue(readyQueue, getProcessInfo(workload, i));
    }

    int time = 0;
    while (!isEmpty(readyQueue))
    {
        PCB *process = dequeue(readyQueue);
        int ProcessDuration = getProcessDuration(workload, process->pid);
        //int ProcessDuration = processesInfo->processes[time].duration; //can we do a getter to do this?

        for (int i = 0; i < ProcessDuration; i++)
        {
            runProcess(process, 1);
            time++;
        }
        free(process);
    }
    // free(readyQueue->processes); might cause double free
    free(readyQueue);
}


int compareProcesses(Workload* w, int pida, int pidb) {
    
    if ( getProcessStartTime(w, pida) != getProcessStartTime(w, pidb)) {
        return getProcessStartTime(w, pida)  - getProcessStartTime(w, pidb);
    } else {
        return getProcessDuration(w, pida) - getProcessDuration(w, pidb);
    }
}


void SJF(Scheduler *scheduler, Workload *workload, int timeQuantum)
{
    int processCount = getProcessCount(workload);
    int *processes = malloc(processCount * sizeof(int));
    if (!processes)
    {
        fprintf(stderr, "Failed to allocate memory for processes array.\n");
        return;
    }

    for (int i = 0; i < processCount; i++)
    {
        processes[i] = i;
    }

    qsort(processes, processCount, sizeof(int), compareProcesses);

    int time = 0;
    for (int i = 0; i < processCount; i++)
    {
        int pid = processes[i];
        int duration = getProcessDuration(workload, pid);
        for (int j = 0; j < duration; j++)
        {
            runProcess(getProcessInfo(workload, pid), 1);
            time++;
        }
    }

    free(processes);
}





/* ---------------------------- static functions --------------------------- */

/* -------------------------- getters and setters -------------------------- */

int getWaitQueueCount(void)
{
    return NB_WAIT_QUEUES;
}

/* -------------------------- init/free functions -------------------------- */

Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount)
{
    Scheduler *scheduler = malloc(sizeof(Scheduler));
    if (!scheduler)
    {
        return NULL;
    }

    scheduler->readyQueueAlgorithms = readyQueueAlgorithms;
    scheduler->readyQueueCount = readyQueueCount;

    return scheduler;
}

void freeScheduler(Scheduler *scheduler)
{
    for (int i = 0; i < scheduler->readyQueueCount; i++)
    {
        free(scheduler->readyQueueAlgorithms[i]);
    }
    free(scheduler->readyQueueAlgorithms);
    free(scheduler);
}

/* -------------------------- scheduling functions ------------------------- */
