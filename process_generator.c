#include "headers.h" // the other structs are inside

// var
int Algo;
int time_quantum = 0;
int count = 0;
int pGeneratorToScheduler;
// for input thing
int a, b, c, d;
struct PCB processArr[3];
// methods
void clearResources(int);
void MDR(int);
struct PriorityQueue que;
struct PCB processToBeSent;
void get_num_algo();
void Start_the_CLK_and_scheduler();
void read_data_from_file();
void IPC(struct PCB processToBeSent);
struct PriorityQueue sendingQueue;
int schedularID;
int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    signal(SIGALRM, MDR);
    // TODO Initialization
    // 1. Read the input files.
    read_data_from_file();
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    get_num_algo();
    // 3. Initiate and create the scheduler and clock processes.
    struct PriorityQueue newQue;
    initializeQueue(&newQue);
    Start_the_CLK_and_scheduler();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    int x = getClk();
    int i = 0;
    while (que.head != NULL)
    { // should loop on the input queue
        int y = getClk();
        processToBeSent = que.head->pcb;
        if (processToBeSent.ArrTime <= y - x)
        {
            DeQueue(&que, &processToBeSent);
            IPC(processToBeSent);
            if (Algo == 2)
            {
                kill(schedularID, SIGALRM);
                // The purpose of this line is to notify the scheduler process
                // that a new process has arrived and needs to be added to the scheduling queue
            }
            if (que.head)
            {
                int timeToWait = que.head->pcb.ArrTime - processToBeSent.ArrTime;
                if (timeToWait != 0)
                {
                    alarm(timeToWait);
                    pause(); // Suspend the process until a signal arrives.
                }
            }
        }
    }

    pause();
    return 0;
}

void read_data_from_file()
{
    int id;
    int arrivalTime;
    int runningTime;
    int priority;
    FILE *process = fopen("input test.txt", "r");
    if (process == NULL)
    {
        printf("Error! File cannot be opened.");
        exit(1);
    }
    char ignoredCharacter[100];
    while (fscanf(process, "%s", ignoredCharacter) == 1)
    {
        if (*ignoredCharacter == '#')
        {
            fgets(ignoredCharacter, sizeof(ignoredCharacter), process);
            continue;
        }
        else
        {
            id = atoi(ignoredCharacter);
            if (fscanf(process, "%d %d %d", &arrivalTime, &runningTime, &priority) != 3)
            {
                printf("Error reading input file\n");
                exit(1);
            }
            setPCB(&processToBeSent, id, arrivalTime, runningTime, priority);
            // printf("id %d, Arr time %d, Running time %d, Priority %d \n", processToBeSent.id, processToBeSent.ArrTime, processToBeSent.RunTime, processToBeSent.Priority);
            AddAccordingToArrivalTime(&que, processToBeSent);
            count++;
        }
    }

fclose(process);
}
void IPC(struct PCB processToBeSent)
{
    pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
    if (pGeneratorToScheduler == -1)
    {
        perror("error in creat");
        exit(-1);
    }
    struct msgBuff processInfo;
    processInfo.mtype = 10;
    CopyPCB(&processInfo.process, processToBeSent);
    int val = msgsnd(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), IPC_NOWAIT);
    if (val == -1)
        printf("Error in send process\n");
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    msgctl(pGeneratorToScheduler, IPC_RMID, (struct msqid_ds *)0);
    destroyClk(true);
    exit(0);
}
void MDR(int x)
{
    // printf("hi\n");
}

void get_num_algo()
{
    printf("Choose the prefered Algorithm....\n");
    printf("1) HPF. 2) SRTN. 3) RR. \n");
    scanf("%d", &Algo);

    while (!(Algo == 1 || Algo == 2 || Algo == 3))
    {
        printf("Choose A valid Number.\n");
        scanf("%d", &Algo);
    }
    if (Algo == 3)
    {
        printf("Enter the slice time:\n");
        scanf("%d", &time_quantum);
    }
}

void Start_the_CLK_and_scheduler()
{
    int pid;
    for (int i = 0; i < 2; i++)
    {
        pid = fork();
        if (pid == 0)
        {
            if (i == 0)
            {
                printf("CLK forking...\n");
                char *argv[] = {"./clk.out", NULL};
                execv(argv[0], argv);
            }
            else
            {
                printf("scheduler forking...\n");
                char cSendAlgo[10];
                char cSendTime_quantum[10];
                char parentID[10];
                char Pcount[10];
                sprintf(cSendAlgo, "%d", Algo);
                sprintf(cSendTime_quantum, "%d", time_quantum);
                sprintf(parentID, "%d", getppid());
                sprintf(Pcount, "%d", count);
                char *argv[] = {"./scheduler.out", cSendAlgo, cSendTime_quantum, parentID, Pcount, 0};
                execv(argv[0], argv);
            }
            // pray for not reaching here
            printf("Failed to execute child process\n");
            exit(1);
        }
        else if (pid != 0 && i == 1)
        {
            schedularID = pid;
        }
    }
}
