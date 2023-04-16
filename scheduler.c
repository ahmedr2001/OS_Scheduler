#include "headers.h"
#include "Queue.h"
#include"types.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include<mqueue.h>
#include<string.h>
void clearResources(int s);
int size;
int n_algo;
int slice=0;
mqd_t mq;
int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    struct Queue *ready = createQueue();
    n_algo = atoi(argv[1]);
    if(n_algo == 3)
    {
        slice = atoi(argv[2]);
    }
   mq = msgget(8, 0666 | IPC_CREAT);
    if (mq == -1)
    {
        perror("Error in openning message queue.");
        exit(-1);
    }
    struct process_message mes_rec;
    int x=0;
    sleep(2);
    while (x<1001)
    {
        struct process rec;
        if(msgrcv(mq, &mes_rec,sizeof(mes_rec.process), 1, IPC_NOWAIT) == -1)
        {
            perror("ERROR in reciever: ");
            //printf("sadasadad\n");
        }
        else
        {   rec = mes_rec.process;
            enqueue(ready,&rec);
            printf("%d",mes_rec.process.id);
        }
        x++;
    }
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    destroyClk(true);
    return 0;
}
void clearResources(int signum)
{
    printf("Cleearing the Resources. \n");
    msgctl(mq,IPC_RMID,0);
    // Destroy the clock
    //destroyClk(true);
    // Exit the program
    exit(signum);
}

struct process* HPF(int time, struct Queue *ready)
{
    struct process *scheduledProcess = NULL;
    int scheduledProcessID = -1;
    bool occupied = 0;
    int max_priority = 11;
    struct Node *root = ready->Front;
    while (root) {
        int st_time = root->process.startingTime;
        int end_time = st_time + root->process.runningtime;
        if (st_time <= time && time <= end_time) {   // Non-preemptive algorithm
            occupied = 1;
            scheduledProcessID = root->process.id;
            break;
        }
        // Schedule the highest priority process (lowest value)
        if (root->process.priority < max_priority) {  
            max_priority = root->process.priority;
            scheduledProcessID = root->process.id;
        }
        root = root->Next;
    }

    struct Node *processNode;
    processNode = findID(ready, scheduledProcessID);
    scheduledProcess = &processNode->process;
    if (!occupied && scheduledProcess) {
        scheduledProcess->startingTime = time;
        
        scheduledProcess->finishTime = time 
            + scheduledProcess->runningtime;
    }

    return scheduledProcess;
}

// Shortest Remaining Time Next Algorithm
struct process* SRTN(int time, struct Queue *ready)
{
    struct process *scheduledProcess = NULL;
    int scheduledProcessID = -1;
    int min_rt = 1e9;
    struct Node *root = ready->Front;
    while (root) {
        if (root->process.remainingTime < min_rt) {
            min_rt = root->process.remainingTime;
            scheduledProcessID = root->process.id;
        }
        root = root->Next;
    }

    struct Node *processNode;
    processNode = findID(ready, scheduledProcessID);
    scheduledProcess = &processNode->process;
    return scheduledProcess;
}

// Round-Robin Algorithm
struct process* RR(int time, int curProcessID, struct Queue *ready)
{
    struct process *scheduledProcess = NULL;
    struct Node *curProcessNode = findID(ready, curProcessID);
    struct Node *front = ready->Front;
    struct Node *rear = ready->Rear;
    rear->Next = front;
    if (time % slice == 0) {
        do {
            curProcessNode = curProcessNode->Next;
        } while (curProcessNode == NULL);
    }

    scheduledProcess = &curProcessNode->process;
    return scheduledProcess;
}
