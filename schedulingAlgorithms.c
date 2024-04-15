// You should not need to look into this file. Just using the functions and
// structs provided in the header is sufficient.

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "schedulingAlgorithms.h"

SchedulingAlgorithm *initSchedulingAlgorithm(SchedulingAlgorithmType type, int RRSliceLimit, int executiontTimeLimit, int ageLimit)
{
    SchedulingAlgorithm *algorithm = (SchedulingAlgorithm *) malloc(sizeof(SchedulingAlgorithm));
    if (!algorithm)
    {
        return NULL;
    }
    algorithm->type = type;
    algorithm->RRSliceLimit = RRSliceLimit;
    algorithm->executiontTimeLimit = executiontTimeLimit;
    algorithm->ageLimit = ageLimit;
    return algorithm;
}

void freeSchedulingAlgorithm(SchedulingAlgorithm *algorithm)
{
    free(algorithm);
}

void setAlgorithmType(SchedulingAlgorithm *algorithm, const char *type)
{
    if (strcmp(type, "FCFS") == 0)
    {
        algorithm->type = FCFS;
    }
    else if (strcmp(type, "RR") == 0)
    {
        algorithm->type = RR;
    }
    else if (strcmp(type, "SJF") == 0)
    {
        algorithm->type = SJF;
    }
    else if (strcmp(type, "PRIORITY") == 0)
    {
        algorithm->type = PRIORITY;
    }
    else
    {
        fprintf(stderr, "Error: Unknown algorithm type %s\n", type);
        exit(EXIT_FAILURE);
    }
}

//debug
const char* AlgoTypeToString(SchedulingAlgorithmType type)
{
    switch (type)
    {
        case RR:
            return "RR";
        case FCFS:
            return "FCFS";
        case SJF:
            return "SJF";
        case PRIORITY:
            return "PRIORITY";
        default:
            return "NOT A TYPE: HUGE ERROR";
    }
}

void printQueueAlgo(SchedulingAlgorithm *algorithm)
{
    printf("type=%s    RRRSliceLimit=%d   executiontTimeLimit=%d    ageLimit=%d\n", AlgoTypeToString(algorithm->type), algorithm->RRSliceLimit, algorithm->executiontTimeLimit, algorithm->ageLimit);
}
