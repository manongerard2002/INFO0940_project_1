#ifndef process_h
#define process_h

typedef enum
{
    // NEW, // Only for consistency with the course, not used in this project
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct PCB_t  PCB;

struct PCB_t
{
    int pid;
    ProcessState state;
    int priority;

    // stats are generally also part of the PCB, but we are using a separate
    // struct
};

#endif // process_h
