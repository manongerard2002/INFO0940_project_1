#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedulingReadyQueues.h"

SchedulingReadyQueue *initSchedulingReadyQueue(int size)
{
    SchedulingReadyQueue *SchedulingReadyQueue = malloc(sizeof(SchedulingReadyQueue));
    if (!SchedulingReadyQueue)
    {
        return NULL;
    }

    SchedulingReadyQueue->processesPID = (int *) malloc(size * sizeof(int)); //to change to a structure
    if (!SchedulingReadyQueue->processesPID)
    {
        free(SchedulingReadyQueue);
        return NULL;
    }

    SchedulingReadyQueue->head = 0;
    SchedulingReadyQueue->tail = 0;
    SchedulingReadyQueue->size = size + 1; //need an extra slot to differentiate empty and full

    return SchedulingReadyQueue;
}

bool isEmptySchedulingReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue)
{
    return SchedulingReadyQueue->head == SchedulingReadyQueue->tail;
}

bool isFullSchedulingReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue)
{
    return (SchedulingReadyQueue->tail + 1) % SchedulingReadyQueue->size == SchedulingReadyQueue->head;
}

bool enqueueSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue, int pid)
{
    if (isFullSchedulingReadyQueue(SchedulingReadyQueue))
    {
        // SchedulingReadyQueue is full, cannot enSchedulingReadyQueue more elements
        return 1;
    }
    else
    {
        // SchedulingReadyQueue is not full, insert the element at the tail
        SchedulingReadyQueue->processesPID[SchedulingReadyQueue->tail] = pid;
        SchedulingReadyQueue->tail = (SchedulingReadyQueue->tail + 1) % SchedulingReadyQueue->size;
    }
    return 0;
}

int dequeueSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue)
{
    if (isEmptySchedulingReadyQueue(SchedulingReadyQueue))
    {
        return -1;
    }

    int pid = SchedulingReadyQueue->processesPID[SchedulingReadyQueue->head];
    // Update front considering wrapping around
    SchedulingReadyQueue->head = (SchedulingReadyQueue->head + 1) % SchedulingReadyQueue->size;

    return pid;
}

int topSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue)
{
    if (isEmptySchedulingReadyQueue(SchedulingReadyQueue))
    {
        return -1;
    }

    int pid = SchedulingReadyQueue->processesPID[SchedulingReadyQueue->head];

    return pid;
}

int allProcessesInReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue, int *allProcesses, int indexStart) {
    int i = SchedulingReadyQueue->head;
    while (i != SchedulingReadyQueue->tail) {
        allProcesses[indexStart] = SchedulingReadyQueue->processesPID[i];
        indexStart++;
        i = (i + 1) % SchedulingReadyQueue->size;
    }
    return indexStart;
}

void printReadyQueue(SchedulingReadyQueue *SchedulingReadyQueue)
{
    printf("printReadyQueue: ");
    int i = SchedulingReadyQueue->head;
    while (i != SchedulingReadyQueue->tail) {
        printf("%d  ", SchedulingReadyQueue->processesPID[i]);
        i = (i + 1) % SchedulingReadyQueue->size;
    }
    printf("\n");
}