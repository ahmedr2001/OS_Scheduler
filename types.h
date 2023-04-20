#ifndef _TYPES_H
#define _TYPES_H
typedef struct process
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
}process;
typedef struct process_message
{
    struct process process;
    long mtype;
}process_message;
#endif