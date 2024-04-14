#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "queues.h"
#include "simulation.h" //debug

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
    node->pcb = pcb;
    node->executionTime = -1; //default value
    node->waitingTime = 0;
    node->prev = NULL;
    node->next = NULL;

    return node;
}

Queue *initQueue()
{
    Queue *queue = (Queue *) malloc(sizeof(Queue));
    if (!queue)
    {
        return NULL;
    }
    queue->head = NULL;
    queue->tail = NULL;

    return queue;
}

void freeNode(Node *node)
{
    free(node);
}

void freeQueue(Queue *queue)
{
    Node *node, *node_next;
    for (node = queue->head; node; node = node_next)
    {
        node_next = node->next;
        freeNode(node);
    }
    free(queue);
}

bool isEmptyQueue(Queue *queue)
{
    return !queue->head;
}

void enqueueNodeFCFS(Queue *queue, Node *node)
{
    node->next = NULL;

    if (!queue->head) {
        node->prev = NULL;
        queue->head = node;
    }
    else
    {
        node->prev = queue->tail;
        queue->tail->next = node;
    }

    queue->tail = node;
}

void enqueueNodePriority(Queue *queue, Node *node)
{
    if (!queue->head) {
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
}

void enqueueNodeSJF(Queue *queue, Node *node)
{
    if (!queue->head) {
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
}

Node *dequeueNode(Queue *queue)
{
    Node *firstNode = queue->head;
    if (firstNode)
    {
        queue->head = firstNode->next;
    }
    if (!queue->head)
    {
        queue->tail = NULL;
    }

    return firstNode;
}

Node *topNode(Queue *queue)
{
    return queue->head;
}

bool processInQueue(Queue *queue, int pid) {
    Node *node = queue->head;
    while (node) {
        if (node->pcb->pid == pid)
            return true;
        node = node->next;
    }
    return false;
}

void printNode(Node *node)
{
    printf("pid %d - state ", node->pcb->pid);
    //ProcessStateToString(node->pcb->state); //works ?
    printf("priority %d ", node->pcb->priority);
    printf("executionTime: %d ", node->executionTime);
    printf("waitingTime: %d\n", node->waitingTime);
}

void printQueue(Queue *queue)
{
    printf("printReadyQueue: \n");
    Node *node = queue->head;
    while (node) {
        printNode(node);
        node = node->next;
    }
}


/*int main() {
    Queue *queue = initQueue();
    PCB *pcb1 = (PCB *) malloc(sizeof(PCB));
    pcb1->pid = 1;
    pcb1->priority = 1;
    Node *node1 = initNode(pcb1);
    node1->executionTime=10;
    PCB *pcb2 = (PCB *) malloc(sizeof(PCB));
    pcb2->pid = 2;
    pcb2->priority = 3;
    Node *node2 = initNode(pcb2);
    node2->executionTime=15;
    PCB *pcb3 = (PCB *) malloc(sizeof(PCB));
    pcb3->pid = 3;
    pcb3->priority = 2;
    Node *node3 = initNode(pcb3);
    node3->executionTime=5;
    PCB *pcb4 = (PCB *) malloc(sizeof(PCB));
    pcb4->pid = 4;
    pcb4->priority = 15;
    Node *node4 = initNode(pcb4);
    node4->executionTime=2;
    PCB *pcb5 = (PCB *) malloc(sizeof(PCB));
    pcb5->pid = 5;
    pcb5->priority = 222;
    Node *node5 = initNode(pcb5);
    node5->executionTime=7;

    printf("Enqueuing processes with FCFS:\n");
    enqueueNodeFCFS(queue, node1);
    enqueueNodeFCFS(queue, node2);
    enqueueNodeFCFS(queue, node3);
    enqueueNodeFCFS(queue, node4);
    enqueueNodeFCFS(queue, node5);
    printQueue(queue);
    printf("Dequeue queue: ");
    Node *node7 = dequeueNode(queue);
    printNode(node7);
    
    printQueue(queue);
    printf("Top queue: ");
    Node *node8 = topNode(queue);
    printNode(node8);
    printQueue(queue);
    printf("\n");

    printf("Dequeue queue\n");
    Node *node9 = dequeueNode(queue);
    printf("Enqueue queue: ");
    enqueueNodeFCFS(queue, node9);
    PCB *pcb6 = (PCB *) malloc(sizeof(PCB));
    pcb6->pid = 6;
    pcb6->priority = 0;
    Node *node6 = initNode(pcb6);
    node6->executionTime=50;
    enqueueNodeFCFS(queue, node6);
    printQueue(queue);
    printf("Dequeue all queue: ");
    dequeueNode(queue);
    dequeueNode(queue);
    dequeueNode(queue);
    dequeueNode(queue);
    dequeueNode(queue);
    printQueue(queue);
    printf("\n");

    
    PCB *pcb10 = (PCB *) malloc(sizeof(PCB));
    pcb10->pid = 7;
    pcb10->priority = 0;
    Node *node10 = initNode(pcb10);
    node10->executionTime=8;
    enqueueNodeFCFS(queue, node10);
    printQueue(queue);
    printf("\n");
    dequeueNode(queue);
    printQueue(queue);
    printf("\n");


    printf("-------------PRIORITY-------------------\n");
    printf("Enqueuing processes with Priority:\n");
    enqueueNodePriority(queue, node1);
    printQueue(queue);
    enqueueNodePriority(queue, node2);
    printQueue(queue);
    enqueueNodePriority(queue, node3);
    printQueue(queue);
    enqueueNodePriority(queue, node4);
    printQueue(queue);
    enqueueNodePriority(queue, node5);
    printQueue(queue);
    printf("Dequeue queue: ");
    node7 = dequeueNode(queue);
    printNode(node7);
    
    printQueue(queue);
    printf("Top queue: ");
    node8 = topNode(queue);
    printNode(node8);
    printQueue(queue);
    printf("\n");

    printf("Dequeue queue\n");
    node9 = dequeueNode(queue);
    printf("Enqueue queue: ");
    enqueueNodePriority(queue, node9);
    enqueueNodePriority(queue, node6);
    printQueue(queue);
    printf("Dequeue all queue: ");
    dequeueNode(queue);
    dequeueNode(queue);
    dequeueNode(queue);
    dequeueNode(queue);
    dequeueNode(queue);
    printQueue(queue);
    printf("\n");

    enqueueNodePriority(queue, node10);
    printQueue(queue);
    printf("\n");
    dequeueNode(queue);
    printQueue(queue);
    printf("\n");

    printf("-------------SJF-------------------\n");
    printf("Enqueuing processes with SJF:\n");
    enqueueNodeSJF(queue, node1);
    printQueue(queue);
    enqueueNodeSJF(queue, node2);
    printQueue(queue);
    enqueueNodeSJF(queue, node3);
    printQueue(queue);
    enqueueNodeSJF(queue, node4);
    printQueue(queue);
    enqueueNodeSJF(queue, node5);
    printQueue(queue);
    printf("Dequeue queue: ");
    node7 = dequeueNode(queue);
    printNode(node7);
    
    printQueue(queue);
    printf("Top queue: ");
    node8 = topNode(queue);
    printNode(node8);
    printQueue(queue);
    printf("\n");

    printf("Dequeue queue\n");
    node9 = dequeueNode(queue);
    printf("Enqueue queue: ");
    enqueueNodeSJF(queue, node9);
    enqueueNodeSJF(queue, node6);
    printQueue(queue);
    printf("Dequeue all queue: ");
    dequeueNode(queue);
    dequeueNode(queue);
    dequeueNode(queue);
    dequeueNode(queue);
    dequeueNode(queue);
    printQueue(queue);
    printf("\n");

    enqueueNodePriority(queue, node10);
    printQueue(queue);
    printf("\n");
    dequeueNode(queue);
    printQueue(queue);
    printf("\n");

    freeNode(node1);
    freeNode(node2);
    freeNode(node3);
    freeNode(node4);
    freeNode(node5);
    freeNode(node6);
    freeNode(node10);
    free(pcb1);
    free(pcb2);
    free(pcb3);
    free(pcb4);
    free(pcb5);
    free(pcb6);
    free(pcb10);
    freeQueue(queue);

    return 0;
}*/