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
    int *queuesSizesBeforeRR; //created heee to avoid many malloc and free
};

/* ---------------------------- static functions --------------------------- */

/**
 * Update the queue waiting timer
 *
 * @param deltaTime The number of step to advance.
 * @param scheduler the scheduler
 */
static void advanceWaitingTime(Scheduler *scheduler, int deltaTime);

/**
 * Checks whether there is another process in a higher priority ready queue.
 * 
 * @return True if there is another process
*/
static bool otherProcessInReadyQueue(Scheduler *scheduler, int queueNbr);

/**
 * Checks whether there is a process from the same queue that has a higher priority 
 * with respect to the scheduling algorithm of this queue
 * 
 * @return True if there is a process with a higher priority.
*/
static bool higherPriorityProcessInReadyQueue(Scheduler *scheduler, int queueNbr, Node *node);

/**
 * Checks whether there is a process from a higher priority queue or 
 * a process from the same queue that has a higher priority 
 * with respect to the scheduling algorithm of this queue.
 * 
 * @return True if there is another process
*/
static bool higherPriorityProcessInReadyQueues(Scheduler *scheduler, int queueNbr, Node *node);

/* -------------------------- getters and setters -------------------------- */

int getWaitQueueCount(void)
{
    return NB_WAIT_QUEUES;
}

/* -------------------------- init/free functions -------------------------- */

Scheduler *initScheduler(SchedulingAlgorithm **readyQueueAlgorithms, int readyQueueCount, Workload *workload)
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
    free(scheduler->queuesSizesBeforeRR);
    free(scheduler);
}

/* -------------------------- scheduling functions ------------------------- */

void putprocessInQueue(Scheduler *scheduler, int queueNbr, Node *node)
{
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

static bool higherPriorityProcessInReadyQueues(Scheduler *scheduler, int queueNbr, Node *node)
{
    return otherProcessInReadyQueue(scheduler, queueNbr-1) || higherPriorityProcessInReadyQueue(scheduler, queueNbr, node);
}

void handleSchedulerEvents(Computer *computer, int time, AllStats *stats)
{
    //1. Handle event(s): simulator and the scheduler check if an event is triggered at the current time unit and handle it
    //Ex: event = scheduling events, such as a process needing to move to an upper queue because of aging
    for (int i=0; i < computer->scheduler->readyQueueCount; i++)
    {
        //To avoid starvation, a process that has been waiting for a certain amount of time in the current queue will be moved to the previous queue (the --age argument).
        if (computer->scheduler->readyQueueAlgorithms[i]->ageLimit != NO_LIMIT)
        {
            Node *node = computer->scheduler->readyQueues[i]->head;
            while (node)
            {
                if (node->currentQueueWaitingTime == computer->scheduler->readyQueueAlgorithms[i]->ageLimit)
                {
                    //change queue
                    removeReadyQueueNode(computer->scheduler, node->queueNbr, node);
                    node->currentQueueExecutionTime = 0;
                    node->currentQueueWaitingTime = 0;
                    //"However, whether the context switch should be omitted or not when a process moves from one ready queue to another is not specified in the statement. Therefore, you can choose to omit the context switch in this situation or not"
                    node->queueNbr -= 1;
                    handleProcessForCPU(computer->scheduler, node);
                }
                node = node->next;
            }
        }
    }
    //cpu: switch-in/out
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        if (computer->cpu->cores[i]->processNode) //if there is a process on the core
        {
            int pid = computer->cpu->cores[i]->processNode->pcb->pid;
            if (computer->cpu->cores[i]->state == SWITCH_IN && computer->cpu->cores[i]->switchInTimer == 0)
            {
                computer->cpu->cores[i]->state = OCCUPIED;
                computer->cpu->cores[i]->processNode->pcb->state = RUNNING;
                /*If on the other hand, a process is preempted by another process at the middle of its time slice,
                when the process is put back on the CPU, it will start a new time slice from 0.*/
                computer->cpu->cores[i]->quantumTime = 0;
            }
            else if (computer->cpu->cores[i]->state == SWITCH_OUT && computer->cpu->cores[i]->switchOutTimer == 0)
            {
                Node *processNode = computer->cpu->cores[i]->processNode;
                computer->cpu->cores[i]->state = IDLE;
                computer->cpu->cores[i]->processNode = NULL; //release the core
                if (computer->cpu->cores[i]->continueOnCPU)
                {
                    handleProcessForCPU(computer->scheduler, processNode);
                }
            }
            else if (getProcessCurEventTimeLeft(computer->scheduler->workload, computer->cpu->cores[i]->processNode->pcb->pid) == 0) //terminated
            {
                //start switch out/terminated
                bool terminated = (getProcessAdvancementTime(computer->scheduler->workload, pid) == getProcessDuration(computer->scheduler->workload, pid));
                if (terminated)
                {
                    //terminated: process can "disappear"
                    //"When a process has finished its execution, there is no switch out time."
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
                        computer->cpu->cores[i]->state = SWITCH_OUT;
                        computer->cpu->cores[i]->switchOutTimer = SWITCH_OUT_DURATION; // start timer
                    } else {
                        computer->cpu->cores[i]->state = IDLE;
                        computer->cpu->cores[i]->processNode = NULL; //release the core
                    }
                    //"An I/O operation can start without having to wait for the process that initiated it to be switched out. Therefore, you must put the process on the wait queue directly (at the same time as the context switch starts)"
                    //The list of events will alternate between CPU and I/O events => next event is a IO_BURST
                    processNode->pcb->state = WAITING;
                    computer->cpu->cores[i]->continueOnCPU = false;
                    handleProcessForDisk(computer->scheduler, processNode);
                    getProcessStats(stats, pid)->nbContextSwitches += 1;
                }
            }

            if (computer->cpu->cores[i]->state == OCCUPIED)
            {
                //For a process to move to the next queue, it must have been executing in the current queue for a certain amount of time (the --limit argument).
                int queueNbr = computer->cpu->cores[i]->processNode->queueNbr;
                if (computer->scheduler->readyQueueAlgorithms[queueNbr]->executiontTimeLimit != NO_LIMIT)
                {
                    if (computer->cpu->cores[i]->processNode->currentQueueExecutionTime == computer->scheduler->readyQueueAlgorithms[queueNbr]->executiontTimeLimit)
                    {
                        //change queue
                        //what to do when it is not the end of the slice
                        computer->cpu->cores[i]->processNode->currentQueueExecutionTime = 0;
                        computer->cpu->cores[i]->processNode->currentQueueWaitingTime = 0;
                        //"However, whether the context switch should be omitted or not when a process moves from one ready queue to another is not specified in the statement. Therefore, you can choose to omit the context switch in this situation or not"
                        computer->cpu->cores[i]->processNode->queueNbr += 1;
                        if (otherProcessInReadyQueue(computer->scheduler, computer->cpu->cores[i]->processNode->queueNbr))
                        {
                            //start switch out
                            Node *processNode = computer->cpu->cores[i]->processNode;
                            if (SWITCH_OUT_DURATION > 0)
                            {
                                computer->cpu->cores[i]->state = SWITCH_OUT;
                                computer->cpu->cores[i]->switchOutTimer = SWITCH_OUT_DURATION; // start timer
                                computer->cpu->cores[i]->continueOnCPU = true; //flag to indicate that once the switch-out finished it must go back on the CPU
                            } else {
                                computer->cpu->cores[i]->state = IDLE;
                                computer->cpu->cores[i]->processNode = NULL; //release the core
                                handleProcessForCPU(computer->scheduler, processNode);
                            }
                            processNode->pcb->state = READY;
                            getProcessStats(stats, processNode->pcb->pid)->nbContextSwitches += 1;
                        }
                    }
                }
            }
            if (computer->cpu->cores[i]->state == OCCUPIED)
            {
                //multilevel :
                /*Supposing there is only one core, a running process should be preempted whenever a higher priority process is ready to run,
                or when the running process has used up its time slice (in the context of the Round-Robin algorithm).
                A higher priority process can either be a process from a higher priority queue
                or a process from the same queue that has a higher priority with respect to the scheduling algorithm of this queue.*/
                if (computer->cpu->cores[i]->processNode && higherPriorityProcessInReadyQueues(computer->scheduler, computer->cpu->cores[i]->processNode->queueNbr, computer->cpu->cores[i]->processNode))
                {
                    //start switch out
                    Node *processNode = computer->cpu->cores[i]->processNode;
                    if (SWITCH_OUT_DURATION > 0)
                    {
                        computer->cpu->cores[i]->state = SWITCH_OUT;
                        computer->cpu->cores[i]->switchOutTimer = SWITCH_OUT_DURATION; // start timer
                        computer->cpu->cores[i]->continueOnCPU = true; //flag to indicate that once the switch-out finished it must go back on the CPU
                    } else
                    {
                        computer->cpu->cores[i]->state = IDLE;
                        computer->cpu->cores[i]->processNode = NULL; //release the core
                        handleProcessForCPU(computer->scheduler, processNode);
                    }
                    processNode->pcb->state = READY;
                    getProcessStats(stats, processNode->pcb->pid)->nbContextSwitches += 1;
                }
            }
        }
        if (computer->cpu->cores[i]->state == INTERRUPTED && computer->cpu->cores[i]->interruptTimer == 0)
        {
            //the process that was waiting for the IO operation to complete will be put back on the ready queue.
            computer->disk->processNode->pcb->state = READY;
            advanceNextEvent(computer->scheduler->workload, computer->disk->processNode->pcb->pid); //schedulingLogic should not use the workload, but here thee is no other choice, we need to update the next event
            handleProcessForCPU(computer->scheduler, computer->disk->processNode);

            computer->disk->processNode = NULL;
            // need to come back to previous state in the CPU core
            if (computer->cpu->cores[i]->processNode)
            { //there was a process on the core
                if (computer->cpu->cores[i]->switchInTimer != 0)
                {
                    computer->cpu->cores[i]->state = SWITCH_IN;
                }
                else if (computer->cpu->cores[i]->switchOutTimer != 0)
                {
                    computer->cpu->cores[i]->state = SWITCH_OUT;
                }
                else
                {
                    computer->cpu->cores[i]->state = OCCUPIED;
                    computer->cpu->cores[i]->processNode->pcb->state = RUNNING; //"restart" the process execution
                }
            }
            else
            {
                computer->cpu->cores[i]->state = IDLE;
            }
        }
    }
    //cpu: need to deal with RR Slices
    //array to store the size of the queues before dealing with RR
    //This is requires to know how many switches will be necessary
    //in addition we must not switch out for process wiating that have a core that is being switch out for them
    //This is to respect the graph for the question on ecampus "multilevel feedback and multicore"
    int numberSwitchOutCores = 0;
    for (int i = 0; i < computer->cpu->coreCount; i++)
        if (computer->cpu->cores[i]->state == SWITCH_OUT)
            numberSwitchOutCores++;
    for (int i=0; i < computer->scheduler->readyQueueCount; i++)
    {
        computer->scheduler->queuesSizesBeforeRR[i] = queueSize(computer->scheduler->readyQueues[i]);
        if (numberSwitchOutCores > 0)
        {
            int tmp = min(computer->scheduler->queuesSizesBeforeRR[i], numberSwitchOutCores);
            computer->scheduler->queuesSizesBeforeRR[i] -= tmp;
            numberSwitchOutCores -= tmp;
        }
    }
    for (int i = computer->cpu->coreCount-1; i >= 0; i--)
    {
        if (computer->cpu->cores[i]->state == OCCUPIED && computer->cpu->cores[i]->processNode
        && computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->type == RR
                && computer->scheduler->readyQueueAlgorithms[computer->cpu->cores[i]->processNode->queueNbr]->RRSliceLimit == computer->cpu->cores[i]->quantumTime) //if there is a process on the core
        {
            if (computer->scheduler->queuesSizesBeforeRR[computer->cpu->cores[i]->processNode->queueNbr])
            {
                //start switch out
                Node *processNode = computer->cpu->cores[i]->processNode;
                if (SWITCH_OUT_DURATION > 0)
                {
                    computer->cpu->cores[i]->state = SWITCH_OUT;
                    computer->cpu->cores[i]->switchOutTimer = SWITCH_OUT_DURATION; // start timer
                    computer->cpu->cores[i]->continueOnCPU = true; //flag to indicate that once the switch-out finished it must go back on the CPU
                } else {
                    computer->cpu->cores[i]->state = IDLE;
                    computer->cpu->cores[i]->processNode = NULL; //release the core
                    handleProcessForCPU(computer->scheduler, processNode);
                }
                processNode->pcb->state = READY;
                computer->scheduler->queuesSizesBeforeRR[processNode->queueNbr]--;
                getProcessStats(stats, processNode->pcb->pid)->nbContextSwitches += 1;
            }
            else
            {
                /*Concerning the RR algorithm, if a process has finished its time slice but no other process is ready
                to be executed, the process will start a new time slice without passing through the ready queue.*/
                computer->cpu->cores[i]->quantumTime = 0;
            }
        }
    }
}

void handleProcessForCPU(Scheduler *scheduler, Node *node)
{
    //"In our simulator, every process starts on queue 0"
    putprocessInQueue(scheduler, node->queueNbr, node);
}

void handleProcessForDisk(Scheduler *scheduler, Node *node)
{
    advanceNextEvent(scheduler->workload, node->pcb->pid); //schedulingLogic should not use the workload, but here there is no other choice, we need to update the next event
    enqueueNodeFCFS(scheduler->waitingQueues[0], node);
}

void assignProcessesToResources(Computer *computer)
{
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
                putProcessOnCPU(computer, i, node);
            }
        }
        if (computer->cpu->cores[i]->state == INTERRUPTED)
        {
            interrupt = true;
        }
    }
    //"Once the interrupt handler completes, the scheduler is informed of the I/O operation's completion, allowing it to schedule the next I/O operation, if applicable"
    //The scheduler could also put a process on the disk if it is idle. (+ no interrupt happening)
    if (!interrupt && computer->disk->state == DISK_IDLE)
    {
        Node *node = dequeueTopNode(computer->scheduler->waitingQueues[0]);
        if (node)
        {
            putProcessOnDisk(computer, node);
        }
    }
}

void putProcessOnCPU(Computer *computer, int coreIndex, Node *node)
{
    //"Whenever a process starts executing on a core, there is a switch in time"
    if (SWITCH_IN_DURATION > 0)
    {
        computer->cpu->cores[coreIndex]->state = SWITCH_IN;
        computer->cpu->cores[coreIndex]->switchInTimer = SWITCH_IN_DURATION; // start timer
        node->pcb->state = READY;
    } else
    {
        computer->cpu->cores[coreIndex]->state = OCCUPIED;
        node->pcb->state = RUNNING;
        /*If on the other hand, a process is preempted by another process at the middle of its time slice,
        when the process is put back on the CPU, it will start a new time slice from 0.*/
        computer->cpu->cores[coreIndex]->quantumTime = 0;
    }
    computer->cpu->cores[coreIndex]->processNode = node;
}

void putProcessOnDisk(Computer *computer, Node *node)
{
    computer->disk->state = DISK_RUNNING;
    node->pcb->state = WAITING;
    computer->disk->processNode = node;
}

void advanceSchedulingTime(int time, int nextTime, Computer *computer)
{
    int deltaTime = nextTime - time;
    advanceWaitingTime(computer->scheduler, deltaTime);
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        if (computer->cpu->cores[i]->state == OCCUPIED)
        {
            computer->cpu->cores[i]->processNode->currentQueueExecutionTime += deltaTime;
            computer->cpu->cores[i]->quantumTime += deltaTime;
        }
        else if (computer->cpu->cores[i]->state == SWITCH_IN)
        {
            computer->cpu->cores[i]->switchInTimer -= deltaTime;
        }
        else if (computer->cpu->cores[i]->state == SWITCH_OUT)
        {
            computer->cpu->cores[i]->switchOutTimer -= deltaTime;
        }
        else if (computer->cpu->cores[i]->state == INTERRUPTED)
        {
            computer->cpu->cores[i]->interruptTimer -= deltaTime;
        }
    }
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