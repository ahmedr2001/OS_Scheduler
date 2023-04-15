#include "headers.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
struct processData
{
    int arrivaltime;
    int startingTime;
    int priority;
    int runningtime;
    int remainingTime;
    int id;
    int finishtime;
} typedef processData;
processData process_default = {-1,-1,-1,-1,-1,-1,-1};
struct processData *data;
struct processData x;
int size=0;

int HPF(int time);
int SRTN(int time);
processData RR(int time);

int main(int argc, char * argv[])
{
    initClk();
    x = process_default;
    printf("sched \n");
    fflush(stdout);
    // mkfifo("myfifo.txt", 0666);
    int open_pipe=open("ff",O_RDONLY);
    if(open_pipe < 0)
    {
        perror("Error opening myfifo pipe");
        exit(1);
    }
    if(read(open_pipe,&size,sizeof(size)) <= 0)
    {
        perror("Error reading the size from myfifo pipe");
        exit(1);
    }
    printf("size: %d\n", size);
    data = (struct processData*) malloc(size * sizeof(struct processData));
    data[0]=process_default;
    printf("zero id: %d\n", data[0].id);
    int i=0;
    while(read(open_pipe, &x, sizeof(x)) > 0) 
    {
        data[i].arrivaltime = x.arrivaltime;
        data[i].finishtime = x.finishtime;
        data[i].id = x.id;
        data[i].priority = x.priority;
        data[i].runningtime = x.runningtime;
        i = i + 1;
        printf("%d %d %d %d %d\n", data[i].arrivaltime, data[i].finishtime, 
        data[i].id, data[i].priority, data[i].runningtime);
    }
    printf("reading the processes to shcedular is done.\n");
    fflush(stdout);
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    destroyClk(true);
}

// Highest Priority First Algorithm
int HPF(int time)
{
    int scheduledProcessIndex = -1;
    bool occupied = 0;
    int max_priority = 11;
    for (int i = 0; i < size; i++) {
        if (data[i].id != -1) {
            int st_time = data[i].startingTime;
            int end_time = st_time + data[i].runningtime;
            if (st_time <= time && time <= end_time) {   // Non-preemptive algorithm
                occupied = 1;
                scheduledProcessIndex = i;
                break;
            }
            // Schedule the highest priority process (lowest value)
            if (data[i].priority < max_priority) {  
                max_priority = data[i].priority;
                scheduledProcessIndex = i;
            }
        }
    }
    if (!occupied && scheduledProcessIndex != -1) {
        data[scheduledProcessIndex].startingTime = time;
        
        data[scheduledProcessIndex].finishtime = time 
            + data[scheduledProcessIndex].runningtime;
    }
    return scheduledProcessIndex;
}

// Shortest Remaining Time Next Algorithm
int SRTN(int time)
{
    int scheduledProcessIndex = -1;
    int min_rt = 1e9;
    for (int i = 0; i < size; i++) {
        if (data[i].id != -1) {
            if (data[i].remainingTime < min_rt) {
                min_rt = data[i].remainingTime;
                scheduledProcessIndex = i;
            }
        }
    }

    return scheduledProcessIndex;
}