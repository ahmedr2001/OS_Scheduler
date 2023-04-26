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
void handler(int s);
void HPF();
int shmid;
int size;
int n_algo;
int slice = 0;
int starting_the_slice;
int size = 0;
int n_processes = 0;
mqd_t msgq_id;
mqd_t msg_id_processes;
struct PCB *pcb;
struct NodePCB *info_processs;
struct Queue *ready;
struct process_message mes_rec;
struct process rec;
struct process current;
bool running = 0;
int temp_finishtime = 0;
//==========================
void write_the_state()
{
    FILE *logfile;
    logfile = fopen("scheduler.log", "a");
    if (logfile == NULL)
    {
        printf("Error in openning scheduler.log file: \n");
        return;
    }
    char *x;
    switch (info_processs->status)
    {
    case 0:
        x = "stopped";
        break;
    case 1:
        x = "started";
        break;
    case 2:
        x = "finished";
        break;
    case 3:
        x = "resumed";
        break;
    case 4:
        x = "started";
        break;
    default:
        x = "unknown";
        break;
    }
    if (info_processs->status != 2) // finished
    {
        fprintf(logfile, "At time %d process %d %s arr %d total %d remain %d wait %d", getClk(),
                info_processs->process.id, x, info_processs->process.arrivaltime,
                info_processs->process.runningtime, info_processs->remaining_time, info_processs->waiting_time);
    }
    else
    {
        fprintf(logfile, "At time %d process %d %s arr %d total %d remain %d wait %d TA %d WA %f", getClk(),
                info_processs->process.id, x, info_processs->process.arrivaltime,
                info_processs->process.runningtime, info_processs->remaining_time, info_processs->waiting_time,
                info_processs->turnaround_time, (info_processs->turnaround_time / (double)info_processs->process.runningtime) * 100 / 100.0f);
    }
    fprintf(logfile, "\n");
    fclose(logfile);
    return;
}
//==========================
int main(int argc, char *argv[])
{
    initClk();
    signal(SIGINT, handler);
    signal(SIGUSR2, handler);
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
    while (size < n_processes)
    {
        if (msgrcv(msgq_id, &mes_rec, sizeof(mes_rec.process), 0, !IPC_NOWAIT) == -1)
        {
            perror("Error in rec:");
        }
        else
        {
            switch (n_algo)
            {
            case 1:
                insertPrioQueue(ready, &mes_rec.process, mes_rec.process.priority);
                HPF();
                break;
            case 2:
                insertPrioQueue(ready, &mes_rec.process, mes_rec.process.runningtime);
                break;
            case 3:
                enqueue(ready, &mes_rec.process);
                printf("hih \n");
                RR();
                break;
            default:
                printf("Error in n_algo \n");
                break;
            }
            size++;
        }
    }
    printPCB(pcb);
    // sleep(2);
    destroyClk(true);
    // kill(getppid(), SIGINT);
    return 0;
}
void handler(int signum)
{
    switch (signum)
    {
    case SIGINT:
        printf("Cleearing the Resources from scheduler. \n");
        msgctl(msgq_id, IPC_RMID, 0);
        shmctl(shmid, IPC_RMID, NULL);
        // Destroy the clock
        // destroyClk(true);
        // Exit the program
        signal(SIGINT, handler);
        exit(signum);
        break;
    case SIGUSR2:

        running = false;
        info_processs->finish_time = getClk();
        info_processs->remaining_time = 0;
        info_processs->status = 2;
        info_processs->turnaround_time = info_processs->finish_time - info_processs->process.arrivaltime;
        update_node_PCB(pcb, info_processs);
        signal(SIGUSR2, handler);
        break;
    case SIGUSR1:
        break;
    }
}

void HPF()
{
    while (!isQueueEmpty(ready))
    {
        if (!running)
        {
            info_processs = creatNodePCB();
            current = dequeue(ready);
            info_processs->process = current;
            info_processs->starting_time = getClk();
            info_processs->status = 1;
            info_processs->spri = -1;
            info_processs->waiting_time = getClk() - info_processs->process.arrivaltime;
            info_processs->remaining_time = current.runningtime;
            info_processs->finish_time = -1;
            insertPCB(pcb, info_processs);
            write_the_state();
            while (getClk() < (current.runningtime + info_processs->starting_time))
            {
                if (msgrcv(msgq_id, &mes_rec, sizeof(mes_rec.process), 0, IPC_NOWAIT) != -1)
                {
                    insertPrioQueue(ready, &mes_rec.process, mes_rec.process.priority);
                    size++;
                }

                running = true;
                pid_t ff = fork();
                if (ff == -1)
                {
                    perror("error in forking \n");
                }
                if (ff == 0)
                {
                    char sremaing[10];
                    sprintf(sremaing, "%d", current.runningtime);
                    execl("./process.out", "./process.out", sremaing, NULL);
                    exit(0);
                }
                info_processs->parentIDRR = ff; // not necessary
            }
            write_the_state();
        }
    }
    return;
}
void RR()
{
    pid_t ff;
    while (!isQueueEmpty(ready))
    {
            current = dequeue(ready);
            info_processs = creatNodePCB();
            info_processs->process = current;
            info_processs->remaining_time = info_processs->process.runningtime;
            info_processs->starting_time = -1;
            info_processs->spri = -1;
            info_processs->status = 0;
            info_processs->finish_time = -1;
            info_processs->waiting_time = -1;
            info_processs->turnaround_time = -1;
            insertPCB(pcb, info_processs);
            printf("dequeue from ready : %d at: %d \n",info_processs->process.id,getClk());
        if (msgrcv(msgq_id, &mes_rec, sizeof(mes_rec.process), 0, IPC_NOWAIT) != -1)
        {
            printf("enqueue : %d at: %d \n",mes_rec.process.id,getClk());
            enqueue(ready, &mes_rec.process);
            size++;
        }
        if (running && (getClk() - starting_the_slice >= slice) && info_processs->remaining_time > slice)
        {
            running = 0;
            kill(ff, SIGSTOP);
            info_processs->status = 0;
            info_processs->remaining_time -= slice;
            enqueue(ready, &info_processs->process);
            update_node_PCB(pcb, info_processs);
            write_the_state();
        }
        if (!running)
        {
            running = 1;
            //current = dequeue(ready);
            info_processs = dequeuePCB(pcb, current);
            if (info_processs->starting_time < 0)
            {
                info_processs->starting_time = getClk();
                info_processs->status = 4;
                info_processs->waiting_time = getClk() - info_processs->process.arrivaltime;
                insertPCB(pcb,info_processs);
                //update_node_PCB(pcb, info_processs);
            }
            else
            {
                info_processs->status = 3;
                update_node_PCB(pcb, info_processs);
            }
            write_the_state();
            if (info_processs->status == 4)
            {
                ff = fork();
                if (ff == -1)
                {
                    perror("Error in forking.\n");
                    exit(-1);
                }
                if (ff == 0)
                {
                    char sremaing[10];
                    sprintf(sremaing, "%d", info_processs->remaining_time);
                    execl("./process.out", "./process.out", sremaing, NULL);
                    exit(0);
                }
                starting_the_slice = getClk();
                info_processs->parentIDRR = ff;
            }
            else
            {
                starting_the_slice = getClk();
                kill(info_processs->parentIDRR,SIGCONT);
            }
        }
    }
    return;
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

