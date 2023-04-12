#include "headers.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int finishtime;
};
struct processData *data;
struct processData x;
int size=0;
int main(int argc, char * argv[])
{
    initClk();
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
