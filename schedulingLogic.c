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
    int waitingQueueCount;
    SchedulingReadyQueue **waitingQueues; //should generalize the name so that it works for both
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
    scheduler->waitingQueueCount = getWaitQueueCount();
    scheduler->waitingQueues = (SchedulingReadyQueue **) malloc(scheduler->waitingQueueCount * sizeof(SchedulingReadyQueue *));
    for (int i=0; i<scheduler->waitingQueueCount; i++)
    {
        scheduler->waitingQueues[i] = initSchedulingReadyQueue(nbProcesses);
    }

    return scheduler;
}

void freeScheduler(Scheduler *scheduler)
{
    for (int i = 0; i < scheduler->readyQueueCount; i++)
    {
        freeSchedulingAlgorithm(scheduler->readyQueueAlgorithms[i]);
        freeSchedulingReadyQueue(scheduler->readyQueues[i]);
    }
    for (int i = 0; i < scheduler->waitingQueueCount; i++)
    {
        freeSchedulingReadyQueue(scheduler->waitingQueues[i]);
    }
    free(scheduler->readyQueueAlgorithms);
    free(scheduler->readyQueues);
    free(scheduler->waitingQueues);
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

bool processInReadyQueues(Scheduler *scheduler, int pid) {
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        printf("processInReadyQueues\n");
        if (processInReadyQueue(scheduler->readyQueues[i], pid))
            return 1;
    }
    return 0;
}

/*int allProcessesInReadyQueues(Scheduler *scheduler, int* ProcessesInReadyQueues) {
    int indexStart = 0;
    for (int i = 0; i < scheduler->readyQueueCount; i++)
    {
        indexStart = allProcessesInReadyQueue(scheduler->readyQueues[i], ProcessesInReadyQueues, indexStart);
    }
    return indexStart;
}*/

//debug:
void printReadyQueues(Scheduler *scheduler) {
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        printReadyQueue(scheduler->readyQueues[i]);
    }
}

void handleProcessForCPU(Scheduler *scheduler, PCB *pcb)
{
    printf("handleProcessForCPU\n");
    putProcessInReadyQueue(scheduler, 0, pcb);
    printReadyQueue(scheduler->readyQueues[0]);
}

void handleProcessForDisk(Scheduler *scheduler, PCB *pcb)
{
    enqueueSchedulingReadyQueueFCFS(scheduler->waitingQueues[0], pcb);
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
            PCB *pcb = dequeueReadyQueue(computer->scheduler);
            if (pcb)
            {
                printf("selected process for CPU: %d\n", pcb->pid);
                putProcessOnCPU(workload, computer, i, pcb);
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
        PCB *pcb = dequeueSchedulingReadyQueueFCFS(computer->scheduler->waitingQueues[0]);
        if (pcb)
        {
            printf("selected process for IO: %d\n", pcb->pid);
            putProcessOnDisk(workload, computer, pcb);
        } else {
            printf("no more process in waiting queue\n");
            //break; //if empty readyQueue: no need to look for any more idle cores
        }
    }
    printDiskStates(computer->disk);
    printReadyQueue(computer->scheduler->waitingQueues[0]);
}

//schedulingLogic should not use the workload, but here thee is no other choice, we need to update the next event
void putProcessOnCPU(Workload *workload, Computer *computer, int coreIndex, PCB *pcb)
{
    printf("putProcessOnCPU with index: %d\n", coreIndex);
    if (SWITCH_IN_DURATION > 0)
    {
        computer->cpu->cores[coreIndex]->state = SWITCH_IN;
        computer->cpu->cores[coreIndex]->switchInTimer = SWITCH_IN_DURATION; // start timer
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

void putProcessOnDisk(Workload *workload, Computer *computer, PCB *pcb)
{
    printf("putProcessOnDisk %d\n", pcb->pid);
    computer->disk->state = DISK_RUNNING_;
    pcb->state = WAITING;
    computer->disk->pcb = pcb;
    
    advanceNextEvent(workload, pcb->pid);

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
            int pid = computer->cpu->cores[i]->pcb->pid;
            getProcessStats(stats, pid)->waitingTime += delta_time;
            printf("\nselected process: %d in occuped cpu\n", pid);

        }
        else*/ if (computer->cpu->cores[i]->state == SWITCH_IN)
        {
            int pid = computer->cpu->cores[i]->pcb->pid;
            printf("advance process %d in switch in\n", pid);
            computer->cpu->cores[i]->switchInTimer -= delta_time;
        }
        else if (computer->cpu->cores[i]->state == SWITCH_OUT)
        {
            int pid = computer->cpu->cores[i]->pcb->pid;
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