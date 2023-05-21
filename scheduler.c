#include "headers.h"
#include "linkedlist.h"
#include "math.h"
#include "ff.h"

linkedlist *free_list[11];
int mp[1024];

void init_free_list()
{
    for(int i=1;i<11;i++){
        free_list[i]=newLinkedList();
    }
    insertSorted(free_list[10], 0);
}

void init_mp()
{
    for(int i=0;i<1024;i++)mp[i]=-1;
}

int calc_block_idx(int req_size)
{
    if(req_size==1)return 1;
    return (int)ceil(log2(req_size));
}

int split(int st_idx, int avail_size, int req_size)
{
    if (avail_size == req_size) {
        return st_idx + req_size - 1;
    }

    int buddyAddress = st_idx + avail_size / 2;
    int list_idx = log2(avail_size / 2);
    insertSorted(free_list[list_idx], buddyAddress);

    return split(st_idx, avail_size / 2, req_size);
}

int allocate(int req_size, int *end_idx)
{
    int block_idx = calc_block_idx(req_size);
    int first_free_idx = block_idx;
    while (first_free_idx < 11) {
        if(isEmpty(free_list[first_free_idx])){
            first_free_idx++;
        }
        else break;
    }

    if (first_free_idx == 11) {
        // printf("passed alloc\n");
        return -1;
    }

    int st_idx_alloc = free_list[first_free_idx]->head->data;
    deleteNode(free_list[first_free_idx], st_idx_alloc);

    int avail_size = pow(2, first_free_idx);
    int mem_size = pow(2, block_idx);

    *end_idx = split(st_idx_alloc, avail_size, mem_size);

    mp[st_idx_alloc] = req_size;

    return st_idx_alloc;
}

int deallocate(int req_size, int st_idx, int end_idx)
{
    // printf("--------------------------------------\n");
    int st, end, sz;
    int block_idx = calc_block_idx(req_size);
    if (~mp[st_idx]) {
        st = st_idx, end = end_idx, sz = pow(2, block_idx);
        // printf("st: %d end %d sz %d\n", st, end, sz);
        for (int i = block_idx; i < 11; i++){
            int buddyAddress;
            int buddyNumber = st / sz;
            if (buddyNumber & 1) {
                buddyAddress = st - sz;
            }
            else {
                buddyAddress = end + 1;
            }
            if (nodeExists(free_list[i], buddyAddress)) {
                deleteNode(free_list[i], buddyAddress);
                if(buddyNumber&1){
                    st = buddyAddress;
                }
                else {
                    end += sz;
                }
                sz *= 2;
                // printf("st: %d end %d sz %d\n", st, end, sz);
            }
            else break;
        }
        int list_idx = log2(sz);
        insertSorted(free_list[list_idx], st);
        // printf("st: %d end %d sz %d\n", st, end, sz);
        mp[st_idx] = -1;
        
        return 0;
    }
    else return -1;
}

#define hpf_Algo 1
#define strn_Algo 2
#define rr_Algo 3
char Algo;
char memPol;
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
FILE *MemoryLog;
int maxCount;
int parentID;
double *WTA;
double *Wait;
double *totalRun;
int endTime;
int startTime;
bool deallocated=false;
int main(int argc, char *argv[])
{
    init_free_list();
    init_mp();
    init_mem_blocks();
    init_mp_ff();
    // signal(SIGCHLD, SIG_IGN);
    signal(SIGALRM, MDR);
    Algo = atoi(argv[1]);
    if (Algo == 3)
        time_quantum = atoi(argv[2]);
    parentID = atoi(argv[3]);
    maxCount = atoi(argv[4]);
    memPol = atoi(argv[5]);
    WTA = calloc(maxCount, sizeof(double));
    Wait = calloc(maxCount, sizeof(double));
    totalRun = calloc(maxCount, sizeof(double));

    initClk();
    int x = getClk();

    SchedulerLog = fopen("scheduler.log", "w");
    MemoryLog = fopen("memory.log", "w");
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
    fclose(MemoryLog);
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
    struct PriorityQueue HPF_Waiting;
    initializeQueue(&HPF_Ready);
    initializeQueue(&HPF_Waiting);
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
            while(HPF_Waiting.head != NULL){
                DeQueue(&HPF_Waiting, &schProcess);
                AddAccordingToPriority(&HPF_Ready, schProcess);
            }
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
                printf("id: %d\n", schProcess.id);
                int mem_end_idx;
                int mem_st_idx;
                if(memPol==1){
                    mem_st_idx=ff_allocate(schProcess.memsize, &mem_end_idx);
                }
                else if (memPol==2){
                    mem_st_idx = allocate(schProcess.memsize, &mem_end_idx);
                }
                if(mem_st_idx==-1){
                    AddAccordingToPriority(&HPF_Waiting, schProcess);
                    printf("failed alloc id: %d st: %d\n", schProcess.id, schProcess.mem_st_idx);
                } 
                else {
                    setMemIdx(&schProcess, mem_st_idx, mem_end_idx);
                    schProcess.startTime = getClk();
                    IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                    fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", schProcess.startTime, schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RunTime, schProcess.WaitTime);
                    fprintf(MemoryLog, "At time %d allocated %d bytes for process %d from %d to %d\n", schProcess.startTime, schProcess.memsize, schProcess.id, schProcess.mem_st_idx, schProcess.mem_end_idx);
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
                    if(memPol==1){
                        ff_deallocate(schProcess.memsize, schProcess.mem_st_idx, schProcess.mem_end_idx);
                    }
                    else if(memPol==2){
                        deallocate(schProcess.memsize, schProcess.mem_st_idx, schProcess.mem_end_idx);
                    }
                    fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                    fprintf(MemoryLog, "At time %d freed %d bytes from process %d from %d to %d\n", getClk(), schProcess.memsize, schProcess.id, schProcess.mem_st_idx, schProcess.mem_end_idx);
                    endTime = getClk();
                    WTA[pDone] = (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime;
                    Wait[pDone] = schProcess.WaitTime;
                    totalRun[pDone] = schProcess.RunTime;
                    schProcess.state = Terminated;
                    pDone++;
                }
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
    struct PriorityQueue RR_Waiting;
    initializeQueue(&RR_Ready);
    initializeQueue(&RR_Waiting);
    struct PriorityQueue Stopped_RR_Ready;
    initializeQueue(&Stopped_RR_Ready);
    int x = getClk();

    int s = 0;
    while (1)
    {
        if(deallocated){
            while(RR_Waiting.head != NULL){
                DeQueue(&RR_Waiting, &tempProcess);
                Add(&RR_Ready, tempProcess);

                // printf("now ready id: %d\n", schProcess.id);
            }
            deallocated = false;
        }
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
                // printf("received id: %d\n", tempProcess.id);
            }

            if (RR_Ready.head != NULL)
            {
                DeQueue(&RR_Ready, &schProcess);
                printf("id: %d\n", schProcess.id);
                schProcess.startTime = getClk();
                IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                if (schProcess.RemainingTime > time_quantum)
                {
                    int mem_end_idx;
                    int mem_st_idx;
                    if(memPol==1){
                        printf("memsize: %d id: %d\n", schProcess.memsize, schProcess.id);
                        mem_st_idx = ff_allocate(schProcess.memsize, &mem_end_idx);
                        if(schProcess.id==8)printf("failed alloc\n");
                    }
                    else if (memPol==2){
                        mem_st_idx = allocate(schProcess.memsize, &mem_end_idx);
                    }
                    // printf("here\n");
                    if(mem_st_idx==-1){
                        Add(&RR_Waiting, schProcess);
                    }
                    else{
                        schProcess.mem_st_idx=mem_st_idx;
                        schProcess.mem_end_idx=mem_end_idx;
                        fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RemainingTime, schProcess.WaitTime);
                        fprintf(MemoryLog, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), schProcess.memsize, schProcess.id, schProcess.mem_st_idx, schProcess.mem_end_idx);
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
                }
                else
                {
                    int mem_end_idx;
                    int mem_st_idx ;
                    if(memPol==1){
                        printf("memsize: %d id: %d\n", schProcess.memsize, schProcess.id);
                        mem_st_idx = ff_allocate(schProcess.memsize, &mem_end_idx);
                        if(schProcess.id==8)printf("failed alloc\n");
                    }
                    else if(memPol==2){
                       mem_st_idx = allocate(schProcess.memsize, &mem_end_idx);
                    }
                    // printf("here\n");
                    if(mem_st_idx==-1){
                        Add(&RR_Waiting, schProcess);
                                                printf("failed alloc\n");

                    }
                    else{
                        schProcess.mem_st_idx=mem_st_idx;
                        schProcess.mem_end_idx=mem_end_idx;
                        fprintf(MemoryLog, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), schProcess.memsize, schProcess.id, schProcess.mem_st_idx, schProcess.mem_end_idx);
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
                        deallocated = true;
                        if(memPol==1){
                            int x=ff_deallocate(schProcess.memsize, schProcess.mem_st_idx, schProcess.mem_end_idx);
                            printf("mem[0]: %d\n", x);
                        }
                        else if(memPol==2){
                            deallocate(schProcess.memsize, schProcess.mem_st_idx, schProcess.mem_end_idx);
                        }
                        fprintf(MemoryLog, "At time %d freed %d bytes from process %d from %d to %d\n", getClk(), schProcess.memsize, schProcess.id, schProcess.mem_st_idx, schProcess.mem_end_idx);
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
                    deallocated = true;
                    if(memPol==1){
                        int x=ff_deallocate(schProcess.memsize, schProcess.mem_st_idx, schProcess.mem_end_idx);
                        printf("mem[0]: %d\n", x);
                    }
                    else if (memPol==2){
                        deallocate(schProcess.memsize, schProcess.mem_st_idx, schProcess.mem_end_idx);
                    }
                    fprintf(MemoryLog, "At time %d freed %d bytes from process %d from %d to %d\n", getClk(), schProcess.memsize, schProcess.id, schProcess.mem_st_idx, schProcess.mem_end_idx);
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
    struct PriorityQueue SRTN_Waiting;
    initializeQueue(&SRTN_Ready);
    initializeQueue(&SRTN_Waiting);
    initClk();
    int x = getClk();
    int s = 0;
    while (1)
    {
        if(true){
            while(SRTN_Waiting.head != NULL){
                DeQueue(&SRTN_Waiting,&tempProcess);
                AddAccordingToRemainingTime(&SRTN_Ready, tempProcess);
            }
            deallocated=false;
        }
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
                    printf("id: %d st_idx: %d\n", schProcess.id, schProcess.mem_st_idx);
                    if(memPol==1){
                        ff_deallocate(schProcess.memsize, schProcess.mem_st_idx, schProcess.mem_end_idx);
                    }
                    else if (memPol==2){
                        deallocate(schProcess.memsize, schProcess.mem_st_idx, schProcess.mem_end_idx);
                    }
                    schProcess.state = Terminated;
                    isRunning = false;
                    deallocated=true;
                    while(SRTN_Waiting.head != NULL){
                        DeQueue(&SRTN_Waiting,&tempProcess);
                        AddAccordingToRemainingTime(&SRTN_Ready, tempProcess);
                    }
                    deallocated=false;
                    fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", getClk(), schProcess.id, schProcess.ArrTime, schProcess.RunTime, 0, schProcess.WaitTime, getClk() - (schProcess.ArrTime), (double)(getClk() - (schProcess.ArrTime)) / schProcess.RunTime);
                    fprintf(MemoryLog, "At time %d freed %d bytes from process %d from %d to %d\n", getClk(), schProcess.memsize, schProcess.id, schProcess.mem_st_idx, schProcess.mem_end_idx);
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
                        printf("stopped id: %d\n", schProcess.id);
                        AddAccordingToRemainingTime(&SRTN_Ready, schProcess);
                    }
                }
            }

            if (SRTN_Ready.head != NULL && isRunning == false)
            {
                struct PCB prevProcess = schProcess;
                DeQueue(&SRTN_Ready, &schProcess);
                printf("deq id: %d st: %d\n", schProcess.id, schProcess.mem_st_idx);
                schProcess.startTime = getClk();

                if (schProcess.state != Stopped)
                {
                    int mem_end_idx;
                    int mem_st_idx;
                    if(memPol==1){
                        mem_st_idx=ff_allocate(schProcess.memsize, &mem_end_idx);
                    }
                    else if(memPol==2){
                        mem_st_idx=allocate(schProcess.memsize, &mem_end_idx);
                    }
                    if(mem_st_idx==-1){
                        AddAccordingToRemainingTime(&SRTN_Waiting, schProcess);
                        printf("waiting id: %d\n", schProcess.id);
                        struct PCB tmp;
                        struct PriorityQueue Tmp;
                        while(SRTN_Ready.head){
                            DeQueue(&SRTN_Ready, &tmp);
                            AddAccordingToRemainingTime(&Tmp, tmp);
                            if(tmp.state==Stopped){
                                kill(tmp.PID, SIGCONT);
                                break;
                            }
                        }
                        while(Tmp.head){
                            DeQueue(&Tmp, &tmp);
                            AddAccordingToRemainingTime(&SRTN_Ready, tmp);
                        }
                    }
                    else{
                        setMemIdx(&schProcess, mem_st_idx, mem_end_idx);
                        printf("alloc st idx id: %d st_idx: %d\n", schProcess.id, schProcess.mem_st_idx);
                        IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                        fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d \n", schProcess.startTime, schProcess.id, schProcess.ArrTime, schProcess.RunTime, schProcess.RunTime, schProcess.WaitTime);
                        fprintf(MemoryLog, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), schProcess.memsize, schProcess.id, schProcess.mem_st_idx, schProcess.mem_end_idx);
                        if (s == 0)
                        {
                            startTime = schProcess.startTime;
                            s++;
                        }
                        schProcess.startTime = getClk();
                        IncreaseWaitTime(&schProcess, schProcess.startTime - schProcess.ArrTime);
                        Run(&schProcess);
                        isRunning = true;
                    }
                }
                /*if (schProcess.state == Stopped)*/
                else
                {
                    IncreaseWaitTime(&schProcess, getClk());
                    Run(&schProcess);
                    isRunning = true;
                }
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