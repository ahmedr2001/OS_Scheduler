#ifndef _PCB_H
#define _PCB_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "types.h"

struct NodePCB *creatNodePCB()
{
    struct NodePCB * n = (struct NodePCB *)malloc(sizeof(NodePCB));
    n->parentIDRR = -1;
    n->Next = NULL;
    n->finish_time = -1;
    //n->process = NULL;
    n->remaining_time = -1;
    n->spri = -1;
    n->starting_time = -1;
    n->status = -1;
    n->turnaround_time = -1;
    n->waiting_time = -1;
    return n;
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
int insertedBefore(struct PCB *B,struct process s)
{
    NodePCB * temp = B->Front;
    while (temp)
    {
        if(temp->process.id == s.id)
        {
            return 1;
        }
        temp =temp->Next;
    }
    return 0;
}
struct NodePCB * dequeuePCB(struct PCB *B,struct process s)
{
    if(isQueueEmptyPCB(B))
    {
        return NULL;
    }
    if(B->count == 1)
    {
        if(B->Front->process.id == s.id)
        {
            B->count = 0;
            NodePCB* temp = B->Front;
            return temp;
            B->Front =NULL;
        }
    }
    NodePCB* temp = B->Front->Next;
    NodePCB* prev = B->Front;
    while (temp)
    {
        if(temp->process.id == s.id)
        {
            prev->Next = temp->Next;
            B->count -=1;
            return temp;
        }
        temp = temp->Next;
        prev = prev->Next;
    }
    return NULL;
}
void insertPCB(struct PCB *B, struct NodePCB *p)
{
    NodePCB * n = creatNodePCB();
    if (!p)
    {
        return;
    }
    n->finish_time = p->finish_time;
    n->parentIDRR = p->parentIDRR;
    n->process = p->process;
    n->remaining_time = p->remaining_time;
    n->spri = p->spri;
    n->starting_time = p->starting_time;
    n->status = p->status;
    n->turnaround_time = p->turnaround_time;
    n->waiting_time = p->waiting_time; 
    if (isQueueEmptyPCB(B) == 1)
    {
        p->Next = NULL;
        B->Front = B->Rear = n;
        B->count++;
        return;
    }
    struct NodePCB*temp;
    temp = B->Front;
     while (temp)
    {
        if (temp->process.id == p->process.id)
        {
            return;
        }
        temp = temp->Next;
    }
    free(temp);
    B->Rear->Next = n;
    B->Rear = n;
    B->count++;
    //printf("inserting success \n");
    return;
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
        printf("process number:%d,arrived: %d,started: %d, finished:%d, waited: %d, remaining: %d, Turnaround:%d \n",temp->process.id,temp->process.arrivaltime,temp->starting_time,temp->finish_time,temp->waiting_time,temp->remaining_time,temp->turnaround_time);
        temp = temp->Next;
    }
    return;
}
#endif