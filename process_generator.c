#include "headers.h"
#include "Queue.h"
#include "types.h"
#include"PCB.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
#include <sys/msg.h>
char line[100];
int shmid;
void clearResources(int);
mqd_t msgq_id;
int main(int argc, char *argv[])
{
    int n_of_algo = 1;
    int slice = 2;
    int n_processes = 0;
    struct Queue *Q = createQueue();
//--------------Reading processes from the FILE----------------------------
    FILE *input_file = fopen("input test.txt", "r");
    if (!input_file)
    {
        printf("Error opening file\n");
        return 1;
    }
    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        if (!(line[0] == '#'))
        { 
            n_processes++;
            struct process temp;
            printf("%s", line);
            sscanf(line, "%d %d %d %d", &temp.id, &temp.arrivaltime, &temp.runningtime, &temp.priority);
            enqueue(Q, &temp);
        }
    }
    fclose(input_file);
    printf("\n");
    //----------------------------- create the message queue---------------
    msgq_id = msgget(7, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in creating message queue.");
        exit(-1);
    }
    //  if (msgctl(msgq_id, IPC_RMID, NULL) == -1) {
    //     perror("msgctl");
    //     return 1;
    // }
    // return 0;
    //-----------------------------
    signal(SIGINT, clearResources);
    printf("Enter the number of the algo, 1 for HPF, 2 for SHRF, 3 for RR: \n");
    scanf("%d", &n_of_algo);
    printf("%d \n",n_of_algo);
    char string_algo[10];
    sprintf(string_algo, "%d", n_of_algo);
    if (n_of_algo == 3)
    {
        printf("Enter the Quantum: \n");
        scanf("%d", &slice);
    }
    char string_slice[10];
    char string_n_processes[10];
    sprintf(string_slice,"%d" ,slice);
    sprintf(string_n_processes,"%d" ,n_processes);
    pid_t pid = fork();
    if (pid == -1)
    {
       perror("Error in fork!");
       exit(-1);
    }
    else if (pid == 0)
    {
       execl("./clk.out", "./clk.out", NULL); // execute the clock process
    }
    initClk();
    int x = getClk();
    printf("current time is %d\n", x);
    pid_t pid1 = fork();
    if (pid1 == -1)
    {
       perror("Error in fork!");
       exit(-1);
    }
    if (pid1 == 0)
    {
       execl("./scheduler.out", "./scheduler.out", string_algo, string_slice,string_n_processes, NULL);
    }
    while (!isQueueEmpty(Q))
    {
        if(getClk() >= Q->Front->process.arrivaltime)
        {
            struct process tempo;
            tempo = dequeue(Q);
            struct process_message mes_send;
            mes_send.process = tempo;
            mes_send.mtype = 1;
            int check = msgsnd(msgq_id, &mes_send, sizeof(mes_send.process), !IPC_NOWAIT);
            if (check == -1)
            {
                perror("ERROR in sending ");
            }
        }
    }
    sleep(120);    
    //destroyClk(true);
    return 0;
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    printf("Cleearing the Resources FROM process generator. \n");
    msgctl(msgq_id,IPC_RMID,0);
    // Destroy the clock
    shmctl(shmid, IPC_RMID, NULL);
    destroyClk(true);
    exit(signum);
}
