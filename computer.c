// You should complete the structures of the CPU and Disk in the header file
// and modify their initialization and freeing functions here.
// You should also implement the triggering of an interrupt here (and put its
// declaration in the header file to access it from simulation.c).

#include <stdio.h>
#include <stdlib.h>

#include "computer.h"
#include "schedulingLogic.h"


#include <string.h> //for debugging: to remove after

#define INTERRUPT_TIME 1

Computer *initComputer(Scheduler *scheduler, CPU *cpu, Disk *disk)
{
    Computer *computer = (Computer *) malloc(sizeof(Computer));
    if (!computer)
    {
        return NULL;
    }
    computer->scheduler = scheduler;
    computer->cpu = cpu;
    computer->disk = disk;
    return computer;
}

void freeComputer(Computer *computer)
{
    freeScheduler(computer->scheduler);
    freeCPU(computer->cpu);
    freeDisk(computer->disk);
    free(computer);
}

CPU *initCPU(int coreCount)
{
    CPU *cpu = malloc(sizeof(CPU));
    if (!cpu)
    {
        return NULL;
    }

    cpu->cores = malloc(coreCount * sizeof(Core *));
    if (!cpu->cores)
    {
        free(cpu);
        return NULL;
    }

    for (int i = 0; i < coreCount; i++)
    {
        cpu->cores[i] = malloc(sizeof(Core));
        if (!cpu->cores[i])
        {
            for (int j = 0; j < i; j++)
            {
                free(cpu->cores[j]);
            }
            free(cpu->cores);
            free(cpu);
            return NULL;
        }
        cpu->cores[i]->state = IDLE;
        cpu->cores[i]->processNode = NULL;
        cpu->cores[i]->switchOutTimer = 0;
        cpu->cores[i]->switchInTimer = 0;
        cpu->cores[i]->interruptTimer = 0;
        cpu->cores[i]->quantumTime = 0; // only used for RR
        cpu->cores[i]->continueOnCPU = false;
    }

    cpu->coreCount = coreCount;

    return cpu;
}

void freeCPU(CPU *cpu)
{
    for (int i = 0; i < cpu->coreCount; i++)
    {
        freeNode(cpu->cores[i]->processNode);
        free(cpu->cores[i]);
    }
    free(cpu->cores);
    free(cpu);
}

Disk *initDisk(void)
{
    Disk *disk = malloc(sizeof(Disk));
    if (!disk)
    {
        return NULL;
    }

    disk->state = DISK_IDLE;
    disk->processNode = NULL;

    return disk;
}

void freeDisk(Disk *disk)
{
    freeNode(disk->processNode);
    free(disk);
}

//need args
void handleInterrupt(Computer *computer)
{
    printf("\nInterrupt handler: pid=%d\n", computer->disk->processNode->pcb->pid);
    int interruptTimer = INTERRUPT_TIME;
    if (interruptTimer > 0)
    {
        computer->disk->state = DISK_IDLE;
        int core = rand() % (computer->cpu->coreCount); //should choose randomly one core for fairness ("no notion of core affinity")
        printf("core %d interrupted\n", core);
        computer->cpu->cores[core]->state = INTERRUPTED;
        computer->cpu->cores[core]->interruptTimer = interruptTimer; // start timer
        if (computer->cpu->cores[core]->processNode)
            computer->cpu->cores[core]->processNode->pcb->state = READY; //stays on the core to be able to "restart" when interrupt ended
    } /*else
    { //no interrupt: -> should deal with it in simulation
        computer->disk->state = DISK_IDLE;
        //need to deal with next event: * io -> waiting queue
        //                              * cpu -> readyqueue
        int index = getProcessIndex(workload, pid);
        ProcessEventType type = workload->processesInfo[index]->nextEvent->type;
        if (type == CPU_BURST)
            addProcessEventToGraph(graph, pid, time, READY, NO_CORE);
        else if (type == IO_BURST)
        {
            addProcessEventToGraph(graph, pid, time, WAITING, NO_CORE);
            printf("Next event in an IO: need to add to waiting queue");
            computer->cpu->cores[i]->continueOnCPU = false;
            handleProcessForDisk(computer->scheduler, computer->disk->processNode);
        }
    }*/
    return;
}


//debug:
const char* CPUstateToString(coreState state)
{
    switch (state)
    {
        case SWITCH_IN:
            return "SWITCH_IN";
        case SWITCH_OUT:
            return "SWITCH_OUT";
        case OCCUPIED:
            return "OCCUPIED";
        case INTERRUPTED:
            return "INTERRUPTED";
        case IDLE:
            return "IDLE";
        default:
            return "NOT A STATE: HUGE ERROR";
    }
}
void printCPUStates(CPU *cpu)
{
    for (int i = 0; i < cpu->coreCount; i++)
    {
        printf("core %d, at state %s ", i, CPUstateToString(cpu->cores[i]->state));
        if (cpu->cores[i]->processNode)
            printNode(cpu->cores[i]->processNode);
        printf(", s-in = %d, s-out = %d, interrupt=%d\n", cpu->cores[i]->switchInTimer, cpu->cores[i]->switchOutTimer, cpu->cores[i]->interruptTimer);
    }
}

const char* DiskStateToString(DiskState state)
{
    switch (state)
    {
        case DISK_RUNNING:
            return "DISK_RUNNING";
        case DISK_IDLE:
            return "DISK_IDLE";
        default:
            return "NOT A STATE: HUGE ERROR";
    }
}
void printDiskState(Disk *disk)
{
    printf("disk at state %s ", DiskStateToString(disk->state));
    if (disk->processNode)
        printf(", with pid %d\n", disk->processNode->pcb->pid);
    else
        printf(".\n");
}