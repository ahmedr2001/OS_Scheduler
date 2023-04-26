#include "headers.h"

/* Modify this file as needed*/
int remainingtime;
void interrupter(int signum)
{
    signal(SIGUSR1,interrupter);
    if(signum == SIGUSR1)
    {
        exit(remainingtime);
        return;
    }
}

int main(int agrc, char * argv[])
{
    initClk();
    signal(SIGUSR1,interrupter);
    remainingtime = atoi(argv[1]);
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    int prev = getClk();
    while (remainingtime > 0)
    {
        int current = getClk();
        if (prev < current)
        {
            remainingtime = remainingtime - 1;
            prev = current;
        }
        // sleep(1);
        // remainingtime = remainingtime - 1;

    }
    kill(getppid(),SIGUSR2);
    destroyClk(false);
        
    return 0;
}
