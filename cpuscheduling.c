#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX 10

typedef struct
{
    char *name;

    int arrival;
    int cpuburst;
    int turnaround;
    int wait;

} Process;
Process processtable[MAX];

int size;
int curr_time = 0;

void ReadProcessTable(char *filename)
{
    FILE *fp;
    fp = fopen(filename, "r");
    char *line;
    int i = 0;
    char str[2];

    int str1;
    int str2;

    while (fscanf(fp, "%s %d %d", str, &str1, &str2) != EOF)
    {
        Process p;
        p.name = (char *)malloc(sizeof(char) * 2);

        strcpy(p.name, str);
        p.arrival = str1;
        p.cpuburst = str2;

        processtable[i] = p;

        i++;
    }
    size = i;
}

void PrintProcessTable()
{
    for (int i = 0; i < size; i++)
    {
        printf("%s %d %d\n", processtable[i].name, processtable[i].arrival, processtable[i].cpuburst);
    }
}

int compare(Process a, Process b)
{
    return (a.arrival - b.arrival);
}

void FCFS()
{
    for (int i = 1; i < size; i++)
        for (int j = 0; j < size - i; j++)
        {
            if (processtable[j].arrival > processtable[j + 1].arrival)
            {
                Process temp;
                temp = processtable[j];
                processtable[j] = processtable[j + 1];
                processtable[j + 1] = temp;
            }
        }
    curr_time = processtable[0].arrival;
    for (int i = 0; i < size; i++)
    {
        processtable[i].wait = curr_time - processtable[i].arrival;
        curr_time = curr_time + (processtable[i].cpuburst);
        processtable[i].turnaround = curr_time - processtable[i].arrival;
    }
}

void PrintStatistics()
{
    printf("Turnaround : ");
    int sumt = 0, sumw = 0;
    for (int k = 0; k < size; k++)
    {

        printf("%d ", processtable[k].turnaround);
        sumt = sumt + processtable[k].turnaround;
    }
    printf("\nWait     : ");
    for (int k = 0; k < size; k++)
    {

        printf("%d ", processtable[k].wait);
        sumw = sumw + processtable[k].wait;
    }
    printf("\n");
    printf("Average Turnaround: %d\n", (sumt) / size);
    printf("Average Wait      : %d\n", (sumw) / size);
}
void RR(int q)
{
    for (int i = 0; i < size; i++)
    {
        processtable[i].turnaround = 0;
        processtable[i].wait = 0;
    }
    for (int i = 1; i < size; i++)
        for (int j = 0; j < size - i; j++)
        {
            if (processtable[j].arrival > processtable[j + 1].arrival)
            {
                Process temp;
                temp = processtable[j];
                processtable[j] = processtable[j + 1];
                processtable[j + 1] = temp;
            }
        }
    int burstcopy[size];
    for (int i = 0; i < size; i++)
    {
        burstcopy[i] = processtable[i].cpuburst;
    }

    int flag = 1;
    curr_time = processtable[0].arrival;
    int k = 0;
    int i = 0;
    int start;
    start = curr_time;
    while (flag != 0)
    {

        k = 0;
        while (k < q && burstcopy[i] > 0)
        {
            burstcopy[i]--;
            curr_time++;
            k++;
        }
        if (burstcopy[i] == 0 && (processtable[i].turnaround == 0 && processtable[i].wait == 0))
        {

            processtable[i].turnaround = curr_time - processtable[i].arrival;
            processtable[i].wait = processtable[i].turnaround - processtable[i].cpuburst;
        }
        if (processtable[i + 1].arrival <= curr_time)
        {
            start = curr_time;
            i = (i + 1) % size;
        }
        else if (burstcopy[i - 1] < burstcopy[i])
        {
            i = (i - 1) % size;
        }
        else
        {
            continue;
        }
        int t = 0;
        while (burstcopy[t] == 0 && t < size)
        {
            t++;
        }
        if (t == size)
        {
            flag = 0;
        }
    }
}

void SRBF()
{
    int burstcopy[size];

    for (int k = 0; k < size; k++)
    {

        burstcopy[k] = processtable[k].cpuburst;
    }
    int flag = 0;
    int burst_time;
    int current_time = 0;
    while (flag != size)
    {
        int next_process = -1;
        int shortest_burst = 1000;
        for (int j = 0; j < size; j++)
        {
            if (processtable[j].arrival <= current_time && burstcopy[j] > 0)
            {
                if (burstcopy[j] < shortest_burst)
                {
                    next_process = j;
                    shortest_burst = burstcopy[j];
                }
            }
        }
        if (next_process == -1)
        {
            current_time++;
            continue;
        }
        burstcopy[next_process]--;
        current_time++;
        if (burstcopy[next_process] == 0)
        {
            flag++;
            processtable[next_process].turnaround = current_time - processtable[next_process].arrival;
            processtable[next_process].wait = processtable[next_process].turnaround - processtable[next_process].cpuburst;
        }
    }
}

int main(int argc, char *args[])
{
    char *filename = args[1];
    ReadProcessTable(filename);

    PrintProcessTable();
    printf("\n");
    int k = 1;

    printf("          CPU SCHEDULING SIMULATION       \n");
    printf("1.FCFS\n");
    printf("2.RR\n");
    printf("3.SRBF\n");
    printf("4.Exit\n");
    printf("\n Choose one of the above options:  ");
    scanf("%d", &k);

    while (k != 4)
    {
        switch (k)
        {
        case 1:
        {

            FCFS();
            PrintStatistics();
            break;
        }
        case 2:
        {

            int q;
            printf("Enter quantum: ");
            scanf("%d", &q);

            RR(q);
            PrintStatistics();
            break;
        }
        case 3:
        {

            SRBF();
            PrintStatistics();
            break;
        }
        }
        printf("          CPU SCHEDULING SIMULATION       \n");
        printf("1.FCFS\n");
        printf("2.RR\n");
        printf("3.SRBF\n");
        printf("4.Exit\n");
        printf("\nChoose one of the above options:  ");
        scanf("%d", &k);
    }
}
