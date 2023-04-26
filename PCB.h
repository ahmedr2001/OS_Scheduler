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
    

}
void insertPCB(struct PCB *B, struct NodePCB *p)
{
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
    B->Rear->Next = p;
    B->Rear = p;
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
        printf("process number:%d,arrived: %d,started: %d, finished:%d, waited: %d, Turnaround:%d \n",temp->process.id,temp->process.arrivaltime,temp->starting_time,temp->finish_time,temp->waiting_time,temp->turnaround_time);
        temp = temp->Next;
    }
    return;
}
#endif