#ifndef schedulingAlgorithms_h
#define schedulingAlgorithms_h

#include <stdbool.h>

#define NO_LIMIT -1

/**
 * @brief Definition of the SchedulingAlgorithm struct.
 * 
 * This struct represents a scheduling algorithm.
 * It is used to define the properties and behavior of a specific scheduling algorithm.
 */
typedef struct SchedulingAlgorithm_t SchedulingAlgorithm;
typedef enum
{
    RR,
    FCFS,
    SJF,
    PRIORITY
} SchedulingAlgorithmType;

struct SchedulingAlgorithm_t
{
    SchedulingAlgorithmType type;
    
    // NO_LIMIT means there is no limit to the timer -> the timer is not used
    int RRSliceLimit;
    int executiontTimeLimit;
    int ageLimit;
};

/**
 * Initialize a new SchedulingAlgorithm with the given type and limits.
 *
 * @param type: the type of the algorithm
 * @param RRSliceLimit: the time slice for the Round Robin algorithm
 * @param executiontTimeLimit: the time limit for the execution of a process
 * @param ageLimit: the time limit for the aging of a process
 *
 * @return a pointer to the new SchedulingAlgorithm
 */
SchedulingAlgorithm *initSchedulingAlgorithm(SchedulingAlgorithmType type, int RRSliceLimit, int executiontTimeLimit, int ageLimit);

/**
 * Free the memory allocated for the SchedulingAlgorithm.
 *
 * @param algorithm: the SchedulingAlgorithm to free
 */
void freeSchedulingAlgorithm(SchedulingAlgorithm *algorithm);

/**
 * Set the type of the SchedulingAlgorithm.
 *
 * @param algorithm: the SchedulingAlgorithm to modify
 * @param type: the new type of the algorithm (e.g. "RR", "FCFS", "SJF", "PRIORITY")
 */
void setAlgorithmType(SchedulingAlgorithm *algorithm, const char *type);

#endif // schedulingAlgorithms_h
