#include "headers.h"
#include "Queue.h"
#include "types.h"
#include "PCB.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <mqueue.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
void clearResources_sched(int s);
void HPF();
int shmid;
int size;
int n_algo;
int slice = 0;
int size = 0;
int n_processes = 0;
mqd_t msgq_id;
mqd_t msg_id_processes;
struct PCB *pcb;
struct Queue *ready;
int main(int argc, char *argv[])
{
    initClk();
    signal(SIGINT, clearResources_sched);
    ready = createQueue();
    pcb = creatPCB();
    n_algo = atoi(argv[1]);
    if (n_algo == 3)
    {
        slice = atoi(argv[2]);
    }
    n_processes = atoi(argv[3]);
    printf("algorithm number: %d , slice: %d , n_processes: %d \n", n_algo, slice, n_processes);
    msgq_id = msgget(7, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in openning message queue.");
        exit(-1);
    }
    msg_id_processes = msgget(10, 0666 | IPC_CREAT);
    if (msg_id_processes == -1)
    {
        perror("Error in openning message queue (msg_id_processes).");
        exit(-1);
    }
    struct process_message mes_rec;
    int once = 1;
    while (size < n_processes)
    {
        struct process rec;
        if (msgrcv(msgq_id, &mes_rec, sizeof(mes_rec.process), 0, !IPC_NOWAIT) == -1)
        {
            perror("Error in rec:");
        }
        else
        {
            switch (n_algo)
            {
            case 1:
                printf("ID:%d,at:%d \n",mes_rec.process.id,getClk());
                insertPrioQueue(ready, &mes_rec.process, mes_rec.process.priority);
                printQ(ready);
                break;
            case 2:
                insertPrioQueue(ready, &mes_rec.process, mes_rec.process.runningtime);
                break;
            case 3:
                enqueue(ready, &mes_rec.process);
                break;
            default:
                printf("Error in n_algo \n");
                break;
            }
            size++;
        }
    }
    //printQ(ready);
    // HPF();
    // printPCB(pcb);
    sleep(2);
    destroyClk(true);
    return 0;
}
void clearResources_sched(int signum)
{
    printf("Cleearing the Resources from scheduler. \n");
    msgctl(msgq_id, IPC_RMID, 0);
    msgctl(msg_id_processes, IPC_RMID, 0);
    shmctl(shmid, IPC_RMID, NULL);
    // Destroy the clock
    // destroyClk(true);
    // Exit the program
    exit(signum);
}

void HPF()
{
    printPCB(pcb);
    printf("\n");
    struct process current;
    while (!isQueueEmpty(ready))
    {
        struct NodePCB *info_processs = creatNodePCB();
        current = dequeue(ready);
        info_processs->process = current;
        info_processs->starting_time = getClk();
        info_processs->status = 1;
        info_processs->spri = -1;
        info_processs->waiting_time = getClk() - info_processs->process.arrivaltime;
        info_processs->remaining_time = -1;
        info_processs->finish_time = -1;
        insertPCB(pcb,info_processs);
        while (getClk() < (current.runningtime + info_processs->starting_time))
        {
        }
        if(getClk() == (current.runningtime + info_processs->starting_time))
        {
            info_processs->finish_time = getClk();
        }
        update_node_PCB(pcb,info_processs);
    }
    return ;
}


// // Shortest Remaining Time Next Algorithm
// int SRTN(int time)
// {
//     int scheduledProcessIndex = -1;
//     int min_rt = 1e9;
//     for (int i = 0; i < size; i++) {
//         if (data[i].id != -1) {
//             if (data[i].remainingTime < min_rt) {
//                 min_rt = data[i].remainingTime;
//                 scheduledProcessIndex = i;
//             }
//         }
//     }

//     return scheduledProcessIndex;
// }

// // Round-Robin Algorithm
// int RR(int time, int previousProcessIndex)
// {
//     int scheduledProcessIndex = previousProcessIndex;
//     if (time % quantum == 0) {
//         int i = previousProcessIndex + 1;
//         i %= size;
//         while (true) {
//             if (data[i].id != -1) {
//                 break;
//             }
//             i++;
//             i %= size;
//         }

//         scheduledProcessIndex = i;
//     }

//     return scheduledProcessIndex;
// }
