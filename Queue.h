#ifndef _QUEUE_H
#define _QUEUE_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include"types.h"

typedef struct Node
{
    struct process process;
    struct Node *Next;
} Node;

typedef struct Queue
{
    struct Node *Front;
    struct Node *Rear;
    int count;
} Queue;

struct Queue *createQueue()
{
    struct Queue *q;
    q = (struct Queue *)malloc(sizeof(Queue));
    q->count = 0;
    q->Front = NULL;
    q->Rear = NULL;
    return q;
}
struct Node *createNode()
{
    return (struct Node *)malloc(sizeof(Node));
}

bool isQueueEmpty(struct Queue *q)
{
    if (q->count == 0)
        return 1;
    return 0;
}

void enqueue(struct Queue *q, struct process *p)
{

    struct Node *n = createNode();

    n->process = *p;

    if (q->count == 0)
    {
        q->Front = n;
        q->Rear = n;
    }
    else
    {
        q->Rear->Next = n;
        q->Rear = n;
    }
    q->count++;
}

struct process dequeue(struct Queue *q)
{

    struct Node *temp;
    struct process tempProcess;
    tempProcess = q->Front->process;

    temp = q->Front;
    q->Front = q->Front->Next;
    free(temp);
    q->count = q->count - 1;
    return tempProcess;
}
struct process* findID(struct Queue *q,int id)
{
    struct Node *temp;
    temp = q->Front;
    while (temp)
    {
        if(temp->process.id == id)
        {
            return &temp->process;
        }
        temp = temp->Next;
    }
    return NULL;
}
#endif