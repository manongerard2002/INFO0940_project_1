// This is not part of the assignment. It is just an example to show you how to
// use the graph and stats functions
// To compile, you can run: make example and then run ./example

#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "stats.h"

int main(void)
{
    ProcessGraph *graph = initGraph();
    if (!graph)
    {
        return EXIT_FAILURE;
    }

    AllStats *allStats = initStats();
    if (!allStats)
    {
        freeGraph(graph);
        return EXIT_FAILURE;
    }

    // graph example

    addProcessToGraph(graph, 1);
    addProcessToGraph(graph, 2);
    addProcessToGraph(graph, 3);

    addProcessEventToGraph(graph, 1, 0, RUNNING, 0);
    addProcessEventToGraph(graph, 2, 0, READY, NO_CORE);
    addProcessEventToGraph(graph, 3, 0, READY, NO_CORE);

    addProcessEventToGraph(graph, 1, 2, WAITING, NO_CORE);
    addProcessEventToGraph(graph, 2, 2, RUNNING, 0);
    addDiskEventToGraph(graph, 1, 2, DISK_RUNNING);

    addProcessEventToGraph(graph, 2, 4, READY, NO_CORE);
    addDiskEventToGraph(graph, 1, 4, DISK_IDLE);

    addProcessEventToGraph(graph, 1, 5, READY, NO_CORE);
    addProcessEventToGraph(graph, 2, 5, RUNNING, 0);

    addProcessEventToGraph(graph, 2, 18, TERMINATED, NO_CORE);
    addProcessEventToGraph(graph, 3, 18, RUNNING, 0);

    addProcessEventToGraph(graph, 1, 23, RUNNING, 0);
    addProcessEventToGraph(graph, 3, 23, TERMINATED, NO_CORE);

    addProcessEventToGraph(graph, 1, 29, TERMINATED, NO_CORE);

    printGraph(graph);
    puts("");

    // stats example

    setNbProcessesInStats(allStats, 3);

    ProcessStats *processStats1 = (ProcessStats *) malloc(sizeof(ProcessStats));
    processStats1->processId = 1;
    processStats1->priority = 1;
    processStats1->arrivalTime = 0;
    processStats1->finishTime = 29;
    processStats1->turnaroundTime = 29;
    processStats1->cpuTime = 8;
    processStats1->waitingTime = 18;
    processStats1->meanResponseTime = 9;
    processStats1->nbContextSwitches = 1;

    ProcessStats *processStats2 = (ProcessStats *) malloc(sizeof(ProcessStats));

    ProcessStats *processStats3 = (ProcessStats *) malloc(sizeof(ProcessStats));
    processStats3->processId = 3;
    processStats3->priority = 2;
    processStats3->arrivalTime = 0;
    processStats3->finishTime = 23;
    processStats3->turnaroundTime = 23;
    processStats3->cpuTime = 5;
    processStats3->waitingTime = 18;
    processStats3->meanResponseTime = 18;
    processStats3->nbContextSwitches = 0;

    addProcessStats(allStats, processStats1);
    addProcessStats(allStats, processStats2);
    addProcessStats(allStats, processStats3);

    processStats2->processId = 2;
    processStats2->priority = 3;
    processStats2->arrivalTime = 0;
    processStats2->finishTime = 18;
    processStats2->turnaroundTime = 18;
    processStats2->cpuTime = 15;
    processStats2->waitingTime = 2;
    processStats2->meanResponseTime = 2;
    processStats2->nbContextSwitches = 0;

    printStats(allStats);
}

