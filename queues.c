#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "queues.h"

Node *initNode(PCB *pcb)
{
    if (!pcb)
    {
        return NULL;
    }
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node)
    {
        return NULL;
    }
    node->queueNbr=0;
    node->pcb = pcb;
    node->executionTime = -1; //default value: only used for SJF
    node->currentQueueWaitingTime = 0;
    node->currentQueueExecutionTime = 0;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

Queue *initQueue(void)
{
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    if (!queue)
    {
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0; //usefull for RR

    return queue;
}

void freeNode(Node *node)
{
    free(node);
}

void freeQueue(Queue *queue)
{
    Node *node, *nextNode;
    for (node = queue->head; node; node = nextNode)
    {
        nextNode = node->next;
        freeNode(node);
    }
    free(queue);
}

bool isEmptyQueue(Queue *queue)
{
    return queue->size == 0;//!queue->head;
}

int queueSize(Queue *queue)
{
    return queue->size;
}

void enqueueNodeFCFS(Queue *queue, Node *node)
{
    node->next = NULL;

    if (!queue->head)
    {
        node->prev = NULL;
        queue->head = node;
    }
    else
    {
        node->prev = queue->tail;
        queue->tail->next = node;
    }

    queue->tail = node;
    queue->size += 1;
}

void enqueueNodePriority(Queue *queue, Node *node)
{
    if (!queue->head)
    {
        node->next = NULL;
        node->prev = NULL;
        queue->head = node;
        queue->tail = node;
    }
    else
    {
        if (node->pcb->priority < queue->head->pcb->priority)
        {
            node->prev = NULL;
            queue->head->prev = node;
            node->next = queue->head;
            queue->head = node;
        }
        else if (node->pcb->priority >= queue->tail->pcb->priority)
        {
            node->next = NULL;
            queue->tail->next = node;
            node->prev = queue->tail;
            queue->tail = node;
        }
        else {
            Node *tmp = queue->head->next;
            while (node->pcb->priority >= tmp->pcb->priority)
                tmp = tmp->next;
            (tmp->prev)->next = node;
            node->prev = tmp->prev;
            node->next = tmp;
            tmp->prev = node;
        }
    }
    queue->size += 1;
}

void enqueueNodeSJF(Queue *queue, Node *node)
{
    if (!queue->head)
    {
        node->next = NULL;
        node->prev = NULL;
        queue->head = node;
        queue->tail = node;
    }
    else
    {
        if (node->executionTime < queue->head->executionTime)
        {
            node->prev = NULL;
            queue->head->prev = node;
            node->next = queue->head;
            queue->head = node;
        }
        else if (node->executionTime >= queue->tail->executionTime)
        {
            node->next = NULL;
            queue->tail->next = node;
            node->prev = queue->tail;
            queue->tail = node;
        }
        else {
            Node *tmp = queue->head->next;
            while (node->executionTime >= tmp->executionTime)
                tmp = tmp->next;
            (tmp->prev)->next = node;
            node->prev = tmp->prev;
            node->next = tmp;
            tmp->prev = node;
        }
    }
    queue->size += 1;
}

Node *dequeueTopNode(Queue *queue)
{
    Node *firstNode = queue->head;
    if (firstNode)
    {
        queue->head = firstNode->next;
        queue->size -= 1;
    }
    if (!queue->head)
    {
        queue->tail = NULL;
    }
    else
    {
        queue->head->prev = NULL;
    }

    return firstNode;
}

void removeNode(Queue *queue, Node *node)
{
    Node *tmpNode = queue->head;
    if (queue->head == node)
    {
        queue->head = queue->head->next;
        queue->head->prev = NULL;
        queue->size -= 1;
        return;
    }
    else if (queue->tail == node)
    {
        queue->tail = queue->tail->prev;
        queue->tail->next = NULL;
        queue->size -= 1;
        return;
    }
    while (tmpNode)
    {
        if (tmpNode == node)
        {
            tmpNode->prev->next = tmpNode->next;
            tmpNode->next->prev = tmpNode;
            queue->size -= 1;
            break;
        }
        tmpNode = tmpNode->next;
    }
}

Node *topNode(Queue *queue)
{
    return queue->head;
}

bool processInQueue(Queue *queue, int pid)
{
    Node *node = queue->head;
    while (node)
    {
        if (node->pcb->pid == pid)
            return true;
        node = node->next;
    }
    return false;
}

//debug
void printNode(Node *node)
{
    printf("pid %d - state ", node->pcb->pid);
    //ProcessStateToString(node->pcb->state); //works ?
    printf("priority %d ", node->pcb->priority);
    printf("executionTime: %d ", node->executionTime);
    printf("currentQueueWaitingTime: %d ", node->currentQueueWaitingTime);
    printf("currentQueueExecutionTime: %d\n", node->currentQueueExecutionTime);
}

void printQueue(Queue *queue)
{
    printf("printReadyQueue: \n");
    Node *node = queue->head;
    while (node)
    {
        printNode(node);
        node = node->next;
    }
}
