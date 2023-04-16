#ifndef _TYPES_H
#define _TYPES_H
typedef struct process
{
    int arrivaltime;
    int priority;
    int runningtime;
    int startingTime;
    int finishTime;
    int remainingTime;
    int id;
}process;
typedef struct process_message
{
    struct process process;
    long mtype;
}process_message;
#endif