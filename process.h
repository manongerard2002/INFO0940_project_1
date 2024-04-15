#ifndef process_h
#define process_h


// ProcessState is an enumeration that represents the different states a process can be in.
typedef enum
{
    // NEW, // Only for consistency with the course, not used in this project
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} ProcessState;

typedef struct PCB_t  PCB;

// PCB is a struct that represents a Process Control Block.
/**
 * @brief Definition of the PCB struct.
 * 
 * This struct represents a process control block.
 * It is used to define the properties and behavior of a specific process.
 */
struct PCB_t
{
    int pid;
    ProcessState state;
    int priority;

    // stats are generally also part of the PCB, but we are using a separate
    // struct
};

#endif // process_h
