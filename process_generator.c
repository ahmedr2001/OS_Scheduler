#include "headers.h"
#include "Queue.h"
#include "types.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <mqueue.h>
#include <string.h>
char line[100];

int shmid;
void clearResources(int);
mqd_t msgq_id;
int main(int argc, char *argv[])
{
    int n_of_algo = 1;
    int slice = 2;
    struct Queue *Q = createQueue();

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
            struct process temp;
            printf("%s \n", line);
            sscanf(line, "%d %d %d %d", &temp.id, &temp.arrivaltime, &temp.runningtime, &temp.priority);
            enqueue(Q, &temp);
        }
    }
    fclose(input_file);
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files. DONE
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any. DONE
    // 3. Initiate and create the scheduler and clock processes. DONE
    // 4. Use this function after creating the clock process to initialize clock. DONE
    //----------------------------- make the message queue---------------
    msgq_id = msgget(8, 0666 | IPC_CREAT);
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
    sprintf(string_slice, "%d", slice);

    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error in fork!");
        exit(-1);
    }
    else if (pid == 0)
    {
        execl("./clk.out", "./clk.out", NULL); // execute the clock process
        exit(0);
    }
    pid_t pid1 = fork();
    if (pid1 == -1)
    {
        perror("Error in fork!");
        exit(-1);
    }
    if (pid1 == 0)
    {

        execl("./scheduler.out", "./scheduler.out", string_algo, string_slice, NULL);
        exit(0);
    }
    initClk();
    signal(SIGINT, clearResources); // in case of CTRL + c interrupt
                                    // TODO Generation Main Loop
                                    // 6. Create a data structure for processes and provide it with its parameters. DONE
                                    // 7. Send the information to the scheduler at the appropriate time. DONE
                                    // 8. Clear clock resources
    int x = getClk();
    printf("current time is %d\n", x);
    while (!isQueueEmpty(Q))
    {
        while(getClk() >= Q->Front->process.arrivaltime)
        {
            printf("xix ");
            struct process tempo;
            tempo = dequeue(Q);
            struct process_message wal;
            wal.process = tempo;
            wal.mtype = 1;
            int check = msgsnd(msgq_id, &wal, sizeof(wal.process), IPC_NOWAIT);
            if (check == -1)
            {
                perror("ERROR in sending \n");
            }
        }
    }

    while (1)
    {
        sleep(1);
    }
    

    destroyClk(true);
    return 0;
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    // destroyClk(1);
    printf("Cleearing the Resources. \n");
    msgctl(msgq_id,IPC_RMID,0);
    // Destroy the clock
    // destroyClk(true);
    // Exit the program
    exit(signum);
}
