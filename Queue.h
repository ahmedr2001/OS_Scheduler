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
    // if(q->count == 0)
    // {
    //     temp = q->Rear;
    //     free(temp);
    // }
    return tempProcess;
}
struct Node* findID(struct Queue *q,int id)
{
    struct Node *temp;
    temp = q->Front;
    while (temp)
    {
        if(temp->process.id == id)
        {
            return temp;
        }
        temp = temp->Next;
    }
    return NULL;
}
void printQ(struct Queue *q)
{
    struct Node *temp;
    temp = q->Front;
    if(!temp)
    {
        printf("Empty Queue.\n");
        return;
    }
    int x = 0;
      while (temp)
    {
        x++;
        printf("id %d : %d \n",x,temp->process.id);
        temp = temp->Next;
    }
    return;
}
void swap(struct Node *a, struct Node *b) {
    struct process temp = a->process;
    a->process = b->process;
    b->process = temp;
}

struct Node* partition(struct Node *start, struct Node *end) {
    struct process *pivot = &end->process;
    struct Node *pIndex = start;

    for (struct Node *i = start; i != end; i = i->Next) {
        if (i->process.priority < pivot->priority) {
            swap(pIndex, i);
            pIndex = pIndex->Next;
        }
    }

    swap(pIndex, end);
    return pIndex;
}

void quickSort(struct Node *start, struct Node *end) {
    if (start == end || start == NULL || end == NULL)
        return;

    struct Node *pIndex = partition(start, end);
    quickSort(start, pIndex);
    quickSort(pIndex->Next, end);
}

void sortQueueByPriorityQuick(struct Queue *q) {
    if (q == NULL || q->Front == NULL || q->Rear == NULL || q->Front == q->Rear)
        return;

    quickSort(q->Front, q->Rear);
}

void sortQueueByPriorityBubble(struct Queue *q) {
    int i, j, size = q->count;
    struct Node *temp1, *temp2;
    struct process tempProcess;
    for (i = 0; i < size - 1; i++) {
        temp1 = q->Front;
        for (j = 0; j < size - i - 1; j++) {
            temp2 = temp1->Next;
            if (temp1->process.priority > temp2->process.priority) {
                tempProcess = temp1->process;
                temp1->process = temp2->process;
                temp2->process = tempProcess;
            }
            temp1 = temp1->Next;
        }
    }
}
void insertPrioQueue(struct Queue *q, struct process *p) {
    struct Node *newNode = createNode();
    newNode->process = *p;
    newNode->Next = NULL;

    if (isQueueEmpty(q)) {
        q->Front = q->Rear = newNode;
        q->count++;
        return;
    }

    if (p->priority > q->Front->process.priority) {
        newNode->Next = q->Front;
        q->Front = newNode;
        q->count++;
        return;
    }

    struct Node *current = q->Front;
    struct Node *prev = NULL;

    // binary search for the position to insert the new node
    int low = 0;
    int high = q->count - 1;
    int mid = (low + high) / 2;
    while (low <= high) {
        for (int i = 0; i < mid; i++) {
            current = current->Next;
        }
        if (p->priority > current->process.priority) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
        prev = current;
        current = q->Front;
        mid = (low + high) / 2;
    }

    // insert the new node
    newNode->Next = prev->Next;
    prev->Next = newNode;
    if (newNode->Next == NULL) {
        q->Rear = newNode;
    }
    q->count++;
}

#endif