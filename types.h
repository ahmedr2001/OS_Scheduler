#ifndef _TYPES_H
#define _TYPES_H
#include"string.h"
typedef struct process
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
} process;
typedef struct NodePCB
{
    struct process process;
    int spri;
    int status; // 0 for waiting, 1 for running, 2 for finish
    int starting_time;
    int waiting_time;
    int remaining_time;
    int finish_time;
    int turnaround_time;
    struct NodePCB *Next;
} NodePCB;
typedef struct PCB
{
    int count;
    struct NodePCB *Front;
    struct NodePCB *Rear;
} PCB;
typedef struct process_message
{
    struct process process;
    long mtype;
} process_message;
#endif