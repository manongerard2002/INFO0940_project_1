#ifndef computer_h
#define computer_h

#include <stdbool.h>

#include "process.h"
#include "schedulingAlgorithms.h"
#include "schedulingLogic.h"

#define SWITCH_OUT_DURATION 2 // Duration of the context switch out
#define SWITCH_IN_DURATION 1  // Duration of the context switch in

typedef struct CPU_t CPU;
typedef struct Core_t Core;
typedef struct Disk_t Disk;

/* ---------------------------- Computer struct ---------------------------- */

struct Computer_t
{
    Scheduler *scheduler;
    CPU *cpu;
    Disk *disk;
};


/* ------------------------------- CPU struct ------------------------------ */

typedef enum
{
    SWITCH_IN,
    SWITCH_OUT,
    OCCUPIED,//RUNNING,
    IDLE
} coreState;

struct CPU_t
{
    // list of cores
    Core **cores;
    int coreCount;
};

struct Core_t
{
    coreState state;
    int PID;
    int switchInTimer; //-1 when not in switch in
    int switchOutTimer; //-1 when not in switch out
};


/* ------------------------------ Disk struct ------------------------------ */

/*typedef enum
{
    DISK_RUNNING,
    DISK_IDLE
} DiskState;*/

struct Disk_t
{
    DiskState state; //like in graph
};

/* ------------------------- function definitions -------------------------
 * These functions respectively initialize and free the computer, CPU and Disk.
 * For the CPU, it initializes the number of cores (coreCount) that will be used.
 */

Computer *initComputer(Scheduler *scheduler, CPU *cpu, Disk *disk);

/**
 * Frees the memory allocated for a Computer object.
 *
 * @param computer The Computer object to be freed.
 */
void freeComputer(Computer *computer);

/**
 * Initializes a CPU with the specified number of cores.
 *
 * @param coreCount The number of cores to initialize the CPU with.
 * @return A pointer to the initialized CPU.
 */
CPU *initCPU(int coreCount);

/**
 * Frees the memory allocated for a CPU object.
 *
 * @param cpu The CPU object to be freed.
 */
void freeCPU(CPU *cpu);


/**
 * Initializes a disk.
 *
 * @return A pointer to the initialized Disk object.
 */
Disk *initDisk(void);

/**
 * Frees the memory allocated for a Disk object.
 *
 * @param disk The Disk object to be freed.
 */
void freeDisk(Disk *disk);

int getSwitchInDuration();

int getSwitchOutDuration();

void interruptHandler();

#endif // computer_h
