// You should complete the structures of the CPU and Disk in the header file
// and modify their initialization and freeing functions here.
// You should also implement the triggering of an interrupt here (and put its
// declaration in the header file to access it from simulation.c).

#include <stdio.h>
#include <stdlib.h>

#include "computer.h"
#include "schedulingLogic.h"

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
void handleInterrupt(Computer *computer, Workload *workload)
{
    int interruptTimer = INTERRUPT_TIME;
    if (interruptTimer > 0)
    {
        computer->disk->state = DISK_IDLE;
        int core = rand() % (computer->cpu->coreCount); //should choose randomly one core for fairness ("no notion of core affinity")
        computer->cpu->cores[core]->state = INTERRUPTED;
        computer->cpu->cores[core]->interruptTimer = interruptTimer; // start timer
        if (computer->cpu->cores[core]->processNode)
            computer->cpu->cores[core]->processNode->pcb->state = READY; //stays on the core to be able to "restart" when interrupt ended
    }
    else
    { //no interrupt: no need to interrupt a core
        computer->disk->state = DISK_IDLE;
        //the process that was waiting for the IO operation to complete will be put back on the ready queue.
        computer->disk->processNode->pcb->state = READY;
        advanceNextEvent(workload, computer->disk->processNode->pcb->pid); //schedulingLogic should not use the workload, but here thee is no other choice, we need to update the next event
        handleProcessForCPU(computer->scheduler, computer->disk->processNode);
        computer->disk->processNode = NULL;
    }
    return;
}