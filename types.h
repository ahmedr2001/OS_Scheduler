#ifndef _TYPES_H
#define _TYPES_H
typedef struct process
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
}process;
typedef struct PCB
{
    struct process process;
    int status; //0 for waiting, 1 for running
    int waiting_time;
    int remaining_time;
}PCB;
typedef struct process_message
{
    struct process process;
    long mtype;
}process_message;
#endif