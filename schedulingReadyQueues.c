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

    SchedulingReadyQueue->processesIndex = (int *) malloc(size * sizeof(int));
    if (!SchedulingReadyQueue->processesIndex)
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

bool enqueueSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue, int processIndex)
{
    if (isFullSchedulingReadyQueue(SchedulingReadyQueue))
    {
        // SchedulingReadyQueue is full, cannot enSchedulingReadyQueue more elements
        return 1;
    }
    else
    {
        // SchedulingReadyQueue is not full, insert the element at the tail
        SchedulingReadyQueue->processesIndex[SchedulingReadyQueue->tail] = processIndex;
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

    int processIndex = SchedulingReadyQueue->processesIndex[SchedulingReadyQueue->head];
    // Update front considering wrapping around
    SchedulingReadyQueue->head = (SchedulingReadyQueue->head + 1) % SchedulingReadyQueue->size;

    return processIndex;
}

int topSchedulingReadyQueueFCFS(SchedulingReadyQueue *SchedulingReadyQueue)
{
    if (isEmptySchedulingReadyQueue(SchedulingReadyQueue))
    {
        return -1;
    }

    int processIndex = SchedulingReadyQueue->processesIndex[SchedulingReadyQueue->head];

    return processIndex;
}