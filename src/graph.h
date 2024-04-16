#ifndef graph_h
#define graph_h

#include "process.h"

#define NO_CORE -1

typedef enum
{
    DISK_RUNNING,
    DISK_IDLE
} DiskState;

typedef struct ProcessGraph_t ProcessGraph;

/**
 * Initialize a new Graph that will be used to display the execution of the processes.
 *
 * @return a pointer to the new ProcessGraph
 */
ProcessGraph *initGraph(void);

/**
 * Free the memory allocated for the graph.
 *
 * @param graph: the graph to free
 */
void freeGraph(ProcessGraph *graph);

/**
 * Add a specific process to the graph by its pid.
 *
 * @param graph: the graph
 * @param processId: the id of the process to add
 */
void addProcessToGraph(ProcessGraph *graph, int processId);

/**
 * Add a process event (state of a process) to the graph.
 *
 * @param graph: the graph
 * @param processId: the id of the process
 * @param time: the time at which the event occurs
 * @param state: the state of the process
 * @param coreId: the id of the core on which the process is running
 */
void addProcessEventToGraph(ProcessGraph *graph, int processId, int time, ProcessState state, int coreId);

/**
 * Add a disk event (IO) to the graph.
 *
 * @param graph: the graph
 * @param processId: the id of the process
 * @param time: the time at which the event occurs
 * @param state: the state of the disk
 */
void addDiskEventToGraph(ProcessGraph *graph, int processId, int time, DiskState state);

/**
 * Print the graph to the console.
 *
 * @param graph: the graph to print
 */
void printGraph(ProcessGraph *graph);

#endif
