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

    return scheduler;
}

void freeScheduler(Scheduler *scheduler)
{
    for (int i = 0; i < scheduler->readyQueueCount; i++)
    {
        free(scheduler->readyQueueAlgorithms[i]);
        free(scheduler->readyQueues[i]);
    }
    free(scheduler->readyQueueAlgorithms);
    free(scheduler->readyQueues);
    free(scheduler);
}

/* -------------------------- scheduling functions ------------------------- */

void putProcessInReadyQueue(Scheduler *scheduler, int queueNbr, int pid) {
    if (scheduler->readyQueueAlgorithms[queueNbr]->type == FCFS)
    {
        enqueueSchedulingReadyQueueFCFS(scheduler->readyQueues[queueNbr], pid);
    }
}

int topReadyQueue(Scheduler *scheduler) {
    for (int i=0; i <= scheduler->readyQueueCount; i++)
    {
        if (!isEmptySchedulingReadyQueue(scheduler->readyQueues[i]))
            if (scheduler->readyQueueAlgorithms[i]->type == FCFS)
                return topSchedulingReadyQueueFCFS(scheduler->readyQueues[i]);
    }
    return -1;
}

int dequeueReadyQueue(Scheduler *scheduler) {
    for (int i=0; i < scheduler->readyQueueCount; i++)
    {
        if (!isEmptySchedulingReadyQueue(scheduler->readyQueues[i])) {
            if (scheduler->readyQueueAlgorithms[i]->type == FCFS)
                return dequeueSchedulingReadyQueueFCFS(scheduler->readyQueues[i]); }
    }
    return -1;
}

int allProcessesInReadyQueues(Scheduler *scheduler, int* allProcesses) {
    int indexStart = 0;
    for (int i = 0; i < scheduler->readyQueueCount; i++)
    {
        indexStart = allProcessesInReadyQueue(scheduler->readyQueues[i], allProcesses, indexStart);
    }
    return indexStart;
}

//Here or in simulation.c
void handleEvents(Computer *computer, Workload *workload, int time, ProcessGraph *graph, AllStats *stats)
{
    //1. Handle event(s): simulator and the scheduler check if an event is triggered at the current time unit and handle it
    //Ex: if a process arrives in the system, the simulator will call the scheduler to put the process in the ready queue.
    printf("beginning of handleEvents\n");
    processArrived(computer->scheduler, workload, time, graph, stats);

    //Ex: event = scheduling events, such as a process needing to move to an upper queue because of aging

    for (int j = 0; j < getProcessCount(workload); j++)
    {
        int pid = getPIDFromWorkload(workload, j);
        if (getProcessStartTime(workload, pid) < time)
        {
            int remainingEventTime = getProcessCurEventTimeLeft(workload, pid);
            if (remainingEventTime <= 0) //or = only
            {
                printf("need to deal with finish op for pid:%d\n", pid);
                
            }
        }
    }
    //cpu: switch-in/out
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        printCPUStates(computer->cpu);
        printf("------------getProcessCurEventTimeLeft(workload, computer->cpu->cores[i]->PID) = %d, getProcessCurEventTimeLeft(workload, computer->cpu->cores[i]->PID) == 0 = %d\n", getProcessCurEventTimeLeft(workload, computer->cpu->cores[i]->PID), getProcessCurEventTimeLeft(workload, computer->cpu->cores[i]->PID) == 0);
        printf("------------getProcessNextEventTime(workload, computer->cpu->cores[i]->PID) = %d, getProcessAdvancementTime(workload, computer->cpu->cores[i]->PID) = %d\n", getProcessNextEventTime(workload, computer->cpu->cores[i]->PID), getProcessAdvancementTime(workload, computer->cpu->cores[i]->PID));
        int pid = computer->cpu->cores[i]->PID;
        if (computer->cpu->cores[i]->switchInTimer == getSwitchInDuration())
        {
            printf("switch-in finished\n");
            computer->cpu->cores[i]->state = OCCUPIED;
            computer->cpu->cores[i]->switchInTimer = -1;
            addProcessEventToGraph(graph, pid, time, RUNNING, i);
        }
        else if (computer->cpu->cores[i]->switchOutTimer == getSwitchOutDuration()) // => state == switch_out
        {
            printf("switch-out finished\n");
            computer->cpu->cores[i]->state = IDLE;
            computer->cpu->cores[i]->switchOutTimer = -1;
            //put it were it should go: and there update graph
        }
        else if (computer->cpu->cores[i]->state == OCCUPIED && getProcessCurEventTimeLeft(workload, computer->cpu->cores[i]->PID) == 0) //terminated
        {
            printf("process might be terminated/begins switch out: %d   \n", time);
            int pid = computer->cpu->cores[i]->PID;
            bool terminated = (getProcessAdvancementTime(workload, pid) + 1 == getProcessDuration(workload, pid));
            if (terminated)
            {
                printf("process %d terminated: do nothing here\n", pid);
                //process can "disappear"
            }
            else {
                //start switch out/terminated
                printf("process %d starts switch-out\n", pid);
                computer->cpu->cores[i]->state = SWITCH_OUT;
                computer->cpu->cores[i]->switchOutTimer = 0; // start timer
                addProcessEventToGraph(graph, pid, time, READY, i);
                //getProcessStats(stats, pid)->cpuTime=getProcessStats(stats, pid)->cpuTime+1; //no: switch out does not count
                //need to understand
                //getProcessStats(stats, pid)->nbContextSwitches=getProcessStats(stats, pid)->nbContextSwitches+1;
            }
        }
    }

    //Ex: event = hardware events, such as the triggering of an interrupt.
}

void assignProcessesToResources(Computer *computer, Workload *workload, int time, ProcessGraph *graph, AllStats *stats, int *cpuCoresIDLE)
{
    printf("assignProcessToResources\n");
    //The scheduler will check if a process is ready to be executed and will choose what core it should put it on (or not).
    //can't rememeber how to do better in c:
    int nb = 0;
    for (int i = 0; i < computer->cpu->coreCount; i++)
    {
        cpuCoresIDLE[i] = -1; //reset: could be skipped
        if (computer->cpu->cores[i]->state == IDLE)
        {
            cpuCoresIDLE[nb] = i;
            nb++;
        }
        //start switch out
        if (computer->cpu->cores[i]->state == OCCUPIED && getProcessCurEventTimeLeft(workload, computer->cpu->cores[i]->PID) == 0)
        {
            int pid = computer->cpu->cores[i]->PID;
            bool terminated = getProcessNextEventTime(workload, pid) == getProcessDuration(workload, pid);
            if (!terminated)
            {
                printf("switch-out start\n");
                computer->cpu->cores[i]->state = SWITCH_OUT;
                computer->cpu->cores[i]->switchOutTimer = 0; // start timer
                //put it were it should go: and there update graph
            }
        }
    }
    //printf("number of core idle: %d\n", nb);
    for (int i = 0; i < nb; i++)
    {
        int pid = dequeueReadyQueue(computer->scheduler);
        if (pid != -1)
        {
            //start switch-in
            printf("selected process: %d\n", pid);
            putProcessOnCPU(computer, cpuCoresIDLE[i], graph, pid, time, stats);
        }
    }

    
        

    //The scheduler could also put a process on the disk if it is idle.
    if (computer->disk->state == DISK_IDLE)
    {
        printf("disk isIdle\n");
    }
}

void putProcessOnCPU(Computer *computer, int coreIndex, ProcessGraph *graph, int pid, int time, AllStats *stats)
{
    printf("putProcessOnCPU with index: %d\n", coreIndex);
    computer->cpu->cores[coreIndex]->state = SWITCH_IN;
    computer->cpu->cores[coreIndex]->switchInTimer = 0; // start timer
    computer->cpu->cores[coreIndex]->PID = pid;
    addProcessEventToGraph(graph, pid, time, READY, coreIndex);
    //getProcessStats(stats, pid)->cpuTime=getProcessStats(stats, pid)->cpuTime+1; //no: switch in does not count
    getProcessStats(stats, pid)->nbContextSwitches=getProcessStats(stats, pid)->nbContextSwitches+1;
}

//Here or in simulation.c
int getNextSchedulingEventTime(Computer *computer, Workload *workload, Scheduler *scheduler)
{
    int time = -1;
    ///deal with multi-thread
    return time;
}

void advanceTimeProcessInQueue(int time, int next_time, Computer *computer, ProcessGraph *graph, AllStats *stats, int* allProcesses) {
    int delta_time = next_time - time;
    printf("\nupdateGraphAndStats: time=%d    next_time=%d     delta_time=%d\n",time, next_time, delta_time);
    //update in the readyqueue
    int maxIndex = allProcessesInReadyQueues(computer->scheduler, allProcesses);
    for (int i = 0; i < maxIndex; i++)
    {
        int pid = allProcesses[i];
        printf("Process in readyqueue: %d\n", pid);
        for(int j = time; j < next_time; j++)
        {
            addProcessEventToGraph(graph, pid, j, READY, i);
        }
        printf("%f", (double) getProcessStats(stats, pid)->waitingTime/(getProcessStats(stats, pid)->nbContextSwitches+1));
        getProcessStats(stats, pid)->waitingTime=getProcessStats(stats, pid)->waitingTime+delta_time;
    }
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