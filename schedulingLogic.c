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
#include "queues.h"

#define NB_WAIT_QUEUES 1

/* --------------------------- struct definitions -------------------------- */

struct Scheduler_t
{
    // This is not the ready queues, but the ready queue algorithms
    SchedulingAlgorithm **readyQueueAlgorithms;
    int readyQueueCount;
    Queue **readyQueues;
    int waitingQueueCount;
    Queue **waitingQueues;
    Workload *workload; //usefull for SJF
    Node **processesOnCPU;
    int *queuesSizesBeforeRR;
};

/* ---------------------------- static functions --------------------------- */

static void advanceWaitingTime(Scheduler *scheduler, int deltaTime);

static bool otherProcessInReadyQueue(Scheduler *scheduler, int queueNbr);

static bool higherPriorityProcessInReadyQueues(Scheduler *scheduler, int queueNbr, Node *node);

static bool higherPriorityProcessInReadyQueue(Scheduler *scheduler, int queueNbr, Node *node);

/* -------------------------- getters and setters -------------------------- */

int getWaitQueueCount(void)
{
    return NB_WAIT_QUEUES;
}

/* -------------------------- init/free functions -------------------------- */

Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount, Workload *workload, int cpuCoreCount)
{
    Scheduler *scheduler = malloc(sizeof(Scheduler));
    if (!scheduler)
    {
        return NULL;
    }

    scheduler->readyQueueAlgorithms = readyQueueAlgorithms;
    scheduler->readyQueueCount = readyQueueCount;
    scheduler->readyQueues = (Queue **) malloc(readyQueueCount * sizeof(Queue *));
    for (int i=0; i<readyQueueCount; i++)
    {
        scheduler->readyQueues[i] = initQueue();
    }
    scheduler->waitingQueueCount = getWaitQueueCount();
    scheduler->waitingQueues = (Queue **) malloc(scheduler->waitingQueueCount * sizeof(Queue *));
    for (int i=0; i<scheduler->waitingQueueCount; i++)
    {
        scheduler->waitingQueues[i] = initQueue();
    }
    scheduler->workload = workload;

    //usefull ?
    scheduler->processesOnCPU = (Node**) malloc(cpuCoreCount * sizeof(Node *));
    if (!scheduler->processesOnCPU)
    {
        for (int i = 0; i < scheduler->readyQueueCount; i++)
        {
            freeSchedulingAlgorithm(scheduler->readyQueueAlgorithms[i]);
            freeQueue(scheduler->readyQueues[i]);
        }
        for (int i = 0; i < scheduler->waitingQueueCount; i++)
        {
            freeQueue(scheduler->waitingQueues[i]);
        }
        free(scheduler->readyQueueAlgorithms);
        free(scheduler->readyQueues);
        free(scheduler->waitingQueues);
        free(scheduler);
        return NULL;
    }

    scheduler->queuesSizesBeforeRR = (int *) malloc(readyQueueCount * sizeof(int));
    if (!scheduler->queuesSizesBeforeRR)
    {
        for (int i = 0; i < scheduler->readyQueueCount; i++)
        {
            freeSchedulingAlgorithm(scheduler->readyQueueAlgorithms[i]);
            freeQueue(scheduler->readyQueues[i]);
        }
        for (int i = 0; i < scheduler->waitingQueueCount; i++)
        {
            freeQueue(scheduler->waitingQueues[i]);
        }
        free(scheduler->readyQueueAlgorithms);
        free(scheduler->readyQueues);
        free(scheduler->waitingQueues);
        free(scheduler->processesOnCPU);
        free(scheduler);
        return NULL;
    }

    return scheduler;
}

void freeScheduler(Scheduler *scheduler)
{
    for (int i = 0; i < scheduler->readyQueueCount; i++)
    {
        freeSchedulingAlgorithm(scheduler->readyQueueAlgorithms[i]);
        freeQueue(scheduler->readyQueues[i]);
    }
    for (int i = 0; i < scheduler->waitingQueueCount; i++)
    {
        freeQueue(scheduler->waitingQueues[i]);
    }
    free(scheduler->readyQueueAlgorithms);
    free(scheduler->readyQueues);
    free(scheduler->waitingQueues);
    free(scheduler->processesOnCPU);
    free(scheduler->queuesSizesBeforeRR);
    free(scheduler);
}

/* -------------------------- scheduling functions ------------------------- */

void putprocessInQueue(Scheduler *scheduler, int queueNbr, Node *node)
{
    printf("putprocessInQueue queueNbr=%d\n", queueNbr);
    printNode(node);
    switch (scheduler->readyQueueAlgorithms[queueNbr]->type)
    {
        case FCFS:
            enqueueNodeFCFS(scheduler->readyQueues[queueNbr], node);
            break;
        case SJF:
            node->executionTime = getProcessCurEventTimeLeft(scheduler->workload, node->pcb->pid);
            enqueueNodeSJF(scheduler->readyQueues[queueNbr], node);
            break;
        case RR:
            enqueueNodeFCFS(scheduler->readyQueues[queueNbr], node);
            break;
        case PRIORITY:
            enqueueNodePriority(scheduler->readyQueues[queueNbr], node);
            break;
    }
}

Node *topReadyQueue(Scheduler *scheduler)
{
    for (int i=0; i <= scheduler->readyQueueCount; i++)
    {
        if (!isEmptyQueue(scheduler->readyQueues[i]))
            return topNode(scheduler->readyQueues[i]);
    }
    return NULL;
}

Node *dequeueTopReadyQueue(Scheduler *scheduler)
{
    //"Queue 0 is the queue which has the highest priority, then the queue 1, and so on."
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        if (!isEmptyQueue(scheduler->readyQueues[i]))
            return dequeueTopNode(scheduler->readyQueues[i]);
    }
    return NULL;
}

void removeReadyQueueNode(Scheduler *scheduler, int queueNbr, Node *node)
{
    removeNode(scheduler->readyQueues[queueNbr], node);
}

bool processInReadyQueues(Scheduler *scheduler, int pid)
{
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        if (processInQueue(scheduler->readyQueues[i], pid))
            return 1;
    }
    return 0;
}

static bool otherProcessInReadyQueue(Scheduler *scheduler, int queueNbr)
{
    for (int i=0; i <= queueNbr; i++)
    {
        if (!isEmptyQueue(scheduler->readyQueues[i]))
            return true;
    }
    return false;
}

static bool higherPriorityProcessInReadyQueue(Scheduler *scheduler, int queueNbr, Node *node)
{
    //process from the same queue that has a higher priority with respect to the scheduling algorithm of this queue
    //only SJF and PRIORITY are preemptive
    Node *head;
    switch (scheduler->readyQueueAlgorithms[queueNbr]->type)
    {
        case FCFS:
        case RR:
            break;
        case SJF:
            head = topNode(scheduler->readyQueues[queueNbr]);
            if (head && head->executionTime <= node->executionTime)
                return true;
            break;
        case PRIORITY:
            head = topNode(scheduler->readyQueues[queueNbr]);
            if (head && head->pcb->priority <= node->pcb->priority)
                return true;
            break;
    }
    return false;
}

/*A higher priority process can either be a process from a higher priority queue
or a process from the same queue that has a higher priority with respect to the scheduling algorithm of this queue.*/
static bool higherPriorityProcessInReadyQueues(Scheduler *scheduler, int queueNbr, Node *node)
{
    return otherProcessInReadyQueue(scheduler, queueNbr-1) || higherPriorityProcessInReadyQueue(scheduler, queueNbr, node);
}

//debug:
void printReadyQueues(Scheduler *scheduler)
{
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        printf("Queue n°%d: ", i);
        printQueueAlgo(scheduler->readyQueueAlgorithms[i]);
        printQueue(scheduler->readyQueues[i]);
    }
}

void handleSchedulerEvents(Computer *computer, int time, AllStats *stats)
{
    //1. Handle event(s): simulator and the scheduler check if an event is triggered at the current time unit and handle it
    //cpu: switch-in/out
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        if (computer->cpu->cores[i]->processNode) //if there is a process on the core
        {
            //printf("getProcessCurEventTimeLeft(workload, computer->cpu->cores[i->processNode->pid) = %d, getProcessCurEventTimeLeft(workload, computer->cpu->cores[i]->processNode->pcb->pid) == 0 = %d\n", getProcessCurEventTimeLeft(workload, computer->cpu->cores[i]->processNode->pcb->pid), getProcessCurEventTimeLeft(workload, computer->cpu->cores[i]->processNode->pcb->pid) == 0);
            //printf("getProcessNextEventTime(workload, computer->cpu->cores[i]->processNode->pcb->pid) = %d, getProcessAdvancementTime(workload, computer->cpu->cores[i]->processNode->pcb->pid) = %d\n", getProcessNextEventTime(workload, computer->cpu->cores[i]->processNode->pcb->pid), getProcessAdvancementTime(workload, computer->cpu->cores[i]->processNode->pcb->pid));
            //printf("computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->type %s == RR = %d\n", AlgoTypeToString(computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->type), computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->type == RR);
            //printf("computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->RRSliceLimit = %d == computer->cpu->cores[i]->quantumTime=%d : %d\n", computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->RRSliceLimit, computer->cpu->cores[i]->quantumTime, computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->RRSliceLimit == computer->cpu->cores[i]->quantumTime);
            int pid = computer->cpu->cores[i]->processNode->pcb->pid;
            if (computer->cpu->cores[i]->state == SWITCH_IN && computer->cpu->cores[i]->switchInTimer == 0)
            {
                printf("pid %d switch-in finished\n", pid);
                computer->cpu->cores[i]->state = OCCUPIED;
                computer->cpu->cores[i]->processNode->pcb->state = RUNNING;
                /*If on the other hand, a process is preempted by another process at the middle of its time slice,
                when the process is put back on the CPU, it will start a new time slice from 0.*/
                computer->cpu->cores[i]->quantumTime = 0;
            }
            else if (computer->cpu->cores[i]->state == SWITCH_OUT && computer->cpu->cores[i]->switchOutTimer == 0)
            {
                printf("pid %d switch-out finished\n", pid);
                Node *processNode = computer->cpu->cores[i]->processNode;
                computer->cpu->cores[i]->state = IDLE;
                computer->cpu->cores[i]->processNode = NULL; //release the core
                if (computer->cpu->cores[i]->continueOnCPU)
                {
                    printf("continueOnCPU: Process put back in readyqueue - %d\n", processNode->queueNbr);
                    handleProcessForCPU(computer->scheduler, processNode);
                }
            }
            else if (getProcessCurEventTimeLeft(computer->scheduler->workload, computer->cpu->cores[i]->processNode->pcb->pid) == 0) //terminated
            {
                //start switch out/terminated
                printf("process %d terminated or begins switch out\n", pid);
                bool terminated = (getProcessAdvancementTime(computer->scheduler->workload, pid) == getProcessDuration(computer->scheduler->workload, pid));
                //printf("getProcessAdvancementTime(workload, pid)=%d   getProcessDuration(workload, pid)=%d    terminated=%d", getProcessAdvancementTime(workload, pid), getProcessDuration(workload, pid), getProcessAdvancementTime(workload, pid) == getProcessDuration(workload, pid));
                if (terminated)
                {
                    //terminated: process can "disappear"
                    //"When a process has finished its execution, there is no switch out time."
                    printf("process %d terminated at time=%d\n", pid, time);
                    computer->cpu->cores[i]->processNode->pcb->state = TERMINATED;
                    computer->cpu->cores[i]->state = IDLE; //release the core
                    //"The process is simply removed from the core."
                    freeNode(computer->cpu->cores[i]->processNode);
                    computer->cpu->cores[i]->processNode = NULL; //release the core

                    //compute the stats for terminated here to avoid repeating it every time step
                    getProcessStats(stats, pid)->finishTime = time;
                    getProcessStats(stats, pid)->turnaroundTime = time - getProcessStats(stats, pid)->arrivalTime; //finish-arrival
                    getProcessStats(stats, pid)->meanResponseTime = (double)getProcessStats(stats, pid)->waitingTime/(getProcessStats(stats, pid)->nbContextSwitches+1);
                }
                else {
                    //start switch out
                    Node *processNode = computer->cpu->cores[i]->processNode;
                    if (SWITCH_OUT_DURATION > 0)
                    {
                        printf("process %d starts switch-out\n", pid);
                        computer->cpu->cores[i]->state = SWITCH_OUT;
                        computer->cpu->cores[i]->switchOutTimer = SWITCH_OUT_DURATION; // start timer
                    } else {
                        printf("process %d no switch-out\n", pid);
                        computer->cpu->cores[i]->state = IDLE;
                        computer->cpu->cores[i]->processNode = NULL; //release the core
                    }
                    //"An I/O operation can start without having to wait for the process that initiated it to be switched out. Therefore, you must put the process on the wait queue directly (at the same time as the context switch starts)"
                    //The list of events will alternate between CPU and I/O events => next event is a IO_BURST
                    processNode->pcb->state = WAITING;
                    printf("Next event in an IO: need to add to waiting queue\n");
                    computer->cpu->cores[i]->continueOnCPU = false;
                    handleProcessForDisk(computer->scheduler, processNode);
                    getProcessStats(stats, pid)->nbContextSwitches += 1;
                }
            }
            

            ///---------------------------------
            if (computer->cpu->cores[i]->state == OCCUPIED)
            {
                //For a process to move to the next queue, it must have been executing in the current queue for a certain amount of time (the --limit argument).
                int queueNbr = computer->cpu->cores[i]->processNode->queueNbr;
                //printf("queueNbr=%d    computer->cpu->cores[i]->processNode->currentQueueExecutionTime=%d\n", queueNbr, computer->cpu->cores[i]->processNode->currentQueueExecutionTime);
                //printf("computer->scheduler->readyQueueAlgorithms[queueNbr]->executiontTimeLimit=%d\n", computer->scheduler->readyQueueAlgorithms[queueNbr]->executiontTimeLimit);
                if (computer->scheduler->readyQueueAlgorithms[queueNbr]->executiontTimeLimit != NO_LIMIT)
                {
                    //printf("need to check for --limit?: computer->cpu->cores[i]->processNode->currentQueueExecutionTime %d >= computer->scheduler->readyQueueAlgorithms[queueNbr]->executiontTimeLimit %d =%d\n", computer->cpu->cores[i]->processNode->currentQueueExecutionTime, computer->scheduler->readyQueueAlgorithms[queueNbr]->executiontTimeLimit, computer->cpu->cores[i]->processNode->currentQueueExecutionTime >= computer->scheduler->readyQueueAlgorithms[queueNbr]->executiontTimeLimit);
                    //>= ? je crois que = suffit: a tester
                    if (computer->cpu->cores[i]->processNode->currentQueueExecutionTime >= computer->scheduler->readyQueueAlgorithms[queueNbr]->executiontTimeLimit)
                    {
                        printf("----------------moving process %d to next queue due to --limit: executiontTimeLimit%d == computer->cpu->cores[i]->processNode->currentQueueExecutionTime %d\n", computer->cpu->cores[i]->processNode->pcb->pid, computer->scheduler->readyQueueAlgorithms[queueNbr]->executiontTimeLimit, computer->cpu->cores[i]->processNode->currentQueueExecutionTime);
                        //change queue
                        //what to do when it is not the end of the slice
                        computer->cpu->cores[i]->processNode->currentQueueExecutionTime = 0;
                        computer->cpu->cores[i]->processNode->currentQueueWaitingTime = 0;
                        //"However, whether the context switch should be omitted or not when a process moves from one ready queue to another is not specified in the statement. Therefore, you can choose to omit the context switch in this situation or not"
                        computer->cpu->cores[i]->processNode->queueNbr = queueNbr+1;
                        //need to make a fct to avoid copy paste
                        ///------------------------------------------------- here put back code
                        /*if (otherProcessInReadyQueue(computer->scheduler, computer->cpu->cores[i]->processNode->queueNbr))
                        {
                            printf("process %d begins switch out due to end of time slice\n", pid);
                            //start switch out
                            Node *processNode = computer->cpu->cores[i]->processNode;
                            if (SWITCH_OUT_DURATION > 0)
                            {
                                printf("process %d starts switch-out\n", pid);
                                computer->cpu->cores[i]->state = SWITCH_OUT;
                                computer->cpu->cores[i]->switchOutTimer = SWITCH_OUT_DURATION; // start timer
                                computer->cpu->cores[i]->continueOnCPU = true; //flag to indicate that once the switch-out finished it must go back on the CPU
                            } else {
                                printf("process %d no switch-out\n", pid);
                                computer->cpu->cores[i]->state = IDLE;
                                computer->cpu->cores[i]->processNode = NULL; //release the core
                                printf("Process put back in readyqueue -- %d\n", processNode->queueNbr);
                                handleProcessForCPU(computer->scheduler, processNode);
                            }
                            processNode->pcb->state = READY;
                        }*/
                    }
                }

                //multilevel :
                printf("--------------------need to do this for multi core--------------\n");
                /*Supposing there is only one core, a running process should be preempted whenever a higher priority process is ready to run,
                or when the running process has used up its time slice (in the context of the Round-Robin algorithm).
                A higher priority process can either be a process from a higher priority queue
                or a process from the same queue that has a higher priority with respect to the scheduling algorithm of this queue.*/
                if (computer->cpu->cores[i]->processNode && higherPriorityProcessInReadyQueues(computer->scheduler, computer->cpu->cores[i]->processNode->queueNbr, computer->cpu->cores[i]->processNode))
                {
                    printf("process %d begins switch out due to higher priority process\n", pid);
                    //start switch out
                    Node *processNode = computer->cpu->cores[i]->processNode;
                    if (SWITCH_OUT_DURATION > 0)
                    {
                        printf("process %d starts switch-out\n", pid);
                        computer->cpu->cores[i]->state = SWITCH_OUT;
                        computer->cpu->cores[i]->switchOutTimer = SWITCH_OUT_DURATION; // start timer
                        computer->cpu->cores[i]->continueOnCPU = true; //flag to indicate that once the switch-out finished it must go back on the CPU
                    } else {
                        printf("process %d no switch-out\n", pid);
                        computer->cpu->cores[i]->state = IDLE;
                        computer->cpu->cores[i]->processNode = NULL; //release the core
                        printf("Process put back in readyqueue ---- %d\n", processNode->queueNbr);
                        handleProcessForCPU(computer->scheduler, processNode);
                    }
                    processNode->pcb->state = READY;
                }
                /*else if (computer->cpu->cores[i]->processNode && computer->scheduler->readyQueueAlgorithms[queueNbr]->type == RR
                && computer->scheduler->readyQueueAlgorithms[queueNbr]->RRSliceLimit == computer->cpu->cores[i]->quantumTime)
                {
                    printf("----------RR------------\n");
                    if (otherProcessInReadyQueue(computer->scheduler, computer->cpu->cores[i]->processNode->queueNbr))
                    {
                        printf("process %d begins switch out due to end of time slice\n", pid);
                        //start switch out
                        Node *processNode = computer->cpu->cores[i]->processNode;
                        if (SWITCH_OUT_DURATION > 0)
                        {
                            printf("process %d starts switch-out\n", pid);
                            computer->cpu->cores[i]->state = SWITCH_OUT;
                            computer->cpu->cores[i]->switchOutTimer = SWITCH_OUT_DURATION; // start timer
                            computer->cpu->cores[i]->continueOnCPU = true; //flag to indicate that once the switch-out finished it must go back on the CPU
                        } else {
                            printf("process %d no switch-out\n", pid);
                            computer->cpu->cores[i]->state = IDLE;
                            computer->cpu->cores[i]->processNode = NULL; //release the core
                            printf("Process put back in readyqueue\n");
                            handleProcessForCPU(computer->scheduler, processNode);
                        }
                        processNode->pcb->state = READY;
                    }
                    else
                    {
                        //Concerning the RR algorithm, if a process has finished its time slice but no other process is ready
                        //to be executed, the process will start a new time slice without passing through the ready queue.
                        computer->cpu->cores[i]->quantumTime = 0;
                    }
                }*/
            }
        }
        /*if (computer->cpu->cores[i]->state == OCCUPIED && computer->cpu->cores[i]->processNode
        && computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->type == RR
                && computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->RRSliceLimit == computer->cpu->cores[i]->quantumTime) //if there is a process on the core
        {
            int pid = computer->cpu->cores[i]->processNode->pcb->pid;
            //int queueNbr = computer->cpu->cores[i]->processNode->queueNbr;
            printf("----------RR------------\n");
            if (otherProcessInReadyQueue(computer->scheduler, computer->cpu->cores[i]->processNode->queueNbr))
            {
                printf("process %d begins switch out due to end of time slice\n", pid);
                //start switch out
                Node *processNode = computer->cpu->cores[i]->processNode;
                if (SWITCH_OUT_DURATION > 0)
                {
                    printf("process %d starts switch-out\n", pid);
                    computer->cpu->cores[i]->state = SWITCH_OUT;
                    computer->cpu->cores[i]->switchOutTimer = SWITCH_OUT_DURATION; // start timer
                    computer->cpu->cores[i]->continueOnCPU = true; //flag to indicate that once the switch-out finished it must go back on the CPU
                } else {
                    printf("process %d no switch-out\n", pid);
                    computer->cpu->cores[i]->state = IDLE;
                    computer->cpu->cores[i]->processNode = NULL; //release the core
                    printf("Process put back in readyqueue\n");
                    handleProcessForCPU(computer->scheduler, processNode);
                }
                processNode->pcb->state = READY;
            }
            else
            {
                //Concerning the RR algorithm, if a process has finished its time slice but no other process is ready
                //to be executed, the process will start a new time slice without passing through the ready queue.
                computer->cpu->cores[i]->quantumTime = 0;
            }
        }*/
        if (computer->cpu->cores[i]->state == INTERRUPTED && computer->cpu->cores[i]->interruptTimer == 0)
        {
            printf("interrupt finished on core %d: needs to come back to previous state - interrupt for pid %d\n", i, computer->disk->processNode->pcb->pid);
            //the process that was waiting for the IO operation to complete will be put back on the ready queue.
            computer->disk->processNode->pcb->state = READY;
            advanceNextEvent(computer->scheduler->workload, computer->disk->processNode->pcb->pid);
            handleProcessForCPU(computer->scheduler, computer->disk->processNode);

            computer->disk->processNode = NULL;
            // need to come back to previous state in the CPU core
            if (computer->cpu->cores[i]->processNode)
            { //there was a process on the core
                int pid = computer->cpu->cores[i]->processNode->pcb->pid;
                if (computer->cpu->cores[i]->switchInTimer != 0)
                {
                    printf("pid %d switch-in continuing\n", pid);
                    computer->cpu->cores[i]->state = SWITCH_IN;
                }
                else if (computer->cpu->cores[i]->switchOutTimer != 0)
                {
                    printf("pid %d switch-out continuing\n", pid);
                    computer->cpu->cores[i]->state = SWITCH_OUT;
                }
                else
                {
                    printf("pid %d execution continuing\n", pid);
                    computer->cpu->cores[i]->state = OCCUPIED;
                    computer->cpu->cores[i]->processNode->pcb->state = RUNNING; //"restart" the process execution
                }
            }
            else {
                computer->cpu->cores[i]->state = IDLE;
            }
        }
    }
    //cpu: need to deal with RR Slices
    //could optimize this a bit
    //array to store the size of the queues before dealing with RR
    //this allows to know how many switches will be necessary
    for (int i=0; i < computer->scheduler->readyQueueCount; i++)
    {
        computer->scheduler->queuesSizesBeforeRR[i] = queueSize(computer->scheduler->readyQueues[i]);
    }
    //for (int i = computer->cpu->coreCount-1; i >= 0; i--)
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        if (computer->cpu->cores[i]->state == OCCUPIED && computer->cpu->cores[i]->processNode
        && computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->type == RR
                && computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->RRSliceLimit == computer->cpu->cores[i]->quantumTime) //if there is a process on the core
        {
            int pid = computer->cpu->cores[i]->processNode->pcb->pid;
            //int queueNbr = computer->cpu->cores[i]->processNode->queueNbr;
            printf("----------RR------------\n");
            printf("computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr: %d]->RRSliceLimit: %d == computer->cpu->cores[i]->quantumTime:%d -> %d\n", computer->cpu->cores[i]->processNode->queueNbr, computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->RRSliceLimit, computer->cpu->cores[i]->quantumTime, computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->RRSliceLimit == computer->cpu->cores[i]->quantumTime);
            if (computer->scheduler->queuesSizesBeforeRR[computer->cpu->cores[i]->processNode->queueNbr])
            {
                printf("process %d begins switch out due to end of time slice\n", pid);
                //start switch out
                Node *processNode = computer->cpu->cores[i]->processNode;
                if (SWITCH_OUT_DURATION > 0)
                {
                    printf("process %d starts switch-out\n", pid);
                    computer->cpu->cores[i]->state = SWITCH_OUT;
                    computer->cpu->cores[i]->switchOutTimer = SWITCH_OUT_DURATION; // start timer
                    computer->cpu->cores[i]->continueOnCPU = true; //flag to indicate that once the switch-out finished it must go back on the CPU
                } else {
                    printf("process %d no switch-out\n", pid);
                    computer->cpu->cores[i]->state = IDLE;
                    computer->cpu->cores[i]->processNode = NULL; //release the core
                    printf("Process put back in readyqueue %d\n", processNode->queueNbr);
                    handleProcessForCPU(computer->scheduler, processNode);
                }
                processNode->pcb->state = READY;
                computer->scheduler->queuesSizesBeforeRR[computer->cpu->cores[i]->processNode->queueNbr]--;
            }
            else
            {
                /*Concerning the RR algorithm, if a process has finished its time slice but no other process is ready
                to be executed, the process will start a new time slice without passing through the ready queue.*/
                computer->cpu->cores[i]->quantumTime = 0;
            }
        }
    }
    
    //Ex: event = scheduling events, such as a process needing to move to an upper queue because of aging
    for (int i=0; i < computer->scheduler->readyQueueCount; i++)
    {
        //For a process to move to the next queue, it must have been executing in the current queue for a certain amount of time (the --limit argument).
        /*if (computer->scheduler->readyQueueAlgorithms[i]->executiontTimeLimit != NO_LIMIT)
        {
            printf("need to check for --limit\n");
            Node *node = computer->scheduler->readyQueues[i]->head;
            while (node)
            {
                //>= ? je crois que = suffit: a tester
                if (node->currentQueueExecutionTime >= computer->scheduler->readyQueueAlgorithms[i]->executiontTimeLimit)
                {
                    printf("----------------moving process %d to next queue due to --limit: executiontTimeLimit%d == node->currentQueueExecutionTime %d\n", node->pcb->pid, computer->scheduler->readyQueueAlgorithms[i]->executiontTimeLimit, node->currentQueueExecutionTime);
                    removeReadyQueueNode(computer->scheduler, i, node);
                    node->currentQueueExecutionTime = 0;
                    node->currentQueueWaitingTime = 0;
                    putprocessInQueue(computer->scheduler, i+1, node);
                }
                node = node->next;
            }
        }*/
        //To avoid starvation, a process that has been waiting for a certain amount of time in the current queue will be moved to the previous queue (the --age argument).
        if (computer->scheduler->readyQueueAlgorithms[i]->ageLimit != NO_LIMIT)
        {
            printf("need to check for --age\n");//currentQueueWaitingTime
            Node *node = computer->scheduler->readyQueues[i]->head;
            while (node)
            {
                //>= ? je crois que = suffit: a tester
                if (node->currentQueueWaitingTime >= computer->scheduler->readyQueueAlgorithms[i]->ageLimit)
                {
                    printf("------------------moving process %d to next queue due to --age: ageTimeLimit%d == node->currentQueueWaitingTime %d\n", node->pcb->pid, computer->scheduler->readyQueueAlgorithms[i]->ageLimit, node->currentQueueWaitingTime);
                    removeReadyQueueNode(computer->scheduler, i, node);
                    node->currentQueueExecutionTime = 0;
                    node->currentQueueWaitingTime = 0;
                    putprocessInQueue(computer->scheduler, i-1, node);
                }
                node = node->next;
            }
        }
    }
    //gerer la preemptiveness
}

void handleProcessForCPU(Scheduler *scheduler, Node *node)
{
    printf("handleProcessForCPU\n");
    printNode(node);
    //"In our simulator, every process starts on queue 0"
    putprocessInQueue(scheduler, node->queueNbr, node);
    printQueue(scheduler->readyQueues[0]);
}

void handleProcessForDisk(Scheduler *scheduler, Node *node)
{
    advanceNextEvent(scheduler->workload, node->pcb->pid);
    enqueueNodeFCFS(scheduler->waitingQueues[0], node);
}

void assignProcessesToResources(Computer *computer)
{
    printf("\nassignProcessToResources\n");
    bool interrupt = false;
    //The scheduler will check if a process is ready to be executed and will choose what core it should put it on (or not).
    //here no notion of fairness between cores, always the first cores that get assigned first: could improve that
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        if (computer->cpu->cores[i]->state == IDLE)
        {
            Node *node = dequeueTopReadyQueue(computer->scheduler);
            if (node)
            {
                printf("selected process for CPU: %d\n", node->pcb->pid);
                putProcessOnCPU(computer, i, node);
            } else {
                printf("no more process in ready queue\n");
                //break; //if empty readyQueue: no need to look for any more idle cores
            }
        }
        if (computer->cpu->cores[i]->state == INTERRUPTED)
        {
            interrupt = true;
        }
    }
    //The scheduler could also put a process on the disk if it is idle.
    //additional bug maybe try multi disk ? could ask the assistant as it seams "easy" to do

    //"Once the interrupt handler completes, the scheduler is informed of the I/O operation's completion, allowing it to schedule the next I/O operation, if applicable"
    //The scheduler could also put a process on the disk if it is idle. (+ no interrupt happening)
    if (!interrupt && computer->disk->state == DISK_IDLE)
    {
        Node *node = dequeueTopNode(computer->scheduler->waitingQueues[0]);
        if (node)
        {
            printf("selected process for IO: %d\n", node->pcb->pid);
            putProcessOnDisk(computer, node);
        } else {
            printf("no more process in waiting queue\n");
            //break; //if empty readyQueue: no need to look for any more idle cores
        }
    }
    printDiskState(computer->disk);
    printQueue(computer->scheduler->waitingQueues[0]);
}

//schedulingLogic should not use the workload, but here thee is no other choice, we need to update the next event
void putProcessOnCPU(Computer *computer, int coreIndex, Node *node)
{
    printf("putProcessOnCPU with index: %d\n", coreIndex);
    //"Whenever a process starts executing on a core, there is a switch in time"
    if (SWITCH_IN_DURATION > 0)
    {
        computer->cpu->cores[coreIndex]->state = SWITCH_IN;
        computer->cpu->cores[coreIndex]->switchInTimer = SWITCH_IN_DURATION; // start timer
        node->pcb->state = READY; //is it really necessary ?
    } else
    {
        computer->cpu->cores[coreIndex]->state = OCCUPIED;
        node->pcb->state = RUNNING;
        /*If on the other hand, a process is preempted by another process at the middle of its time slice,
        when the process is put back on the CPU, it will start a new time slice from 0.*/
        computer->cpu->cores[coreIndex]->quantumTime = 0;
    }
    computer->cpu->cores[coreIndex]->processNode = node;
    computer->scheduler->processesOnCPU[coreIndex] = node;
    
    //advanceNextEvent(workload, node->pcb->pid);
}

void putProcessOnDisk(Computer *computer, Node *node)
{
    printf("putProcessOnDisk %d\n", node->pcb->pid);
    computer->disk->state = DISK_RUNNING;
    node->pcb->state = WAITING;
    computer->disk->processNode = node;
    
    //advanceNextEvent(workload, node->pcb->pid);

    printf("end putProcessOndisk: ");
    printDiskState(computer->disk);
}

void advanceSchedulingTime(int time, int nextTime, Computer *computer)
{
    int deltaTime = nextTime - time;
    //update the cpu switch in/out timers...
    advanceWaitingTime(computer->scheduler, deltaTime);
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        //---------------------------------
        if (computer->cpu->cores[i]->state == OCCUPIED)
        {
            computer->cpu->cores[i]->processNode->currentQueueExecutionTime += deltaTime;
            computer->cpu->cores[i]->quantumTime += deltaTime;
        }
        else if (computer->cpu->cores[i]->state == SWITCH_IN)
        {
            int pid = computer->cpu->cores[i]->processNode->pcb->pid;
            printf("advance process %d in switch in\n", pid);
            computer->cpu->cores[i]->switchInTimer -= deltaTime;
        }
        else if (computer->cpu->cores[i]->state == SWITCH_OUT)
        {
            int pid = computer->cpu->cores[i]->processNode->pcb->pid;
            printf("advance process %d switch out\n", pid);
            computer->cpu->cores[i]->switchOutTimer -= deltaTime;
        }
        else if (computer->cpu->cores[i]->state == INTERRUPTED)
        {
            computer->cpu->cores[i]->interruptTimer -= deltaTime;
            printf("advance interrupt: timeleft=%d\n", computer->cpu->cores[i]->interruptTimer);
        }
    }
    //update in the disk
}

static void advanceWaitingTime(Scheduler *scheduler, int deltaTime)
{
    for (int i=0; i<scheduler->readyQueueCount; i++)
    {
        Node *node = scheduler->readyQueues[i]->head;
        while (node)
        {
            node->currentQueueWaitingTime+=deltaTime;
            node = node->next;
        }
    }
}