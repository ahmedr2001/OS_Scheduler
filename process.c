#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char * argv[])
{
    initClk();
    remainingtime = atoi(argv[1]);
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    while (remainingtime > 0)
    {
        remainingtime = getClk();
    }
    
    destroyClk(false);
    
    return 0;
}
