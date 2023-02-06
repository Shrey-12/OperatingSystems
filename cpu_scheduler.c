#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#define MAX_PROCESS 10
#define MAX_NAME_LENGTH 20

// look up what typedef does
// gloabl data structure: array of processes
static int entries = 0;

typedef struct
{
    int arrival;
    int cpuburst;
    int turnaround;
    int wait;
    char *name;
} Process;

Process processtable[MAX_PROCESS];

int CompareByArrivalTime(const void *a, const void *b)
{
    Process *p1 = (Process *)a;
    Process *p2 = (Process *)b;
    return p1->arrival - p2->arrival;
}

int CompareByBurstTime(const void *a, const void *b)
{
    Process *p1 = (Process *)a;
    Process *p2 = (Process *)b;
    return p1->cpuburst - p2->cpuburst;
}

void PrintProcessTable(int entries)
{
    printf("Process Name\tArrival Time\tCPU Burst\n");
    for (int i = 0; i < entries; i++)
    {
        printf("%s\t\t%d\t\t%d\n", processtable[i].name, processtable[i].arrival, processtable[i].cpuburst);
    }
}

// reads the input file into global data structure
void ReadProcessTable(char *filename)
{
    // open the input file
    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        printf("Error in opening file\n");
        return;
    }

    char name[20] = {'\0'};
    while (fscanf(file, "%s %d %d", name, &processtable[entries].arrival, &processtable[entries].cpuburst) == 3)
    {
        processtable[entries].name = (char *)malloc(sizeof(char) * MAX_NAME_LENGTH);
        strcpy(processtable[entries].name, name);
        entries++;
    }

    // Close the input file
    fclose(file);
}

// turnaround time is the waiting time+burst time

void FCFS(int entries)
{
    int current_time = 0;
    for (int i = 0; i < MAX_PROCESS; i++)

    {
        if (processtable[i].arrival > current_time)
        {
            current_time = processtable[i].arrival;
        }
        processtable[i].wait = current_time - processtable[i].arrival;
        current_time += processtable[i].cpuburst;
        processtable[i].turnaround = current_time - processtable[i].arrival;
    }
}

void PrintStatistics(int entries)
{
    int total_turnaround = 0, total_wait = 0;
    printf("Turnaround times: ");
    for (int i = 0; i < entries; i++)
    {
        total_turnaround += processtable[i].turnaround;
        printf("P%d[%d], ", i + 1, processtable[i].turnaround);
    }
    printf("\nWait times:       ");
    for (int i = 0; i < entries; i++)
    {
        total_wait += processtable[i].wait;
        printf("P%d[%d], ", i + 1, processtable[i].wait);
    }
    printf("\n");
    printf("Average turnaround time: %f\n", (float)total_turnaround / entries);
    printf("Average waiting time: %f\n", (float)total_wait / entries);
}

void RR(int quantum, int entries)
{
    int currenttime = 0;
    int first = 0; // index of the first process with positive remaining cpu burst time
    while (1)
    {
        int all_done = 1; // flag to check if all processes have completed
        for (int i = first; i < entries; i++)
        {
            if (processtable[i].cpuburst > 0)
            {
                all_done = 0; // at least one process has not completed
                int burst = (quantum > processtable[i].cpuburst) ? processtable[i].cpuburst : quantum;
                processtable[i].cpuburst -= burst;
                if (processtable[i].cpuburst == 0)
                {
                    // process has completed, update turnaround time
                    processtable[i].turnaround = currenttime + burst - processtable[i].arrival;
                }
                if (processtable[i].arrival > currenttime)
                {
                    currenttime = processtable[i].arrival;
                }
                processtable[i].wait = currenttime - processtable[i].arrival;
                currenttime += burst;
            }
        }
        if (all_done)
            break; // all processes have completed, exit loop
        currenttime += quantum;
    }
}


void SRTF(int size){
    int burstcopy[size];

    for (int k = 0; k < size; k++) 
    {

        burstcopy[k] = processtable[k].cpuburst;
    }

    int flag = 0;
    int burst_time;
    int current_time = 0;
    while (flag != size) {
        int next_process = -1;
        int shortest_burst = 1000; //intmax
        for (int j = 0; j < size; j++) {
            if (processtable[j].arrival <= current_time && burstcopy[j] > 0) {
                if (burstcopy[j] < shortest_burst) {
                    next_process = j;
                    shortest_burst = burstcopy[j];
                }
            }
        }
        if (next_process == -1) {
            current_time++;
            continue;
        }
        burstcopy[next_process]--;
        current_time++;
        if (burstcopy[next_process] == 0) {
            flag++;
            processtable[next_process].turnaround = current_time - processtable[next_process].arrival;
            processtable[next_process].wait = processtable[next_process].turnaround - processtable[next_process].cpuburst;
        }
    }
}




int main()
{
    ReadProcessTable("process1.txt");
    qsort(processtable, entries, sizeof(Process), CompareByArrivalTime);
    PrintProcessTable(entries);
    // FCFS(entries);
     RR(1, entries);
    //PrintStatistics(entries);

    //qsort(processtable, entries, sizeof(Process), CompareByBurstTime);
     //SRTF(entries);
     //PrintProcessTable(entries);
     PrintStatistics(entries);

    return 0;
}