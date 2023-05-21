#include <stdio.h>
#include <unistd.h>


// defining the diffrent states of a process
typedef short STATE;
#define NotStarted 0
#define Waiting 1
#define Running 2
#define Terminated 3 
#define Stopped 4


// defining the PCB struct (Process Control Block)
struct PCB{
    int id; // process id
    int ArrTime; // time of arrival
    int RunTime; // running time (in state of running it must be devremented)
    int Priority; // priority (the less the number the higher the priority)
    int WaitTime; // time spent waiting
    int state; // ready or waiting or running or terminated
    int TA; // turnaround time
    int RemainingTime; // RemainingTime time
    int startTime;
    int endTime;
    double WTA; // weighted tur arround time
    int PID; // pid of the actual created process
    int memsize;
    int mem_st_idx;
    int mem_end_idx;
};

// sets the main prameters of the pcb
//  at the initialinzation the pcb hasn't arrived  
void setPCB(struct PCB* pcb, int ID, int ARR, int RUN, int Pr, int mem)
{
    pcb->id = ID;
    pcb->ArrTime = ARR;
    pcb->RunTime = RUN;
    pcb->RemainingTime = pcb->RunTime;
    pcb->Priority = Pr;
    pcb->memsize = mem;
    pcb->RemainingTime = RUN;
    pcb->state = NotStarted;
    pcb->WaitTime = 0;
}

void setMemIdx(struct PCB *pcb, int st_idx, int end_idx)
{
    pcb->mem_st_idx = st_idx;
    pcb->mem_end_idx = end_idx;
}

//sets the value of a pcb by the value of another pcb
void CopyPCB(struct PCB* pcb,struct PCB other)
{
    pcb->id = other.id;
    pcb->ArrTime = other.ArrTime;
    pcb->RunTime = other.RunTime;
    pcb->Priority = other.Priority;
    pcb->WaitTime = other.WaitTime;
    pcb->state = other.state;
    pcb->TA = other.TA;
    pcb->WTA = other.WTA;
    pcb->PID = other.PID;
    pcb->startTime=other.startTime;
    pcb->endTime=other.endTime;
    pcb->RemainingTime=other.RemainingTime;
    pcb->memsize = other.memsize;
    pcb->mem_st_idx = other.mem_st_idx;
    pcb->mem_end_idx = other.mem_end_idx;
}

// sets the state
void setState(struct PCB* pcb, STATE s)
{
    pcb->state = s;
}

// sets the wait time
void SetWaitTime(struct PCB* pcb, int wait)
{
    pcb->WaitTime = wait;
}

// 
void IncreaseWaitTime(struct PCB* pcb, int NewWait)
{
    pcb->WaitTime = pcb->WaitTime + NewWait;
}

void fork_process(struct PCB* pcb)
{
    // Create a new process using fork()
    int process = fork();

    // Check if there was an error
    if(process == -1)
    {
        printf("error in forking\n");
    }
    // The child process
    else if(process == 0)
    {
        // Convert the process ID to a character
        char sendid = pcb->id + '0';
        char * sendingid = &sendid;
        execl("./process.out", "./process.out", sendingid, (char *)NULL);
    }
}



