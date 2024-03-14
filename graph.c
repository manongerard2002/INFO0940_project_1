// You should not need to look into this file. Just using the functions
// provided in the header is sufficient.

#include <stdlib.h>
#include <stdio.h>

#include "graph.h"

#define MAX_PROCESSES 100
#define MAX_TIME_SLOTS 10000

#define WAITING_SYMBOL '.'
#define READY_SYMBOL '-'
#define TERMINATED_SYMBOL ' '
#define DISK_IDLE_SYMBOL ' '


/* -------------------------- struct definitions -------------------------- */

typedef struct Event_t Event;
typedef struct Process_t Process;
typedef struct Disk_t Disk;

struct Event_t
{
    char symbol;           // (= for RUNNING, . for WAITING, - for READY)
    int time;              // Time at which the event occurs
    Event *nextEvent;      // Pointer to the next event in the queue
};

struct Process_t
{
    int processId;              // Process id
    Event *firstEvent;          // Pointer to first event in the queue
};

struct Disk_t
{
    Event *firstEvent;
};

struct ProcessGraph_t
{
    int nbProcesses;
    int nbTimeSlots;
    Process processes[MAX_PROCESSES];
    Disk *disk;
};


/* ---------------------------- static functions --------------------------- */

static Event *createNewEventForGraph(ProcessGraph *graph, int time);
static char idToSymbol(int id);


/* ------------------------- function definitions ------------------------- */

ProcessGraph *initGraph(void)
{
    ProcessGraph *graph = (ProcessGraph *) malloc(sizeof(ProcessGraph));
    if (!graph)
    {
        return NULL;
    }

    graph->nbProcesses = 0;
    graph->nbTimeSlots = 0;
    for (int i = 0; i < MAX_PROCESSES; i++)
    {
        graph->processes[i].firstEvent = NULL;
    }
    graph->disk = (Disk *) malloc(sizeof(Disk));
    graph->disk->firstEvent = NULL;

    return graph;
}

void freeGraph(ProcessGraph *graph)
{
    for (int i = 0; i <= graph->nbProcesses; i++)
    {
        Event *currentEvent = NULL;
        if (i == graph->nbProcesses)
        {
            currentEvent = graph->disk->firstEvent;
        }
        else
        {
            currentEvent = graph->processes[i].firstEvent;
        }
        while (currentEvent != NULL)
        {
            Event *nextEvent = currentEvent->nextEvent;
            free(currentEvent);
            currentEvent = nextEvent;
        }
    }
    free(graph->disk);
    free(graph);
}

void addProcessToGraph(ProcessGraph *graph, int processId)
{
    if (graph->nbProcesses >= MAX_PROCESSES)
    {
        fprintf(stderr, "Error: Maximum number of processes reached\n");
        return;
    }

    graph->processes[graph->nbProcesses].processId = processId;
    graph->nbProcesses++;
}

void addProcessEventToGraph(ProcessGraph *graph, int processId, int time, ProcessState state, int coreId)
{
    Event *event = createNewEventForGraph(graph, time);
    if (!event)
    {
        return;
    }

    switch (state)
    {
        case RUNNING:
            event->symbol = idToSymbol(coreId);
            break;
        case WAITING:
            event->symbol = WAITING_SYMBOL;
            break;
        case READY:
            event->symbol = READY_SYMBOL;
            break;
        case TERMINATED:
            event->symbol = TERMINATED_SYMBOL;
            break;
        default:
            fprintf(stderr, "Error: Invalid process state\n");
            return;
    }

    int i = 0;
    for (i = 0; i < graph->nbProcesses; i++)
    {
        if (graph->processes[i].processId == processId)
        {
            break;
        }
    }
    if (i >= graph->nbProcesses)
    {
        fprintf(stderr, "Error: Process not found\n");
        return;
    }

    if (graph->processes[i].firstEvent == NULL)
    {
        graph->processes[i].firstEvent = event;
    }
    else
    {
        Event *currentEvent = graph->processes[i].firstEvent;
        while (currentEvent->nextEvent != NULL)
        {
            currentEvent = currentEvent->nextEvent;
        }
        currentEvent->nextEvent = event;
    }
}

void addDiskEventToGraph(ProcessGraph *graph, int processId, int time, DiskState state)
{
    Event *event = createNewEventForGraph(graph, time);
    if (!event)
    {
        return;
    }

    switch (state)
    {
        case DISK_RUNNING:
            event->symbol = idToSymbol(processId);
            break;
        case DISK_IDLE:
            event->symbol = DISK_IDLE_SYMBOL;
            break;
        default:
            fprintf(stderr, "Error: Invalid state for disk\n");
            return;
    }

    if (graph->disk->firstEvent == NULL)
    {
        graph->disk->firstEvent = event;
    }
    else
    {
        Event *currentEvent = graph->disk->firstEvent;
        while (currentEvent->nextEvent != NULL)
        {
            currentEvent = currentEvent->nextEvent;
        }
        currentEvent->nextEvent = event;
    }
}

void printGraph(ProcessGraph *graph)
{
    printf("Process");
    for (int i = 0; i < graph->nbTimeSlots; i += 5)
    {
        printf("%5d", i);
    }
    printf("\n");

    for (int i = 0; i < graph->nbProcesses; i++)
    {
        // print process id
        printf("%7d    ", graph->processes[i].processId);

        Event *currentEvent = graph->processes[i].firstEvent;
        if (!currentEvent)
        {
            printf("\n");
            continue;
        }

        // print spaces until the first event
        int t = 0;
        for (; t < currentEvent->time && t < graph->nbTimeSlots; t++)
        {
            printf(" ");
        }

        // print the events
        for (; t < graph->nbTimeSlots; t++)
        {
            // while loop in case multiple events occur at the same time
            // (for example if a process is immediately RUNNING when set READY
            // and there is no switch time)
            while (currentEvent->nextEvent && t == currentEvent->nextEvent->time)
            {
                currentEvent = currentEvent->nextEvent;
            }

            printf("%c", currentEvent->symbol);
        }
        printf("\n");
    }

    printf("\n");
    printf("   Disk    ");

    Event *currentEvent = graph->disk->firstEvent;
    if (currentEvent)
    {
        // print spaces until the first event
        int t = 0;
        for (; t < currentEvent->time && t < graph->nbTimeSlots; t++)
        {
            printf(" ");
        }

        // print the events
        for (; t < graph->nbTimeSlots; t++)
        {
            // while loop in case multiple events occur at the same time
            // (for example if a process is immediately RUNNING when set READY
            // and there is no switch time)
            while (currentEvent->nextEvent && t == currentEvent->nextEvent->time)
            {
                currentEvent = currentEvent->nextEvent;
            }

            printf("%c", currentEvent->symbol);
        }
    }

    printf("\n\nX = RUNNING on core X   . = WAITING (IO)   - = READY\n");
}


/* ---------------------------- static functions --------------------------- */

static Event *createNewEventForGraph(ProcessGraph *graph, int time)
{
    if (time >= MAX_TIME_SLOTS)
    {
        fprintf(stderr, "Error: Maximum number of time slots reached\n");
        return NULL;
    }

    graph->nbTimeSlots = (time + 1 > graph->nbTimeSlots) ? time + 1 : graph->nbTimeSlots;

    Event *event = (Event *) malloc(sizeof(Event));
    if (!event)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }

    event->time = time;
    event->nextEvent = NULL;
    event->symbol = ' ';

    return event;
}

static char idToSymbol(int id)
{
    id %= 62;
    if (id < 10)
    {
        return '0' + id;
    }
    else if (id < 36)
    {
        return 'A' + id - 10;
    }
    else
    {
        return 'a' + id - 36;
    }
}
