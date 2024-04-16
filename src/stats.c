// You should not need to look into this file. Just using the functions and
// structs provided in the header is sufficient.

#include <stdlib.h>
#include <stdio.h>

#include "stats.h"


/* -------------------------- struct definitions -------------------------- */

struct AllStats_t
{
    ProcessStats **processesStatsArray;

    int nbProcesses;
    int nbProcessesDone;
};


/* ------------------------- function definitions ------------------------- */

AllStats *initStats(void)
{
    AllStats *stats = (AllStats *) malloc(sizeof(AllStats));
    if (stats == NULL)
    {
        return NULL;
    }

    stats->nbProcesses = 0;
    stats->nbProcessesDone = 0;

    stats->processesStatsArray = NULL;

    return stats;
}

void freeStats(AllStats *stats)
{
    for (int i = 0; i < stats->nbProcessesDone; i++)
    {
        free(stats->processesStatsArray[i]);
    }
    free(stats->processesStatsArray);
    free(stats);
}

void setNbProcessesInStats(AllStats *stats, int nbProcesses)
{
    stats->processesStatsArray = (ProcessStats **) realloc(
            stats->processesStatsArray, nbProcesses * sizeof(ProcessStats *));

    stats->nbProcesses = nbProcesses;
}

void addProcessStats(AllStats *stats, ProcessStats *processStats)
{
    if (stats->nbProcessesDone >= stats->nbProcesses)
    {
        fprintf(stderr, "Error: not enough processes to set stats for process "
                "%d\n", processStats->processId);
        return;
    }

    stats->processesStatsArray[stats->nbProcessesDone++] = processStats;
}

ProcessStats *getProcessStats(AllStats *stats, int processId)
{
    // Yes, the complexity is O(n) but it's not a problem here. You can use it
    // as much as you want without being penalized.
    for (int i = 0; i < stats->nbProcessesDone; i++)
    {
        if (stats->processesStatsArray[i]->processId == processId)
        {
            return stats->processesStatsArray[i];
        }
    }

    return NULL;
}

void printStats(AllStats *stats)
{
    puts("PID | PRIORITY | ARRIVAL | FINISH | TURNAROUND | CPU | WAITING | "
         "RESPONSE | C. SWITCHES");

    for (int i = 0; i < stats->nbProcessesDone; i++)
    {
        const ProcessStats *processStats = stats->processesStatsArray[i];

        printf("%3d | %8d | %7d | %6d | %10d | %3d | %7d | %8.2f | %11d\n",
               processStats->processId, processStats->priority,
               processStats->arrivalTime, processStats->finishTime,
               processStats->turnaroundTime, processStats->cpuTime,
               processStats->waitingTime, processStats->meanResponseTime,
               processStats->nbContextSwitches);
    }
}
