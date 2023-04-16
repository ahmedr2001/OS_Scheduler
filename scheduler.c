#include "headers.h"
#include "Queue.h"
#include"types.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include<mqueue.h>
#include<string.h>
int size;
int main(int argc, char * argv[])
{
    struct Queue *ready = createQueue();
    int n_algo = atoi(argv[1]);
    int slice = 0;
    if(n_algo == 3)
    {
        slice = atoi(argv[2]);
    }
    mqd_t mq = msgget(8, 0666 | IPC_CREAT);
    struct process rec;
    struct process_message mes_rec;
    while (1)
    {
        if(msgrcv(mq, &mes_rec,sizeof(rec), 0, !IPC_NOWAIT)==-1)
        {
            perror("Error in receive ");
        }
        else
        {
            printf("%d",mes_rec.process.id);
            break;
        }
    }
    
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    
    destroyClk(true);
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
