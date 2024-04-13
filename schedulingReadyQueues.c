#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedulingReadyQueues.h"

SchedulingReadyQueue *initSchedulingReadyQueue(int size)
{
    SchedulingReadyQueue *readyQueue = (SchedulingReadyQueue *) malloc(sizeof(SchedulingReadyQueue));
    if (!readyQueue)
    {
        return NULL;
    }

    readyQueue->head = 0;
    readyQueue->tail = 0;
    readyQueue->size = size + 1; //need an extra slot to differentiate empty and full

    readyQueue->processesPCB = (PCB **) malloc(readyQueue->size * sizeof(PCB *)); //to change to a structure
    if (!readyQueue->processesPCB)
    {
        free(readyQueue);
        return NULL;
    }

    return readyQueue;
}

void freeSchedulingReadyQueue(SchedulingReadyQueue *readyQueue)
{
    free(readyQueue->processesPCB);
    free(readyQueue);
}

bool isEmptySchedulingReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue)
{
    return SchedulingReadyQueue->head == SchedulingReadyQueue->tail;
}

bool isFullSchedulingReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue)
{
    return (SchedulingReadyQueue->tail + 1) % SchedulingReadyQueue->size == SchedulingReadyQueue->head;
}

bool enqueueSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue, PCB *pcb)
{
    if (isFullSchedulingReadyQueue(SchedulingReadyQueue))
    {
        // SchedulingReadyQueue is full, cannot enSchedulingReadyQueue more elements
        return 1;
    }
    else
    {
        // SchedulingReadyQueue is not full, insert the element at the tail
        printf("SchedulingReadyQueue->tail=%d\n", SchedulingReadyQueue->tail);
        SchedulingReadyQueue->processesPCB[SchedulingReadyQueue->tail] = pcb;
        printf("ok2\n");
        SchedulingReadyQueue->tail = (SchedulingReadyQueue->tail + 1) % SchedulingReadyQueue->size;
    }
    return 0;
}

PCB *dequeueSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue)
{
    if (isEmptySchedulingReadyQueue(SchedulingReadyQueue))
    {
        return NULL;
    }

    PCB *pcb = SchedulingReadyQueue->processesPCB[SchedulingReadyQueue->head];
    // Update front considering wrapping around
    SchedulingReadyQueue->head = (SchedulingReadyQueue->head + 1) % SchedulingReadyQueue->size;

    return pcb;
}

PCB *topSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue)
{
    if (isEmptySchedulingReadyQueue(SchedulingReadyQueue))
    {
        return NULL;
    }

    PCB *pcb = SchedulingReadyQueue->processesPCB[SchedulingReadyQueue->head];

    return pcb;
}

bool processInReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue, int pid) {
    int i = SchedulingReadyQueue->head;
    while (i != SchedulingReadyQueue->tail) {
        if (SchedulingReadyQueue->processesPCB[i]->pid == pid)
            return 1;
        i = (i + 1) % SchedulingReadyQueue->size;
    }
    return 0;
}

/*int allProcessesInReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue, int *ProcessesInReadyQueues, int indexStart) {
    int i = SchedulingReadyQueue->head;
    while (i != SchedulingReadyQueue->tail) {
        ProcessesInReadyQueues[indexStart] = SchedulingReadyQueue->processesPCB[i];
        indexStart++;
        i = (i + 1) % SchedulingReadyQueue->size;
    }
    return indexStart;
}*/

void printReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue)
{
    printf("printReadyQueue: size =%d    head=%d   tail=%d\n", SchedulingReadyQueue->size, SchedulingReadyQueue->head, SchedulingReadyQueue->tail);
    int i = SchedulingReadyQueue->head;
    while (i != SchedulingReadyQueue->tail) {
        printf("pid %d - state ", SchedulingReadyQueue->processesPCB[i]->pid);
        ProcessStateToString(SchedulingReadyQueue->processesPCB[i]->state); //works ?
        printf("priority %d", SchedulingReadyQueue->processesPCB[i]->priority);
        i = (i + 1) % SchedulingReadyQueue->size;
        printf("\n");
    }
}