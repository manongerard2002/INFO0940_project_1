#ifndef computer_h
#define computer_h

#include <stdbool.h>

#include "process.h"
#include "schedulingAlgorithms.h"
#include "schedulingLogic.h"

#define SWITCH_OUT_DURATION 2 // Duration of the context switch out
#define SWITCH_IN_DURATION 1 // Duration of the context switch in

typedef struct CPU_t CPU;
typedef struct Core_t Core;
typedef struct Disk_t Disk;

/* ---------------------------- Computer struct ---------------------------- */

/**
 * @struct Computer_t
 * @brief Represents a computer.
 *
 * This struct is used to define the properties and behavior of a computer.
 * It can be used to store information such as the computer's model, processor,
 * memory, and other relevant details.
 */

struct Computer_t
{
    Scheduler *scheduler;
    CPU *cpu;
    Disk *disk;
};


/* ------------------------------- CPU struct ------------------------------ */

/**
 * @brief Enumeration representing the different types of computers.
 */
typedef enum
{
    SWITCH_IN,
    SWITCH_OUT,
    INTERRUPTED,
    OCCUPIED,//RUNNING,
    IDLE
} coreState;

/* ------------------------------- Cpu struct ------------------------------ */
/**
 * @struct CPU_t
 * Represents the CPU (Central Processing Unit) of a computer.
 * This struct contains the necessary attributes and methods to manage the CPU.
 */
struct CPU_t
{
    // list of cores
    Core **cores;
    int coreCount;
};

/* ------------------------------- Core struct ------------------------------ */

/**
 * @struct Core_t
 * Represents a core of a CPU.
 * This struct contains the necessary attributes and methods to manage a core.
 */
struct Core_t
{
    coreState state;
    Node *processNode;
    int switchInTimer; //0 when not in switch in
    int switchOutTimer; //0 when not in switch out
    int interruptTimer; //0 when not during an interrupt
    int quantumTime;
    bool continueOnCPU;
};


/* ------------------------------ Disk struct ------------------------------ */

struct Disk_t
{
    DiskState state; //used the samee state than the graph
    Node *processNode;
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

/**
 * Handles an interrupt for the given computer.
 *
 * @param computer The computer object to handle the interrupt for.
 */
void handleInterrupt(Computer *computer);


// debug: to remove after
//debug:


/**
 * Converts the given CPU core state to a string representation.
 *
 * @param state The CPU core state to convert.
 * @return A string representation of the CPU core state.
 */
const char* CPUstateToString(coreState state);


/**
 * Prints the CPU states.
 *
 * This function takes a pointer to a CPU structure and prints the states of the CPU.
 *
 * @param cpu A pointer to the CPU structure.
 */
void printCPUStates(CPU *cpu);

/**
 * Converts the given Disk state to a string representation.
 *
 * @param state The Disk state to convert.
 * @return A string representation of the Disk state.
 */
const char* DiskStateToString(DiskState state);

/**
 * Prints the Disk state.
 *
 * This function takes a pointer to a Disk structure and prints the state of the Disk.
 *
 * @param disk A pointer to the Disk structure.
 */
void printDiskState(Disk *disk);

#endif // computer_h
