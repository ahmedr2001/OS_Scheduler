#ifndef _PCB_H
#define _PCB_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "types.h"

struct NodePCB *creatNodePCB()
{
    return (struct NodePCB *)malloc(sizeof(NodePCB));
}
struct PCB *creatPCB()
{
    struct PCB *B;
    B = (struct PCB *)malloc(sizeof(PCB));
    B->Front = NULL;
    B->Rear = NULL;
    B->count = 0;
    return B;
}
int isQueueEmptyPCB(struct PCB *B)
{
    if (B->count == 0)
        return 1;
    return 0;
}
void insertPCB(struct PCB *B, struct NodePCB *p)
{
    // if(findID_PCB(B,p->process.id) == 0)
    // {
    //     return; // to be modified
    // }
    if (!p)
    {
        return;
    }
    if (isQueueEmptyPCB(B) == 1)
    {
        p->Next = NULL;
        B->Front = B->Rear = p;
        B->count++;
        return;
    }
    B->Rear->Next = p;
    B->Rear = p;
    B->count++;
    //printf("inserting success \n");
    return;
}
int findID_PCB(struct PCB *B, int id)
{
    struct NodePCB *temp;
    temp = B->Front;
    while (temp)
    {
        if (temp->process.id == id)
        {
            return 1;
        }
        temp = temp->Next;
    }
    return 0;
}
void update_node_PCB(struct PCB *B, struct NodePCB *updateit)
{
    if(isQueueEmptyPCB(B))
    {
        return;
    }
    struct NodePCB *temp;
    temp = B->Front;
    while (temp)
    {
        if (temp->process.id == updateit->process.id)
        {
            break;
        }
        temp = temp->Next;
    }
    if(temp)
    {
        temp = updateit;
        //printf("updating success\n");
    }
}
void printPCB(struct PCB *B)
{
    if(B->count == 0)
    {
        printf("Empty PCB.\n");
        return;
    }
    struct NodePCB *temp;
    temp = B->Front;
    while (temp)
    {    
        printf("process number:%d,arrived: %d,started: %d, finished:%d, waited: %d, Turnaround:%d \n",temp->process.id,temp->process.arrivaltime,temp->starting_time,temp->finish_time,temp->waiting_time,temp->turnaround_time);
        temp = temp->Next;
    }
    return;
}
#endif