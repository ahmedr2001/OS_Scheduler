#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include"PCB.h"

struct PCBNode{
    struct PCB pcb;
    struct PCBNode* next;
};

struct PriorityQueue{
    struct PCBNode* head;
};

struct PCBNode createNode(struct PCB pcb)
{
    struct PCBNode New;
    New.pcb = pcb;
    New.next = NULL;
    return New;
}

void initializeQueue(struct PriorityQueue* que)
{
    que->head = NULL;
}

void enQueue(struct PriorityQueue* que, struct PCBNode* newNode)
{
    if(que->head == NULL)
    {
        que->head = newNode;
    }
    else
    {
        struct PCBNode* next = que->head;
        while(next->next != NULL)
        {
            next = next->next;
        }
        next->next = newNode;
    }
}

void InsertAccordingToPriority(struct PriorityQueue* que, struct PCBNode* newNode)
{
    if(que->head == NULL)
    {
        que->head = newNode;
    }
    else
    {
        if(que->head->pcb.Priority > newNode->pcb.Priority)
        {
            newNode->next = que->head;
            que->head = newNode;
        }
        else
        {
            struct PCBNode* next = que->head;
            while(next->next != NULL && next->next->pcb.Priority < newNode->pcb.Priority)
            {
                next = next->next;
            }
            if(next->next != NULL)
            {
                newNode->next = next->next;
                next->next = newNode;
            }
            else
            {
                next->next = newNode;
            }
        }
    }
}

void InsertAccordingToArrivalTime(struct PriorityQueue* que, struct PCBNode* newNode)
{
    if(que->head == NULL)
    {
        que->head = newNode;
    }
    else
    {
        if(que->head->pcb.ArrTime > newNode->pcb.ArrTime)
        {
            newNode->next = que->head;
            que->head = newNode;
        }
        else
        {
            struct PCBNode* next = que->head;
            while(next->next != NULL && next->next->pcb.ArrTime <= newNode->pcb.ArrTime)
            {
                next = next->next;
            }
            if(next->next != NULL)
            {
                newNode->next = next->next;
                next->next = newNode;
            }
            else
            {
                next->next = newNode;
            }
        }
    }
}

void InsertAccordingToInverseArrivalTime(struct PriorityQueue* que, struct PCBNode* newNode)
{
    if(que->head == NULL)
    {
        que->head = newNode;
    }
    else
    {
        if(1/(que->head->pcb.ArrTime) > 1/(newNode->pcb.ArrTime))
        {
            newNode->next = que->head;
            que->head = newNode;
        }
        else
        {
            struct PCBNode* next = que->head;
            while(next->next != NULL && 1/(next->next->pcb.ArrTime) <= 1/(newNode->pcb.ArrTime))
            {
                next = next->next;
            }
            if(next->next != NULL)
            {
                newNode->next = next->next;
                next->next = newNode;
            }
            else
            {
                next->next = newNode;
            }
        }
    }
}

void InsertAccordingToReaminingTime(struct PriorityQueue* que, struct PCBNode* newNode)
{
    if(que->head == NULL)
    {
        que->head = newNode;
    }
    else
    {
        if(que->head->pcb.RunTime > newNode->pcb.RunTime)
        {
            newNode->next = que->head;
            que->head = newNode;
        }
        else
        {
            struct PCBNode* next = que->head;
            while(next->next != NULL && next->next->pcb.RunTime < newNode->pcb.RunTime)
            {
                next = next->next;
            }
            if(next->next != NULL)
            {
                newNode->next = next->next;
                next->next = newNode;
            }
            else
            {
                next->next = newNode;
            }
        }
    }
}

void DeQueue(struct PriorityQueue* que, struct PCB* tosetPCB)
{
    if(que->head != NULL)
    {
        struct PCBNode* node = que->head;
        que->head = que->head->next;
        CopyPCB(tosetPCB, node->pcb);
       // free(node);
        node = NULL;
    }
    else
    {
        printf("null state\n");
        tosetPCB = NULL;
    }
}

int countNodes(struct PriorityQueue* a)
{
    int iter = 0;
    struct PCBNode* n;
    n = a->head;
    while(n!= NULL)
    {
        iter = iter + 1;
        n = n->next;
    }
    return iter;
}

void Add(struct PriorityQueue* a, struct PCB pcb)
{
    struct PCBNode* node = (struct PCBNode*)malloc(sizeof(struct PCBNode));
    node->pcb = pcb;
    enQueue(a,node);
}

void AddAccordingToArrivalTime(struct PriorityQueue* a, struct PCB pcb)
{
    struct PCBNode* node = (struct PCBNode*)malloc(sizeof(struct PCBNode));
    node->pcb = pcb;
    InsertAccordingToArrivalTime(a,node);
}
void AddAccordingToInverseArrivalTime(struct PriorityQueue* a, struct PCB pcb)
{
    struct PCBNode* node = (struct PCBNode*)malloc(sizeof(struct PCBNode));
    node->pcb = pcb;
    InsertAccordingToInverseArrivalTime(a,node);
}

void AddAccordingToRemainingTime(struct PriorityQueue* a, struct PCB pcb)
{
    struct PCBNode* node = (struct PCBNode*)malloc(sizeof(struct PCBNode));
    node->pcb = pcb;
    InsertAccordingToReaminingTime(a,node); 
}

void AddAccordingToPriority(struct PriorityQueue* a, struct PCB pcb)
{
    struct PCBNode* node = (struct PCBNode*)malloc(sizeof(struct PCBNode));
    node->pcb = pcb;
    InsertAccordingToPriority(a,node);
}