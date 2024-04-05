#ifndef stats_h
#define stats_h

typedef struct AllStats_t AllStats;
typedef struct ProcessStats_t ProcessStats;

struct ProcessStats_t {
    int processId;
    int priority;
    int arrivalTime;
    int finishTime;
    int turnaroundTime;
    int cpuTime;
    // /!\ waiting to be executed -> waiting inside the ready queue, not the
    // wait queue
    int waitingTime;
    double meanResponseTime;
    int nbContextSwitches;
};


/**
 * Initializes a new instance of the AllStats structure.
 *
 * @return A pointer to the newly initialized AllStats structure.
 */
AllStats *initStats(void);

// Also frees the ProcessStats inside
/**
 * Frees the memory allocated for the AllStats structure.
 *
 * @param stats The pointer to the AllStats structure to be freed.
 */
void freeStats(AllStats *stats);
/**
 * Sets the number of processes in the statistics.
 *
 * @param stats The pointer to the AllStats structure.
 * @param nbProcesses The number of processes to set.
 */
void setNbProcessesInStats(AllStats *stats, int nbProcesses);

// Modify the ProcessStats after this call will modify the stats inside the
// AllStats

/**
 * Adds the statistics of a process to the overall system statistics.
 *
 * @param stats The pointer to the AllStats structure that holds the overall system statistics.
 * @param processStats The pointer to the ProcessStats structure that holds the statistics of a process.
 */
void addProcessStats(AllStats *stats, ProcessStats *processStats);
// Modify the ProcessStats after this call will modify the stats inside the
// AllStats


/**
 * Retrieves the process statistics for a given process ID.
 *
 * @param stats The pointer to the AllStats structure containing all the process statistics.
 * @param processId The ID of the process for which the statistics are to be retrieved.
 * @return A pointer to the ProcessStats structure containing the process statistics, or NULL if the process ID is not found.
 */
ProcessStats *getProcessStats(AllStats *stats, int processId);

/**
 * Prints the statistics of the given AllStats object.
 *
 * @param stats The pointer to the AllStats object.
 */
void printStats(AllStats *stats);

#endif // stats_h
