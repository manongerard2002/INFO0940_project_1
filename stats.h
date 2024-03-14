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


AllStats *initStats(void);
// Also frees the ProcessStats inside
void freeStats(AllStats *stats);
void setNbProcessesInStats(AllStats *stats, int nbProcesses);
// Modify the ProcessStats after this call will modify the stats inside the
// AllStats
void addProcessStats(AllStats *stats, ProcessStats *processStats);
// Modify the ProcessStats after this call will modify the stats inside the
// AllStats
ProcessStats *getProcessStats(AllStats *stats, int processId);
void printStats(AllStats *stats);

#endif // stats_h
