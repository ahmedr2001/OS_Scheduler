#include "headers.h"
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
char line[100];
int n_of_algo;
int size = 0;
int capacity = 1;
struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int finishtime;
};
struct processData *arr;
int shmid;
void clearResources(int);

int main(int argc, char *argv[])
{

    FILE *input_file = fopen("input test.txt", "r");
    if (!input_file)
    {
        printf("Error opening file\n");
        return 1;
    }
    arr = (struct processData *)malloc(capacity * sizeof(struct processData));
    while (fgets(line, sizeof(line), input_file) != NULL)
    {
        if (!(line[0] == '#'))
        {
            printf("line: %s\n", line);
            printf("\n");
            sscanf(line, "%d   %d   %d   %d", &arr[size].id, &arr[size].arrivaltime, &arr[size].runningtime, &arr[size].priority);
            arr[size].finishtime = -1;
            size = size + 1;
        }
        if (size == capacity)
        {
            capacity *= 2;
            arr = (struct processData *)realloc(arr, capacity * sizeof(struct processData));
            if (arr == NULL)
            {
                printf("Memory reallocation failed\n");
                return 1;
            }
        }
    }
    fclose(input_file);
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files. DONE
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any. DONE
    // 3. Initiate and create the scheduler and clock processes. DONE
    // 4. Use this function after creating the clock process to initialize clock. DONE
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("Error in fork!");
        exit(-1);
    }
    else if (pid == 0)
    {
        execl("./clk.out", "./clk.out", NULL); // execute the clock process
        perror("Error in execl clk.out!");
        exit(-1);
    }
    else
    {
        initClk();
        signal(SIGINT, clearResources); // in case of CTRL + c interrupt
        printf("Enter the algo number : 1, 2, 3 \n");
        scanf("%d", &n_of_algo);
        printf("%d \n", n_of_algo);
        int x = getClk();
        // int x = 4;
        printf("current time is %d\n", x);

        printf("sadasda");
        mkfifo("ff", 0666);
        printf("2sadasda");
        fflush(stdout);
        int open_pipe = open("ff", O_WRONLY);
        printf("fghf %d", open_pipe);
        fflush(stdout);
        if (open_pipe < 0)
        {
            printf("3sadasd");
            fflush(stdout);
            perror("Error opening myfifio pipe");
            exit(1);
        }

        write(open_pipe, &size, sizeof(size));
        int i = 0;
        while (i < size) {
            if (getClk() >= arr[i].arrivaltime) {
                printf("writing\n");
                fflush(stdout);
                printf("arrvals: %d %d\n", arr[0].arrivaltime, arr[0].id);
                write(open_pipe, &arr[i], sizeof(arr[i]));
                i++;
            }
        }
        // for (size_t i = 0; i < size; i++)
        // {
        //     while (getClk() >= arr[i].arrivaltime)
        //     {
        //         // TODO
        //         // 5) sending the arr[i] to the shceduler DONE
        //         printf("writing");
        //         fflush(stdout);
        //         write(open_pipe, &arr[i], sizeof(arr[i]));
        //     }
        // }
        close(open_pipe);
        execl("./scheduler.out", "./scheduler.out", NULL);
    }
    // return 0;

    // TODO Generation Main Loop
    // 6. Create a data structure for processes and provide it with its parameters. DONE
    // 7. Send the information to the scheduler at the appropriate time. DONE
    // 8. Clear clock resources
    destroyClk(true);
    clearResources(0);
}

void clearResources(int signum)
{
    // TODO Clears all resources in case of interruption
    // destroyClk(1);
    shmdt(arr);
    // Deallocate the shared memory segment
    shmctl(shmid, IPC_RMID, NULL);
    // Destroy the semaphore
    // semctl(semid, 0, IPC_RMID);
    // Destroy the clock
    destroyClk(true);
    // Exit the program
    exit(signum);
}
