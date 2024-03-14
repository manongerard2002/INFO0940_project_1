// You should not need to look into this file, but you can if you want to.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "graph.h"
#include "stats.h"
#include "simulation.h"
#include "utils.h"
#include "schedulingAlgorithms.h"

/**
 * Print the help message.
 */
static void printHelp(void)
{
    printf("Usage: ./cpuScheduler INPUT_FILE -c NB_CORES -q NB_READY_QUEUES ALGORITHM_OPTIONS_QUEUE_0 [ALGORITHM_OPTIONS_QUEUE_1] [...]\n");
    printf("ALGORITHM_OPTIONS: --algorithm=ALGORITHM\n");
    printf("                   [--RRSlice=RRSLICE_LIMIT]\n");
    printf("                   [--limit=TIME_LIMIT]\n");
    printf("                   [--age=AGE_LIMIT]\n");
}

/**
 * Parse the command line arguments.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 *
 * @return The scheduling algorithms for the ready queues.
 */
static SchedulingAlgorithm **parseArguments(int argc, const char* argv[])
{
    if (argc < 7)
    {
        fprintf(stderr, "Error: Invalid number of arguments\n");
        printHelp();
        exit(EXIT_FAILURE);
    }

    // Input file will be parsed later

    // Parse mandatory options
    int nbReadyQueues = 0;
    SchedulingAlgorithm **readyQueueAlgorithms = NULL;
    int i = 2;
    for (; i < 6; i += 2)
    {
        // we force the order of the parameters but whatever, it's easier that way
        if (i == 2 && strcmp(argv[i], "-c") == 0)
        {
            if (atoi(argv[i + 1]) < 1)
            {
                fprintf(stderr, "Error: Invalid number of cores\n");
                printHelp();
                exit(EXIT_FAILURE);
            }
        }
        else if (i == 4 && strcmp(argv[i], "-q") == 0)
        {
            nbReadyQueues = atoi(argv[i + 1]);
            if (nbReadyQueues < 1)
            {
                fprintf(stderr, "Error: Invalid number of ready queues\n");
                printHelp();
                exit(EXIT_FAILURE);
            }
            readyQueueAlgorithms = (SchedulingAlgorithm **) malloc(nbReadyQueues * sizeof(SchedulingAlgorithm *));
        } else
        {
            fprintf(stderr, "Error: Unknown option %s\n", argv[i]);
            printHelp();
            exit(EXIT_FAILURE);
        }
    }

    // Parse algorithm options
    int queueIndex = 0;
    while (i < argc)
    {
        if (queueIndex > nbReadyQueues)
        {
            printf("i: %d\n", i);
            printf("queueIndex: %d\n", queueIndex);
            printf("nbReadyQueues: %d\n", nbReadyQueues);
            fprintf(stderr, "Error: Too many algorithm options\n");
            printHelp();
            for (int j = queueIndex - 1; j >= 0; j--)
            {
                freeSchedulingAlgorithm(readyQueueAlgorithms[j]);
            }
            free(readyQueueAlgorithms);
            exit(EXIT_FAILURE);
        }

        // default values
        readyQueueAlgorithms[queueIndex] = initSchedulingAlgorithm(FCFS, NO_LIMIT, NO_LIMIT, NO_LIMIT);
        if (!readyQueueAlgorithms[queueIndex])
        {
            fprintf(stderr, "Error: Could not allocate memory for algorithm options\n");
            for (int j = queueIndex - 1; j >= 0; j--)
            {
                freeSchedulingAlgorithm(readyQueueAlgorithms[j]);
            }
            free(readyQueueAlgorithms);
            exit(EXIT_FAILURE);
        }

        if (strncmp(argv[i], "--algorithm=", 12) != 0)
        {
            fprintf(stderr, "Error: Missing algorithm option\n");
            printHelp();
            for (int j = queueIndex; j >= 0; j--)
            {
                freeSchedulingAlgorithm(readyQueueAlgorithms[j]);
            }
            free(readyQueueAlgorithms);
            exit(EXIT_FAILURE);
        }

        setAlgorithmType(readyQueueAlgorithms[queueIndex], argv[i] + 12);
        i++;
        while (i < argc && strncmp(argv[i], "--algorithm=", 12) != 0)
        {
            if (strncmp(argv[i], "--RRSlice=", 10) == 0)
            {
                readyQueueAlgorithms[queueIndex]->RRSliceLimit = atoi(argv[i] + 10);
            }
            else if (strncmp(argv[i], "--limit=", 8) == 0)
            {
                readyQueueAlgorithms[queueIndex]->executiontTimeLimit = atoi(argv[i] + 8);
            }
            else if (strncmp(argv[i], "--age=", 6) == 0)
            {
                readyQueueAlgorithms[queueIndex]->ageLimit = atoi(argv[i] + 6);
            }
            else
            {
                fprintf(stderr, "Error: Unknown algorithm option %s\n", argv[i]);
                printHelp();
                exit(EXIT_FAILURE);
            }
            i++;
        }

        queueIndex++;
    }

    return readyQueueAlgorithms;
}

/**
 * Main function.
 * Parses the arguments, initializes some structures, launches the simulation
 * and finally, prints the stats and graph.
 *
 * @param argc The number of arguments.
 * @param argv The arguments.
 *
 * @return The exit status.
 */
int main(int argc, const char *argv[])
{
    SchedulingAlgorithm **readyQueueAlgorithms = parseArguments(argc, argv);

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

    Workload *workload = parseInputFile(argv[1]);
    if (!workload)
    {
        freeGraph(graph);
        freeStats(allStats);
        return EXIT_FAILURE;
    }

    printVerbose("Starting simulation\n");
    launchSimulation(workload, readyQueueAlgorithms, atoi(argv[5]), atoi(argv[3]), graph, allStats);
    printVerbose("Simulation finished\n");

    printVerbose("Printing results\n");
    printf("----------------- Stats -----------------\n");
    printStats(allStats);
    printf("-----------------------------------------\n");
    printf("----------------- Graph -----------------\n");
    printGraph(graph);
    printf("-----------------------------------------\n");

    freeGraph(graph);
    freeStats(allStats);
    freeWorkload(workload);

    return EXIT_SUCCESS;
}
