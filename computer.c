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
        cpu->cores[i]->switchOutTimer = -1;
        cpu->cores[i]->switchInTimer = -1;
    }

    cpu->coreCount = coreCount;

    return cpu;
}

void freeCPU(CPU *cpu)
{
    for (int i = 0; i < cpu->coreCount; i++)
    {
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

    return disk;
}

void freeDisk(Disk *disk)
{
    free(disk);
}

int getSwitchInDuration() {
    return SWITCH_IN_DURATION;
}

int getSwitchOutDuration() {
    return SWITCH_OUT_DURATION;
}

//need args
void interruptHandler()
{
    printf("Interrupt handler");
    //to do
    return;
}


//debug:
const char* stateToString(coreState state) {
    switch (state) {
        case SWITCH_IN:
            return "SWITCH_IN";
        case SWITCH_OUT:
            return "SWITCH_OUT";
        case OCCUPIED:
            return "OCCUPIED";
        default:
            return "NOT A STATE: HUGE ERROR";
    }
}
void printCPUStates(CPU *cpu) {
    for (int i = 0; i < cpu->coreCount; i++)
    {
        printf("core %d, at state %s\n", i, stateToString(cpu->cores[i]->state));
    }
}