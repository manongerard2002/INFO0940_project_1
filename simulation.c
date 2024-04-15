// In this file, you should modify the main loop inside launchSimulation and
// use the workload structure (either directly or through the getters and
// setters).

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#include "simulation.h"
#include "process.h"
#include "utils.h"
#include "computer.h"
#include "schedulingLogic.h"

#define MAX_CHAR_PER_LINE 500


/* --------------------------- struct definitions -------------------------- */

/**
 * The ProcessEvent strcut represent processes events as they are in the input
 * file (CPU or IO). They are represented as a linked list where each event
 * points to the next one.
 */
typedef struct ProcessEvent_t ProcessEvent;
/**
 * The ProcessSimulationInfo struct contains all the input file information
 * and the advancement time of a particular process. The Workload struct
 * contains an array of ProcessSimulationInfo.
 */
typedef struct ProcessSimulationInfo_t ProcessSimulationInfo;

typedef enum
{
    CPU_BURST,
    IO_BURST, // For simplicity, we'll consider that IO bursts are blocking (among themselves)
} ProcessEventType;

struct ProcessEvent_t
{
    ProcessEventType type;
    int time; // Time at which the event occurs. /!\ time relative to the process
    ProcessEvent *nextEvent; // Pointer to the next event in the queue
};

struct ProcessSimulationInfo_t
{
    PCB *pcb;
    int startTime;
    int processDuration; // CPU + IO !
    int advancementTime; // CPU + IO !
    ProcessEvent *nextEvent; // Pointer to the next event after the current one
};

struct Workload_t
{
    ProcessSimulationInfo **processesInfo;
    int nbProcesses;
};

/* ---------------------------- static functions --------------------------- */

/**
 * Return the index of the process with the given pid in the array of processes
 * inside the workload.
 *
 * @param workload: the workload
 * @param pid: the pid of the process
 *
 * @return the index of the process in the workload
 */
static int getProcessIndex(Workload *workload, int pid);

/**
 * Set the advancement time of the process with the given pid in the workload.
 *
 * @param workload: the workload
 * @param pid: the pid of the process
 * @param advancementTime: the new advancement time
 */
static void setProcessAdvancementTime(Workload *workload, int pid, int advancementTime);

/*
 * Returns true if all processes in the workload have finished
 * (advancementTime == processDuration).
 *
 * @param workload: the workload
 * @return true if all processes have finished, false otherwise
 */
static bool workloadOver(const Workload *workload);

static void addAllProcessesToStats(AllStats *stats, Workload *workload);

/**
 * Compare function used in qsort before the main simulation loop. If you don't
 * use qsort, you can remove this function.
 *
 * @param a, b: pointers to ProcessSimulationInfo to compare
 *
 * @return < 0 if process a is first, > 0 if b is first, = 0 if they start at
 *         the same time
 */
//static int compareProcessStartTime(const void *a, const void *b);

/**
 * Retrieves the PCB (Process Control Block) from the given Workload at the specified index.
 *
 * @param workload The Workload from which to retrieve the PCB.
 * @param index The index of the PCB to retrieve.
 * @return A pointer to the PCB at the specified index, or NULL if the index is out of bounds.
 */
static PCB *getPCBFromWorkload(Workload *workload, int index);

/**
 * Handles the events of the simulation.
 *
 * @param computer The computer on which the simulation is running.
 * @param workload The workload of the simulation.
 * @param time The current time of the simulation.
 * @param stats The statistics of the simulation.
 */
static void handleSimulationEvents(Computer *computer, Workload *workload, int time, AllStats *stats);

//static int getNextProcessEventTime(Workload *Workload, Scheduler *scheduler);

/**
 * Handles the events of the simulation.
 *
 * @param scheduler The scheduler of the simulation.
 * @param workload The workload of the simulation.
 * @param time The current time of the simulation.
 * @param stats The statistics of the simulation.
 */
static void processArrived(Scheduler *scheduler, Workload *workload, int time, AllStats *stats);


/**
 * Advances the time of the processes in the workload and the scheduler.
 *
 * @param time The current time of the simulation.
 * @param nextTime The next time of the simulation.
 * @param workload The workload of the simulation.
 * @param computer The computer on which the simulation is running.
 * @param graph The graph of the simulation.
 * @param stats The statistics of the simulation.
 */
static void advanceProcessTime(int time, int nextTime, Workload *workload, Computer *computer, ProcessGraph *graph, AllStats *stats);

//int minTime(int time, int nextTime);

/* -------------------------- getters and setters -------------------------- */

int getProcessCount(const Workload *workload)
{
    return workload->nbProcesses;
}

int getPIDFromWorkload(Workload *workload, int index)
{
    return workload->processesInfo[index]->pcb->pid;
}

int getProcessStartTime(Workload *workload, int pid)
{
    for (int i = 0; i < workload->nbProcesses; i++)
    {
        if (getPIDFromWorkload(workload, i) == pid)
        {
            return workload->processesInfo[i]->startTime;
        }
    }
    return -1;
}

int getProcessDuration(Workload *workload, int pid)
{
    for (int i = 0; i < workload->nbProcesses; i++)
    {
        if (getPIDFromWorkload(workload, i) == pid)
        {
            return workload->processesInfo[i]->processDuration;
        }
    }
    return -1;
}

int getProcessAdvancementTime(Workload *workload, int pid)
{
    for (int i = 0; i < workload->nbProcesses; i++)
    {
        if (getPIDFromWorkload(workload, i) == pid)
        {
            return workload->processesInfo[i]->advancementTime;
        }
    }
    return -1;
}

int getProcessNextEventTime(Workload *workload, int pid)
{
    int processNextEventTime = getProcessDuration(workload, pid); // relative to the process
    for (int i = 0; i < workload->nbProcesses; i++)
    {
        if (getPIDFromWorkload(workload, i) != pid)
        {
            continue;
        }
        if (workload->processesInfo[i]->nextEvent)
        {
            processNextEventTime = workload->processesInfo[i]->nextEvent->time;
        }
        break;
    }
    return processNextEventTime;
}

int getProcessCurEventTimeLeft(Workload *workload, int pid)
{
    return getProcessNextEventTime(workload, pid)
           - getProcessAdvancementTime(workload, pid);
}

static int getProcessIndex(Workload *workload, int pid)
{
    int processIndex = 0;
    for (; processIndex < workload->nbProcesses; processIndex++)
    {
        if (getPIDFromWorkload(workload, processIndex) != pid)
        {
            continue;
        }
        break;
    }

    return processIndex;
}

static void setProcessAdvancementTime(Workload *workload, int pid, int advancementTime)
{
    for (int i = 0; i < workload->nbProcesses; i++)
    {
        if (getPIDFromWorkload(workload, i) == pid)
        {
            workload->processesInfo[i]->advancementTime = advancementTime;
            return;
        }
    }
}

static PCB *getPCBFromWorkload(Workload *workload, int index)
{
    return workload->processesInfo[index]->pcb;
}

/* -------------------------- init/free functions -------------------------- */

Workload *parseInputFile(const char *fileName)
{
    printVerbose("Parsing input file...\n");
    FILE *file = fopen(fileName, "r");
    if (!file)
    {
        fprintf(stderr, "Error: could not open file %s\n", fileName);
        return NULL;
    }

    Workload *workload = (Workload *) malloc(sizeof(Workload));
    if (!workload)
    {
        fprintf(stderr, "Error: could not allocate memory for workload\n");
        fclose(file);
        return NULL;
    }

    char line[MAX_CHAR_PER_LINE];
    int nbProcesses = 0;
    // 1 line == 1 process
    while (fgets(line, MAX_CHAR_PER_LINE, file))
    {
        if (line[strlen(line) - 1] != '\n')
        {
            fprintf(stderr, "Error: line too long in the input file.\n");
            freeWorkload(workload);
            fclose(file);
            return NULL;
        }
        if (line[0] == '#' || line[0] == '\n')
        {
            continue;
        }
        nbProcesses++;
    }
    
    workload->processesInfo = (ProcessSimulationInfo **) malloc(
            sizeof(ProcessSimulationInfo *) * nbProcesses);
    if (!workload->processesInfo)
    {
        fprintf(stderr, "Error: could not allocate memory for processes info\n");
        freeWorkload(workload);
        fclose(file);
        return NULL;
    }

    workload->nbProcesses = 0;

    rewind(file);
    while (fgets(line, MAX_CHAR_PER_LINE, file)) // Read file line by line
    {
        if (line[0] == '#' || line[0] == '\n')
        {
            continue;
        }

        ProcessSimulationInfo *processInfo = (ProcessSimulationInfo *) malloc(
                sizeof(ProcessSimulationInfo));
        if (!processInfo)
        {
            fprintf(stderr, "Error: could not allocate memory for process info\n");
            freeWorkload(workload);
            fclose(file);
            return NULL;
        }

        processInfo->pcb = (PCB *) malloc(sizeof(PCB));
        if (!processInfo->pcb)
        {
            fprintf(stderr, "Error: could not allocate memory for PCB\n");
            free(processInfo);
            freeWorkload(workload);
            fclose(file);
            return NULL;
        }

        processInfo->pcb->state = READY;

        char *token = strtok(line, ",");
        processInfo->pcb->pid = atoi(token);

        token = strtok(NULL, ",");
        processInfo->startTime = atoi(token);

        token = strtok(NULL, ",");
        processInfo->processDuration = atoi(token);

        token = strtok(NULL, ",");
        processInfo->pcb->priority = atoi(token);

        processInfo->advancementTime = 0;

        token = strtok(NULL, "(");

        ProcessEvent *event = NULL;
        while (strstr(token, ",") || strstr(token, "[")) // Read events
        {
            if (strstr(token, "[")) // first event
            {
                event = (ProcessEvent *) malloc(sizeof(ProcessEvent));
                processInfo->nextEvent = event;
            }
            else
            {
                event->nextEvent = (ProcessEvent *) malloc(sizeof(ProcessEvent));
                event = event->nextEvent;
            }
            if (!event)
            {
                fprintf(stderr, "Error: could not allocate memory for event\n");
                free(processInfo->pcb);
                free(processInfo);
                freeWorkload(workload);
                fclose(file);
                return NULL;
            }

            token = strtok(NULL, ",");
            event->time = atoi(token);

            token = strtok(NULL, ")");

            if (token != NULL)
            {
                if (strstr(token, "CPU"))
                {
                    event->type = CPU_BURST;
                }
                else if (strstr(token, "IO"))
                {
                    event->type = IO_BURST;
                }
                else
                {
                    fprintf(stderr, "Error: Unknown operation type\n");
                }
            }

            event->nextEvent = NULL;
            token = strtok(NULL, "(");
        } // End of events
        workload->processesInfo[workload->nbProcesses] = processInfo;
        workload->nbProcesses++;
    } // End of file

    fclose(file);

    printVerbose("Input file parsed successfully\n");

    return workload;
}

void freeWorkload(Workload *workload)
{
    for (int i = 0; i < workload->nbProcesses; i++)
    {
        ProcessEvent *event = workload->processesInfo[i]->nextEvent;
        while (event)
        {
            ProcessEvent *nextEvent = event->nextEvent;
            free(event);
            event = nextEvent;
        }
        free(workload->processesInfo[i]->pcb);
        free(workload->processesInfo[i]);
    }
    free(workload->processesInfo);
    free(workload);
}


/* ---------------------------- other functions ---------------------------- */

void launchSimulation(Workload *workload, SchedulingAlgorithm **algorithms, int algorithmCount, int cpuCoreCount, ProcessGraph *graph, AllStats *stats)
{
    for (int i = 0; i < getProcessCount(workload); i++)
    {
        addProcessToGraph(graph, getPIDFromWorkload(workload, i));
    }
    setNbProcessesInStats(stats, getProcessCount(workload));

    Scheduler *scheduler = initScheduler(algorithms, algorithmCount, workload, cpuCoreCount);
    if (!scheduler)
    {
        fprintf(stderr, "Error: could not initialize scheduler\n");
        return;
    }

    CPU *cpu = initCPU(cpuCoreCount);
    if (!cpu)
    {
        fprintf(stderr, "Error: could not initialize CPU\n");
        freeScheduler(scheduler);
        return;
    }

    Disk *disk = initDisk();
    if (!disk)
    {
        fprintf(stderr, "Error: could not initialize disk\n");
        freeCPU(cpu);
        freeScheduler(scheduler);
        return;
    }

    Computer *computer = initComputer(scheduler, cpu, disk);
    if (!computer)
    {
        fprintf(stderr, "Error: could not initialize computer\n");
        freeDisk(disk);
        freeCPU(cpu);
        freeScheduler(scheduler);
        return;
    }

    addAllProcessesToStats(stats, workload);

    int time = 0;
    /* Main loop of the simulation.*/
    while (!workloadOver(workload)) // You probably want to change this condition
    {
        printf("----------------------------------------------------------------in while time: %d----------------------------------------\n", time);
        // TODO
        //1. Handle event(s): simulator and the scheduler check if an event is triggered at the current time unit and handle it
        handleSimulationEvents(computer, workload, time, stats);

        //2. Assign processes to resources: This is the step where the main scheduling decisions are made:
        //choosing what processes to execute next.
        assignProcessesToResources(computer);

        /*//3. Get next event time: Here, the simulator and scheduler will simply check what is the time unit
        //of the next event in order to jump directly to it in the next step.
        int nextProcessEventTime = getNextProcessEventTime(workload, scheduler);
        //int nextSchedulingEventTime = getNextSchedulingEventTime(workload, scheduler);
        int nextTime = nextProcessEventTime;//(nextProcessEventTime, nextSchedulingEventTime);
        printf("time=%d    next event time= %d\n", time, nextTime);*/

        //4. Advance time to the next event: This is where the progression of time is simulated. The simulator
        //will update the advancement of the processes in the workload and the scheduler will update its
        //timers.
        int nextTime=time+1; //tmp
        advanceProcessTime(time, nextTime, workload, computer, graph, stats);
        advanceSchedulingTime(time, nextTime, computer);

        printStats(stats);
        printGraph(graph);
        printCPUStates(computer->cpu);
        printDiskState(computer->disk);
        printReadyQueues(computer->scheduler);
        
        //Then the current time can be updated too in order to deal with the next event at the next iteration of the loop.
        if (nextTime == - 1 || time == nextTime)
            break;
        time = nextTime;
        
        //Maximum iteration to avoid infinite loop for preemptiveness
        if (time > 6)
            break;
    }
    freeComputer(computer);
}

/* ---------------------------- static functions --------------------------- */

static bool workloadOver(const Workload *workload)
{
    //disk idle, cpu idle, readyqueue empty & no more next event
    printProcessStates(workload);
    for (int i = 0; i < workload->nbProcesses; i++)
    {
        //if (workload->processesInfo[i]->advancementTime < workload->processesInfo[i]->processDuration)
        if (workload->processesInfo[i]->pcb->state != TERMINATED)
        {
            return 0;
        }
    }

    return 1;
}

static void addAllProcessesToStats(AllStats *stats, Workload *workload)
{
    for (int i = 0; i < workload->nbProcesses; i++)
    {
        ProcessStats *processStats = (ProcessStats *) malloc(sizeof(ProcessStats));
        if (!processStats)
        {
            fprintf(stderr, "Error: could not allocate memory for process stats\n");
            return;
        }
        processStats->processId = getPIDFromWorkload(workload, i);
        processStats->priority = workload->processesInfo[i]->pcb->priority;
        processStats->arrivalTime = 0;
        processStats->finishTime = 0;
        processStats->turnaroundTime = 0;
        processStats->cpuTime = 0;
        processStats->waitingTime = 0;
        processStats->meanResponseTime = 0;
        // You could want to put this field to -1
        processStats->nbContextSwitches = 0;

        addProcessStats(stats, processStats);
    }
}

/*static int compareProcessStartTime(const void *a, const void *b)
{
    const ProcessSimulationInfo *infoA = *(const ProcessSimulationInfo **)a;
    const ProcessSimulationInfo *infoB = *(const ProcessSimulationInfo **)b;

    if (infoA->startTime < infoB->startTime)
    {
        return -1;
    }
    else if (infoA->startTime > infoB->startTime)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}*/

void advanceNextEvent(Workload *workload, int pid)
{
    int index = getProcessIndex(workload, pid);
    ProcessEvent *event = workload->processesInfo[index]->nextEvent;
    ProcessEvent *nextEvent = event->nextEvent;
    free(event);
    workload->processesInfo[index]->nextEvent = nextEvent;
}

//get the time of the next process that has an event to be able to advance to it in the simulation
/*static int getNextProcessEventTime(Workload *workload, Scheduler *scheduler)
{
    //next event: soit il y a plus rien, process arrive, disk fini, IO ..., CPU
    int time = -1;
    for (int i = 0; i < getProcessCount(workload); i++)
    {
        int pid = getPIDFromWorkload(workload, i);
        if (pid != -1)
        {
            int nextTime = getProcessNextEventTime(workload, pid) + getProcessStartTime(workload, pid); //need "real" time not of th eprocess
            printf("nextPevent: time=%d    nextTime=%d\n", time, nextTime);
            time = minTime(time, nextTime);
        }
    }
    return time;
}*/

static void handleSimulationEvents(Computer *computer, Workload *workload, int time, AllStats *stats)
{
    //Ex: if a process arrives in the system, the simulator will call the scheduler to put the process in the ready queue.
    printf("\nbeginning of handleEvents\n");
    processArrived(computer->scheduler, workload, time, stats);

    handleSchedulerEvents(computer, time, stats);
    
    //Ex: event = hardware events, such as the triggering of an interrupt.
    if (computer->disk->processNode && getProcessCurEventTimeLeft(workload, computer->disk->processNode->pcb->pid) == 0) //terminated -> interrupt
    {
        //start interrupt
        printf("process %d begins interrupt\n", computer->disk->processNode->pcb->pid);
        handleInterrupt(computer);
    }
}

void processArrived(Scheduler *scheduler, Workload *workload, int time, AllStats *stats)
{
    for (int i = 0; i < getProcessCount(workload); i++)
    {
        int pid = getPIDFromWorkload(workload, i);
        //"The first event will always be a CPU event (at time 0)"
        if (getProcessStartTime(workload, pid) == time)
        {
            printf("process %d arrived at time %d\n", pid, time);
            PCB *pcb = getPCBFromWorkload(workload, i);
            pcb->state = READY;
            Node *node = initNode(pcb);
            advanceNextEvent(workload, pid);
            handleProcessForCPU(scheduler, node);
            getProcessStats(stats, pid)->arrivalTime = time;
        }
    }
}

void advanceProcessTime(int time, int nextTime, Workload *workload, Computer *computer, ProcessGraph *graph, AllStats *stats)
{
    int deltaTime = nextTime - time;
    for (int i = 0; i < workload->nbProcesses; i++)
    {
        ProcessState state = workload->processesInfo[i]->pcb->state;
        int pid = getPIDFromWorkload(workload, i);
        switch (state)
        {
            case READY:
                //Process ready by default => need to differenciate
                // * READY: process not started: no graph
                // * READY: process in readyQueue: update graph/stats
                // * READY: process in switch-in/switch-out
                // * READY: process interrupted
                if (getProcessStartTime(workload, pid) <= time)
                {
                    if (processInReadyQueues(computer->scheduler, pid))
                        getProcessStats(stats, pid)->waitingTime += deltaTime;
                    addProcessEventToGraph(graph, pid, time, state, NO_CORE);
                }
                break;
            case WAITING:
                addProcessEventToGraph(graph, pid, time, state, NO_CORE);
                printDiskState(computer->disk);
                if (computer->disk->state == DISK_RUNNING && computer->disk->processNode->pcb->pid == pid)
                {
                    printf("add disk running in graph\n");
                    setProcessAdvancementTime(workload, pid, getProcessAdvancementTime(workload, pid) + deltaTime);
                    addDiskEventToGraph(graph, pid, time, DISK_RUNNING);
                }
                else if (computer->disk->state == DISK_IDLE && computer->disk->processNode->pcb->pid == pid)
                { //debug
                    printf("add disk idle while interrupt\n");
                    addDiskEventToGraph(graph, pid, time, DISK_IDLE);
                }
                else
                {
                    printf("process %d waiting in the waiting queue", pid);
                }
               break;
            case RUNNING:
                getProcessStats(stats, pid)->cpuTime += deltaTime;
                int core;
                for (core = 0; core < computer->cpu->coreCount; core++)
                    if (computer->cpu->cores[core]->processNode && computer->cpu->cores[core]->processNode->pcb->pid == pid)
                        break;
                addProcessEventToGraph(graph, pid, time, state, core);
                setProcessAdvancementTime(workload, pid, getProcessAdvancementTime(workload, pid) + deltaTime);
                break;
            case TERMINATED:
                addProcessEventToGraph(graph, pid, time, state, NO_CORE);
            default:
                break;
        }
    }

    if (computer->disk->state == DISK_IDLE)
    {
        //Issue on submission platform if not present
        addDiskEventToGraph(graph, -1, time, DISK_IDLE);
    }
}

//to deal with time = -1
/*int minTime(int time, int nextTime)
{
    if (time < 0)
        return nextTime;
    else if (nextTime < 0)
        return time;
    else
        return min(time, nextTime);
}*/

//debug:
const char* ProcessStateToString(ProcessState state)
{
    switch (state)
    {
        case READY:
            return "READY";
        case RUNNING:
            return "RUNNING";
        case WAITING:
            return "WAITING";
        case TERMINATED:
            return "TERMINATED";
        default:
            return "NOT A STATE: HUGE ERROR";
    }
}

void printProcessStates(const Workload *workload)
{
    for (int i = 0; i < workload->nbProcesses; i++)
    {
        printf("process at index %d, with pid %d, at state %s, ", i, workload->processesInfo[i]->pcb->pid, ProcessStateToString(workload->processesInfo[i]->pcb->state));
        printf("processDuration=%d and advancementTime=%d\n", workload->processesInfo[i]->processDuration, workload->processesInfo[i]->advancementTime);
    }
}