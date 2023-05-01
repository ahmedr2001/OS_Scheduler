#include "headers.h"
#include "math.h"

#define hpf_Algo 1
#define strn_Algo 2
#define rr_Algo 3
char Algo;
int time_quantum = 1;
bool isRunning;
// algos
void HPF();
void SRTN();
void RR();
struct PCB IPC();
void Run(struct PCB *processToRun);
void handler1();
void MDR(int);
FILE *SchedulerLog;
FILE *SchedulerPerf;
int maxCount;
int parentID;
double *WTA;
double *Wait;
double *totalRun;
int endTime;
int startTime;
int main(int argc, char *argv[])
{
    // signal(SIGCHLD, SIG_IGN);
    signal(SIGALRM, MDR);
    Algo = atoi(argv[1]);
    if (Algo == 3)
        time_quantum = atoi(argv[2]);
    parentID = atoi(argv[3]);
    maxCount = atoi(argv[4]);
    WTA = calloc(maxCount, sizeof(double));
    Wait = calloc(maxCount, sizeof(double));
    totalRun = calloc(maxCount, sizeof(double));

    initClk();
    int x = getClk();

    SchedulerLog = fopen("scheduler.log", "w");
    switch (Algo)
    {
    case hpf_Algo:
        HPF();
        break;
    case strn_Algo:
        signal(SIGCHLD, handler1);
        SRTN();
        break;
    case rr_Algo:
        RR();
        break;
    }
    fclose(SchedulerLog);
    double avgWait = 0;
    double avgWTA = 0;
    double CPUperf = 0;
    double std = 0.0;
    for (int i = 0; i < maxCount; i++)
    {
        avgWait += Wait[i];
        avgWTA += WTA[i];
        CPUperf += totalRun[i];
    }

    avgWTA = avgWTA / maxCount;
    avgWait = avgWait / maxCount;
    for (int i = 0; i < maxCount; i++)
    {
        std += pow((WTA[i] - avgWTA), 2);
    }
    std = sqrt(std / maxCount);
    // printf("total run time = %f\n",CPUperf);
    // printf("Last clock equals %d\n", getClk());
    CPUperf = (CPUperf) / (endTime - startTime);

    SchedulerPerf = fopen("scheduler.perf", "w");
    if (Algo == 1)
        fprintf(SchedulerPerf, "# The running algorithm is : HPF\n");
    else if (Algo == 2)
        fprintf(SchedulerPerf, "# The running algorithm is : SRTN\n");
    else
        fprintf(SchedulerPerf, "# The running algorithm is : RR\n");
    fprintf(SchedulerPerf, "CPU utilization = %.2f%%\n", CPUperf * 100);
    fprintf(SchedulerPerf, "Avg WTA = %.2f\n", avgWTA);
    fprintf(SchedulerPerf, "Avg waiting = %.2f\n", avgWait);
    fprintf(SchedulerPerf, "Std WTA = %.2f\n", std);
    fclose(SchedulerPerf);
    destroyClk(true);
    return 0;
}

struct PCB tempProcess;
struct PCBNode processNode;
struct PCB schProcess;

int val;
int c = 0;
int pDone = 0;
void HPF()
{

    fprintf(SchedulerLog, "# The running algorithm is : HPF\n");
    int count = maxCount; /// should be the number of processes
    isRunning = false;
    schProcess.id = -1;
    struct PriorityQueue HPF_Ready;
    initializeQueue(&HPF_Ready);
    int x = getClk();
    int s = 0;
    while (1)
    {
        struct msgBuff processInfo;
        int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
        if (pGeneratorToScheduler == -1)
        {
            perror("error in creat\n");
            exit(-1);
        }
        {
            val = 1;
            while (val != -1)
            {
                val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 0, IPC_NOWAIT); // ...........
                if (val == -1)
                    break;
                CopyPCB(&tempProcess, processInfo.process);
                AddAccordingToPriority(&HPF_Ready, tempProcess);
            }
            //
            if (HPF_Ready.head != NULL)
            {
                DeQueue(&HPF_Ready, &schProcess);
                schProcess.startTime = getClk();
                IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", schProcess.startTime, schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RunTime, schProcess.WaitTime);
                if (s == 0)
                {
                    startTime = schProcess.startTime;
                    s++;
                }
                // Run(&schProcess);
                isRunning = true;
                int pid;
                pid = fork();
                if (pid == 0)
                {
                    // printf("process forking...\n");
                    char runTime[10];
                    sprintf(runTime, "%d", schProcess.RunTime);
                    char *process_arg_list[] = {"./process.out", runTime, 0};
                    // RunningProcessID = getpid();
                    execv(process_arg_list[0], process_arg_list);
                }
                else
                {
                    int status;
                    int y = wait(&status);
                    WIFEXITED(status);
                    schProcess.PID = pid;
                }
                fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                endTime = getClk();
                WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
                Wait[pDone] = schProcess.WaitTime;
                totalRun[pDone] = schProcess.RunTime;
                schProcess.state = Terminated;
                pDone++;
            }
        }

        if (pDone == maxCount)
        {
            // finishing 3ala 5er
            kill(getppid(), SIGTERM);
            break;
        }
    }
}
int RunningProcessID;
void Run(struct PCB *processToRun)
{
    if (processToRun->state == Stopped && Algo == 2)
    {
        // printf("A process is about to be running. \n");
        processToRun->state = Running;
        kill(processToRun->PID, SIGCONT);
        fprintf(SchedulerLog, "At time %d process %d resumed arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
        return;
    }
    int pid;
    pid = fork();
    if (pid == 0)
    {
        char runTime[10];
        sprintf(runTime, "%d", processToRun->RunTime);
        char *process_arg_list[] = {"./process.out", runTime, 0};
        RunningProcessID = getpid();
        execv(process_arg_list[0], process_arg_list);
    }
    else
    {

        processToRun->PID = pid;
        processToRun->state = Running;
    }
}
//aktar 7aga mo2refa
void RR()
{
    fprintf(SchedulerLog, "# The running algorithm is : RR\n");
    int count = maxCount; // the number of processes
    isRunning = false;

    struct PriorityQueue RR_Ready;
    initializeQueue(&RR_Ready);
    struct PriorityQueue Stopped_RR_Ready;
    initializeQueue(&Stopped_RR_Ready);
    int x = getClk();

    int s = 0;
    while (1)
    {
        struct msgBuff processInfo;
        int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
        if (pGeneratorToScheduler == -1)
        {
            perror("error in creat\n");
            exit(-1);
        }

        {

            int rc;
            struct msqid_ds buf;
            int num_messages;

            rc = msgctl(pGeneratorToScheduler, IPC_STAT, &buf);
            num_messages = buf.msg_qnum;

            for (int i = 0; i < num_messages; i++)
            {
                val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 0, !IPC_NOWAIT); // ...........
                CopyPCB(&tempProcess, processInfo.process);
                Add(&RR_Ready, tempProcess);
            }

            if (RR_Ready.head != NULL)
            {
                DeQueue(&RR_Ready, &schProcess);
                schProcess.startTime = getClk();
                IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                if (schProcess.RemainingTime > time_quantum)
                {

                    fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    if (s == 0)
                    {
                        startTime = schProcess.startTime;
                        s++;
                    }
                    Run(&schProcess);

                    alarm(time_quantum + getClk() - x);
                    pause();

                    kill(schProcess.PID, SIGSTOP);

                    schProcess.RemainingTime = schProcess.RemainingTime - time_quantum;

                    fprintf(SchedulerLog, "At time %d process %d stopped arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    schProcess.state = Stopped;
                    IncreaseWaitTime(&schProcess, -getClk());
                    Add(&Stopped_RR_Ready, schProcess);
                }
                else
                {
                    fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    if (s == 0)
                    {
                        startTime = schProcess.startTime;
                        s++;
                    }

                    Run(&schProcess);

                    int status;
                    int x;
                    x = wait(&status);
                    WIFEXITED(status);

                    fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                    endTime = getClk();
                    WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
                    Wait[pDone] = schProcess.WaitTime;
                    totalRun[pDone] = schProcess.RunTime;
                    schProcess.state = Terminated;
                    pDone++;
                }
            }
            else if (Stopped_RR_Ready.head != NULL)
            {
                DeQueue(&Stopped_RR_Ready, &schProcess);
                IncreaseWaitTime(&schProcess, getClk());
                if (schProcess.RemainingTime > time_quantum)
                {
                    kill(schProcess.PID, SIGCONT);
                    fprintf(SchedulerLog, "At time %d process %d resumed arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);

                    // sleep(time_quantum); WRONG
                    alarm(time_quantum + getClk() - x);
                    pause();
                    kill(schProcess.PID, SIGSTOP);
                    schProcess.RemainingTime = schProcess.RemainingTime - time_quantum;
                    fprintf(SchedulerLog, "At time %d process %d stopped arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    schProcess.state = Stopped;
                    IncreaseWaitTime(&schProcess, -getClk());
                    Add(&Stopped_RR_Ready, schProcess);
                }
                else
                {
                    kill(schProcess.PID, SIGCONT);
                    fprintf(SchedulerLog, "At time %d process %d resumed arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                    int status;
                    int x;
                    x = wait(&status);
                    WIFEXITED(status);
                    fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                    endTime = getClk();
                    WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
                    Wait[pDone] = schProcess.WaitTime;
                    totalRun[pDone] = schProcess.RunTime;
                    schProcess.state = Terminated;
                    pDone++;
                }
            }
        }
        x = getClk();
        if (pDone == maxCount)
        {
            // fifinshing 3ala 5er
            kill(getppid(), SIGTERM);
            break;
        }
    }
}
void handler1() // from sigchild
{
    raise(SIGALRM);
}
void SRTN()
{
    fprintf(SchedulerLog, "# The running algorithm is : SRTN\n");
    int count = maxCount; // the number of processes
    isRunning = false;
    schProcess.id = -1;
    struct PriorityQueue SRTN_Ready;
    initializeQueue(&SRTN_Ready);
    initClk();
    int x = getClk();
    int s = 0;
    while (1)
    {
        struct msgBuff processInfo;
        int pGeneratorToScheduler = msgget(1234, 0666 | IPC_CREAT);
        if (pGeneratorToScheduler == -1)
        {
            perror("error in creat\n");
            exit(-1);
        }
        if (c < maxCount)
        {
            val = 1;
            while (val != -1)
            {
                val = msgrcv(pGeneratorToScheduler, &processInfo, sizeof(processInfo.process), 0, IPC_NOWAIT); // ...........
                if (val == -1)
                    break;
                CopyPCB(&tempProcess, processInfo.process);
                AddAccordingToRemainingTime(&SRTN_Ready, tempProcess);
            }

            if (schProcess.id != -1 && isRunning == true)
            {
                schProcess.RemainingTime = schProcess.RemainingTime - (getClk() - schProcess.startTime);
                if (schProcess.RemainingTime <= 0 && schProcess.state != Terminated)
                {
                    schProcess.state = Terminated;
                    isRunning = false;
                    fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                    endTime = getClk();
                    WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
                    Wait[pDone] = schProcess.WaitTime;
                    totalRun[pDone] = schProcess.RunTime;
                    pDone++;
                    int status;
                    int y = wait(&status);
                    WIFEXITED(status);
                }
                if (SRTN_Ready.head != NULL)
                {
                    if (SRTN_Ready.head->pcb.RemainingTime < schProcess.RemainingTime)
                    {
                        schProcess.state = Stopped;
                        isRunning = false;
                        kill(schProcess.PID, SIGSTOP);
                        fprintf(SchedulerLog, "At time %d process %d stopped arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                        IncreaseWaitTime(&schProcess, -getClk());
                        AddAccordingToRemainingTime(&SRTN_Ready, schProcess);
                    }
                }
            }

            if (SRTN_Ready.head != NULL && isRunning == false)
            {
                DeQueue(&SRTN_Ready, &schProcess);
                schProcess.startTime = getClk();

                if (schProcess.state != Stopped)
                {
                    IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                    fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", schProcess.startTime, schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RunTime, schProcess.WaitTime);
                    if (s == 0)
                    {
                        startTime = schProcess.startTime;
                        s++;
                    }
                    schProcess.startTime = getClk();
                    IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                }
                if (schProcess.state == Stopped)
                {
                    IncreaseWaitTime(&schProcess, getClk());
                }
                Run(&schProcess);
                isRunning = true;
            }
        }

        if (pDone == maxCount)
        {
            break;
        }
        pause();
    }
}
void MDR(int x) //
{
    // printf("I received an alarm signal at %d\n", getClk());
}