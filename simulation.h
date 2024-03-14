#ifndef simulation_h
#define simulation_h

#include "graph.h"
#include "stats.h"
#include "schedulingAlgorithms.h"

/* ---------------------------- Workload struct ---------------------------- */

/* 
 * The Workload struct is used to store the processes that will be simulated.
 * It contains the information of the input file and the advancement of the
 * processes.
 */
typedef struct Workload_t Workload;


/* -------------------------- getters and setters -------------------------- */

/**
 * Get the total amount of processes in the workload.
 *
 * @param workload: the workload
 *
 * @return the amount of processes in the workload.
 */
int getProcessCount(const Workload *workload);

/**
 * Get the PID of the process at the given index in the workload
 *
 * @param workload: the workload
 * @param index: the index of the process
 *
 * @return the PID of the process at the given index
 */
int getPIDFromWorkload(Workload *workload, int index);

/**
 * Get the arrival time of the process with the given PID
 *
 * @param workload: the workload
 * @param pid: the PID of the process
 *
 * @return the arrival time of the process
 */
int getProcessStartTime(Workload *workload, int pid);

/**
 * Get the duration of the process with the given PID
 *
 * @param workload: the workload
 * @param pid: the PID of the process
 *
 * @return the duration of the process
 */
int getProcessDuration(Workload *workload, int pid);

/**
 * Get the advancement time of the process with the given PID
 *
 * @param workload: the workload
 * @param pid: the PID of the process
 *
 * @return the advancement time of the process
 */
int getProcessAdvancementTime(Workload *workload, int pid);

/**
 * Get the next event time of the process with the given PID. This can either
 * be the next CPU or IO event time or the total time if the process has no
 * more events.
 *
 * @param workload: the workload
 * @param pid: the PID of the process
 *
 * @return the next event time of the process
 */
int getProcessNextEventTime(Workload *workload, int pid);

/**
 * Get the current event time left of the process with the given PID. This can
 * either be the time left until the next CPU or IO event or the time left until
 * the process is finished if it has no more events.
 *
 * @param workload: the workload
 * @param pid: the PID of the process
 *
 * @return the current event time left of the process
 */
int getProcessCurEventTimeLeft(Workload *workload, int pid);


/* -------------------------- init/free functions -------------------------- */

/**
 * Create a new Workload struct and parse the input file to fill it with the
 * processes. Serves as init function for the Workload struct.
 *
 * @param fileName: the name of the input file
 *
 * @return the new Workload struct
 */
Workload *parseInputFile(const char *fileName);

/**
 * Free the memory of the given Workload struct.
 *
 * @param workload: the Workload struct to free
 */
void freeWorkload(Workload *workload);


/* -------------------------- simulation functions ------------------------- */

/**
 * Simulate the given workload with the given scheduling algorithms and store
 * the results in the given stats and graph struct.
 *
 * @param workload: the workload to simulate
 * @param algorithms: the algorithms to use for the simulation, each algorithm
 *                  will be simulated on its own ready queue in the order they
 *                  are given in the array (first algo is on first queue, etc.)
 * @param algorithmCount: the amount of algorithms/ready queues to use
 * @param cpuCoreCount: the amount of CPU cores to simulate on
 * @param graph: the graph struct to store the results of the simulation
 * @param stats: the stats struct to store the results of the simulation
 */
void launchSimulation(Workload *workload, SchedulingAlgorithm **algorithms, int algorithmCount, int cpuCoreCount, ProcessGraph *graph, AllStats *stats);

#endif // simulation_h
