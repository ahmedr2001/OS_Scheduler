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
int shmid;
int size;
int n_algo;
int slice=0;
mqd_t msgq_id;
int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    initClk();
    struct Queue *ready = createQueue();
    n_algo = atoi(argv[1]);
    if(n_algo == 3)
    {
        slice = atoi(argv[2]);
    }
   msgq_id = msgget(7, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in openning message queue.");
        exit(-1);
    }
    struct process_message mes_rec;
    int x=0;
    while (1)
    {
        struct process rec;
        if(msgrcv(msgq_id, &mes_rec,sizeof(mes_rec.process), 1, IPC_NOWAIT) == -1)
        {
            perror("ERROR in reciever: ");
        }
        else
        {
            printf("%d",mes_rec.process.id);
            break;
        }
    }
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    //destroyClk(true);
    printf("Breaked!! \n");
    return 0;
}
void clearResources(int signum)
{
    printf("Cleearing the Resources. \n");
    msgctl(msgq_id,IPC_RMID,0);
    // Destroy the clock
    //destroyClk(true);
    // Exit the program
    exit(signum);
}
// int HPF(int time)
// {
//     int scheduledProcessIndex = -1;
//     bool occupied = 0;
//     int max_priority = 11;
//     for (int i = 0; i < size; i++) {
//         if (data[i].id != -1) {
//             int st_time = data[i].startingTime;
//             int end_time = st_time + data[i].runningtime;
//             if (st_time <= time && time <= end_time) {   // Non-preemptive algorithm
//                 occupied = 1;
//                 scheduledProcessIndex = i;
//                 break;
//             }
//             // Schedule the highest priority process (lowest value)
//             if (data[i].priority < max_priority) {  
//                 max_priority = data[i].priority;
//                 scheduledProcessIndex = i;
//             }
//         }
//     }
//     if (!occupied && scheduledProcessIndex != -1) {
//         data[scheduledProcessIndex].startingTime = time;
        
//         data[scheduledProcessIndex].finishtime = time 
//             + data[scheduledProcessIndex].runningtime;
//     }
//     return scheduledProcessIndex;
// }

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
