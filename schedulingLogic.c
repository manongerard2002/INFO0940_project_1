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
};

/* ---------------------------- static functions --------------------------- */

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
    free(scheduler);
}

/* -------------------------- scheduling functions ------------------------- */

void putprocessInQueue(Scheduler *scheduler, int queueNbr, Node *node) {
    if (scheduler->readyQueueAlgorithms[queueNbr]->type == FCFS)
    {
        enqueueNodeFCFS(scheduler->readyQueues[queueNbr], node);
    }
}

Node *topReadyQueue(Scheduler *scheduler) {
    for (int i=0; i <= scheduler->readyQueueCount; i++)
    {
        if (!isEmptyQueue(scheduler->readyQueues[i]))
            return topNode(scheduler->readyQueues[i]);
    }
    return NULL;
}

Node *dequeueReadyQueue(Scheduler *scheduler) {
    //"Queue 0 is the queue which has the highest priority, then the queue 1, and so on."
    printf("dequeue: printreadyqueue:");
    printReadyQueues(scheduler);
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        if (!isEmptyQueue(scheduler->readyQueues[i])) {
            return dequeueNode(scheduler->readyQueues[i]); }
    }
    printf("WTF: dequeue failed\n");
    return NULL;
}

bool processInReadyQueues(Scheduler *scheduler, int pid) {
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        printf("processInReadyQueues\n");
        if (processInQueue(scheduler->readyQueues[i], pid))
            return 1;
    }
    return 0;
}

//debug:
void printReadyQueues(Scheduler *scheduler) {
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        printQueueAlgo(scheduler->readyQueueAlgorithms[i]);
        printQueue(scheduler->readyQueues[i]);
    }
}

void handleSchedulerEvents(Computer *computer, int time, AllStats *stats) {
    //1. Handle event(s): simulator and the scheduler check if an event is triggered at the current time unit and handle it
    printf("handleSchedulerEvents: need to do it----\n");
    
    //Ex: event = scheduling events, such as a process needing to move to an upper queue because of aging
    /*for (int i=0; i < computer->scheduler->readyQueueCount; i++)
    {
        printf("processInReadyQueues\n");
        //For a process to move to the next queue, it must have been executing in the current queue for a certain amount of time (the --limit argument).
        if (computer->scheduler->readyQueueAlgorithms[i]->executiontTimeLimit != NO_LIMIT)
        {
            printf("need to check for --limit");
        }
        //To avoid starvation, a process that has been waiting for a certain amount of time in the current queue will be moved to the previous queue (the --age argument).
        if (computer->scheduler->readyQueueAlgorithms[i]->ageLimit != NO_LIMIT)
        {
            printf("need to check for --age");
        }
        //deal with RR slice
        if (computer->scheduler->readyQueueAlgorithms[i]->type = RR)
        {
            //computer->scheduler->readyQueueAlgorithms[i]->RRSliceLimit
            printf("need to check for --RR");
        }
    }*/
    //gerer la preemptiveness
}

void handleProcessForCPU(Scheduler *scheduler, Node *node)
{
    printf("handleProcessForCPU\n");
    //"In our simulator, every process starts on queue 0"
    putprocessInQueue(scheduler, 0, node);
    printQueue(scheduler->readyQueues[0]);
}

void handleProcessForDisk(Scheduler *scheduler, Node *node)
{
    enqueueNodeFCFS(scheduler->waitingQueues[0], node);
}

void assignProcessesToResources(Computer *computer, Workload *workload)
{
    printf("\nassignProcessToResources\n");
    bool interrupt = false;
    //The scheduler will check if a process is ready to be executed and will choose what core it should put it on (or not).
    //here no notion of fairness between cores, always the first cores that get assigned first: could improve that
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        if (computer->cpu->cores[i]->state == IDLE)
        {
            Node *node = dequeueReadyQueue(computer->scheduler);
            if (node)
            {
                printf("selected process for CPU: %d\n", node->pcb->pid);
                putProcessOnCPU(workload, computer, i, node);
            } else {
                printf("no more process in ready queue\n");
                break; //if empty readyQueue: no need to look for any more idle cores
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
    if (!interrupt && computer->disk->state == DISK_IDLE_)
    {
        Node *node = dequeueNode(computer->scheduler->waitingQueues[0]);
        if (node)
        {
            printf("selected process for IO: %d\n", node->pcb->pid);
            putProcessOnDisk(workload, computer, node);
        } else {
            printf("no more process in waiting queue\n");
            //break; //if empty readyQueue: no need to look for any more idle cores
        }
    }
    printDiskStates(computer->disk);
    printQueue(computer->scheduler->waitingQueues[0]);
}

//schedulingLogic should not use the workload, but here thee is no other choice, we need to update the next event
void putProcessOnCPU(Workload *workload, Computer *computer, int coreIndex, Node *node)
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
    }
    computer->cpu->cores[coreIndex]->processNode = node;
    
    advanceNextEvent(workload, node->pcb->pid);

    printf("end putProcessOnCPU: ");
    printCPUStates(computer->cpu);
}

void putProcessOnDisk(Workload *workload, Computer *computer, Node *node)
{
    printf("putProcessOnDisk %d\n", node->pcb->pid);
    computer->disk->state = DISK_RUNNING_;
    node->pcb->state = WAITING;
    computer->disk->processNode = node;
    
    advanceNextEvent(workload, node->pcb->pid);

    printf("end putProcessOndisk: ");
    printDiskStates(computer->disk);
}

void advanceSchedulingTime(int time, int next_time, Computer *computer) {
    int delta_time = next_time - time;
    //update the cpu switch in/out timers
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        /*if (computer->cpu->cores[i]->state == OCCUPIED)
        {
            int pid = computer->cpu->cores[i->processNode->pid;
            getProcessStats(stats, pid)->waitingTime += delta_time;
            printf("\nselected process: %d in occuped cpu\n", pid);

        }
        else*/ if (computer->cpu->cores[i]->state == SWITCH_IN)
        {
            int pid = computer->cpu->cores[i]->processNode->pcb->pid;
            printf("advance process %d in switch in\n", pid);
            computer->cpu->cores[i]->switchInTimer -= delta_time;
        }
        else if (computer->cpu->cores[i]->state == SWITCH_OUT)
        {
            int pid = computer->cpu->cores[i]->processNode->pcb->pid;
            printf("advance process %d switch out\n", pid);
            computer->cpu->cores[i]->switchOutTimer -= delta_time;
        }
        else if (computer->cpu->cores[i]->state == INTERRUPTED)
        {
            computer->cpu->cores[i]->interruptTimer -= delta_time;
            printf("advance interrupt: timeleft=%d\n", computer->cpu->cores[i]->interruptTimer);
        }
    }
    //update in the disk
}
