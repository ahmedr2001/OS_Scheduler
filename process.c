#include <stdlib.h>
#include "headers.h"
#include <time.h>

/* Modify this file as needed*/
int remainingtime;
int main(int agrc, char * argv[])
{
    
    /*
    The while loop checks the current CPU time used by the program (as returned by clock()) 
    against the target wait time (which is specified in seconds and converted to CPU time using CLOCKS_PER_SEC). 
    The loop continues until the target wait time has elapsed.
    */
    initClk();
    remainingtime = atoi(argv[1]);
    while(clock() < remainingtime * CLOCKS_PER_SEC);
    return 0;
}

