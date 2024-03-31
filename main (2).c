#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> //library for isdigit function
#define INFINITY_INT 999999999
struct Process
{
    int process_id;//the id process to know which is the sequence of processes after sort the array
    int arrival_time;//the default arrival time is zero
    int CPU_burst;
    int CPU_burst_copy;//this copy from CPU burst to calculate the waiting time and to print the burst time (because CPU_burst will change so i need a copy)
    int completion_time;
    int turaround_time;//some of needed metrics is saved for each process in its struct
    int response_time;
    int waiting_time;
    int flag;//flag to check if the process done its CPU burst time (for RR)
    int quantum;//for the RR algorithm
    int priority;//for the priority algorithm
    int exec_previously;//HOW MUCH CPU BURSTS EXECUTED PREVIOUSLY (FOR PREEMPTIVE PRIO. ALGO.)
    int enteredCPU;//HOW MANY TIMES THE PROCESS ENTERED THE CPU
};
typedef struct Process Process;

int compare(const void* num1, const void* num2) // comparing function for qsort function
{
    const Process *a = (const Process *) num1;//const mean that the pointer value will not change
    const Process *b = (const Process *) num2;
    if (a->arrival_time == b->arrival_time)//this statement is when two processes come at the same time don't change their order
        return 1;
    return a->arrival_time - b->arrival_time;
}

int checkIfDigit(char theword[30])//function to check if the word is digit or no
{
    if (0 >= (int)(theword[0]) - '0')//the number cannot be zero (- '0' because the ASCII code for zero = 48 -> int('0') = 48)
    {
        printf("The number must be greater than zero only!!\n");
        return 1;
    }
    for(int i = 0 ; i < strlen(theword) ; i++)
    {
        if(!isdigit(theword[i])) //check if the word contain character or a negative number
        {
            printf("You Must enter a positive digit only !\n");
            return 1;
        }
    }
    return 0;
}

int checkTime(char time[30])//function to check if the time is legal
{
    if(0 > (int)(time[0]) - '0')//minus zero to check if the time is less than zero (because zero in ASCII = 48)
    {
        printf("The time must be greater than or equal zero!!\n");
        return 1;
    }
    for(int i = 0 ; i < strlen(time) ; i++)
    {
        if(!isdigit(time[i]))
        {
            printf("You Must enter a positive integer only!\n");
            return 1;
        }
    }
    return 0;
}

void printList()
{
    printf("chose from bellow list:\n");
    printf("1. Update processes and their relevant properties\n");
    printf("2. Print Gantt Chart and evaluate metrics using Round-Rubin (RR) algorithm\n");
    printf("3. Print Gantt Chart and evaluate metrics using Priority algorithm\n");
    printf("4. Print Gantt Chart and evaluate metrics using Shortest-Job-First (SJF) algorithm\n");
    printf("5. Print Gantt Chart and evaluate metrics using Shortest-remaining-time-first algorithm\n");
    printf("6. Print Gantt Chart and evaluate metrics using First-Come-First-Served (FCFS) algorithm\n");
    printf("7. Print Processes and Their relevant information\n");
    printf("8. Exit\n");

}
int t = 0; //global variable used to print and calculate CPU utilization (for time)
void RoundRubin(Process processes[], int num_processes,int quantum, int time, int count_response_time)//time is variable that will be used as a real time simulator and count_response will be used as a condition to count the response time
{
    //count_response_time variable is used to calculate the response time for each process (as flag to calculate once)
    if(time == 0)
        printf("The Gantt Chart:\n");//print this line once only at the beginning
    //first the algorithm must order the processes on their arrival time using qsort function
    qsort(processes, num_processes, sizeof(Process), compare);
    int first_arrival;
    for(int k = 0 ; k < num_processes ; k++)//this for loop to check if the CPU is ideal or not and to know the time for first arrival process
    {
        if(processes[k].flag) //if the process not finished it work yet and it is the first one in array (first one after sort the array)
        {
            first_arrival = processes[k].arrival_time;
            while(time < first_arrival)
            {
                printf("I%d|", time);
                time++;//this time is when the CPU Ideal
            }
            break;
        }
    }

    for(int i = 0 ; i < num_processes ; i++)
    {
        if(time < processes[i].arrival_time)//check if the time is greater than arrival time for process meaning that this process is not available now so increment i by one
            i++;//this condition is to check if the process in ready queue or not (don't calculate any thing until process enter the CPU)
        else
        {
            int q = quantum;
            if(count_response_time)//this flag is set for just 1 time true and then false (because response time is computed just once)
                processes[i].response_time = time - processes[i].arrival_time;//when the first response happened this is the response time (when process enters CPU)
            if(processes[i].flag)
                printf("|p%d", processes[i].process_id);//printing line used to print Gantt chart
            while(q > 0 && processes[i].CPU_burst_copy != 0)//loop while there is quantum or there is work to be done
            {
                printf(" ");//print in Gantt chart that process is working
                processes[i].CPU_burst_copy--;
                q--;
                time++;

                if(processes[i].CPU_burst_copy == 0)//if there is no work then this process is done so we can calculate what we want and change the flag to zero
                {
                    printf("%d|", time);//for Gantt chart
                    processes[i].flag = 0;//process i is done
                    processes[i].completion_time = time;//save the completion time for the process
                    processes[i].turaround_time = processes[i].completion_time - processes[i].arrival_time;//save the turnaround time
                    processes[i].waiting_time = processes[i].turaround_time - processes[i].CPU_burst;//also save the waiting time for each process (the whole time minus time working in CPU)
                }

            }
            if(q == 0 && processes[i].CPU_burst_copy != 0)//if the process not finish and the quantum do then print for Gantt chart (and condition to prevent process from printing more than once)
                printf("%d|", time);

        }

    }

    t = time;//return the time to calculate the utilization of CPU
    for(int j = 0 ; j < num_processes ; j++)//this loop is very important to check if there is process that need more than quantum time to finish
    {
        if(processes[j].flag != 0)//recursively call the RR and do the same operation on the remaining processes
            RoundRubin(processes, num_processes, quantum, time, 0);//zero because the response time was already calculated
    }
}

void FirstComeFirstServed(Process processes[], int num_processes, int time, int count_response_time)//time is variable that will be used as a real time simulator and count_response will be used as a condition to count the response time
{
    //first the algorithm must order the processes on their arrival time using qsort function
    qsort(processes, num_processes, sizeof(Process), compare);
    printf("The Gantt Chart:\n");
    int first_arrival;
    for(int k = 0 ; k < num_processes ; k++)//this for loop to check if the CPU is ideal or not
    {
        if(processes[k].flag) //the first arrival process in array
        {
            first_arrival = processes[k].arrival_time;
            while(time < first_arrival)
            {
                printf("I%d|", time);
                time++;//this time is when the CPU Ideal
            }

            break;
        }
    }
    for(int i = 0 ; i < num_processes ; i++)
    {
        while(time < processes[i].arrival_time)
        {
            printf("|I%d|", time);
            time++;//this time is when the CPU Ideal
        }
        if(count_response_time)//this flag is set for just 1 time true and then false (because response time is computed just once)
            processes[i].response_time = time - processes[i].arrival_time;//when the first response happened this is the response time (when process enters CPU)
        printf("|p%d", processes[i].process_id);
        while(processes[i].CPU_burst_copy != 0)//loop while there is quantum or there is work to be done
        {
            printf(" ");
            processes[i].CPU_burst_copy--;
            time++;

            if(processes[i].CPU_burst_copy == 0)//if there is no work then this process is done so we can calculate what we want and change the flag to zero
            {
                printf("%d|", time);
                processes[i].completion_time = time;//save the completion time for the process
                processes[i].turaround_time = processes[i].completion_time - processes[i].arrival_time;//save the turnaround time
                processes[i].waiting_time = processes[i].turaround_time - processes[i].CPU_burst;//also save the waiting time for each process
            }

        }
    }
    printf("\n");
    t = time;//return the time to calculate the utilization of CPU
}
void ShortestJobFirst(Process processes[], int num_processes)
{
    qsort(processes, num_processes, sizeof(Process), compare);

    int currentTime = 0;
    int finishedProcesses = 0;
    int lastPrint = -1; // To make Gantt Chart fancier
    int isIdle = 0; // flag to check if the CPU is idle

    printf("Gantt Chart:\n");

    if (processes[0].arrival_time != 0)
    {
        isIdle = processes[0].arrival_time;
        while (isIdle > 0)
        {
            printf("%d|I\t", currentTime);
            isIdle--;
            currentTime++;
        }
    }

    while (finishedProcesses != num_processes)
    {
        int shortest = INFINITY_INT;
        int index = -1;

        for (int i = 0; i < num_processes; i++)
        {
            if (processes[i].flag == 1 && processes[i].arrival_time <= currentTime)
            {
                if (processes[i].CPU_burst_copy < shortest)
                {
                    shortest = processes[i].CPU_burst_copy;
                    index = i;
                }
            }
        }

        if (index != -1)
        {
            if (lastPrint != index)
            {
                printf("%d|P%d\t", currentTime, processes[index].process_id);
                lastPrint = index;
            }
            else
            {
                printf(" ");
            }

            processes[index].completion_time = currentTime + processes[index].CPU_burst_copy;
            processes[index].turaround_time = processes[index].completion_time - processes[index].arrival_time;
            processes[index].waiting_time = processes[index].turaround_time - processes[index].CPU_burst;
            processes[index].response_time = processes[index].waiting_time;
            processes[index].flag = 0;

            currentTime += processes[index].CPU_burst_copy;
            finishedProcesses++;
        }
        else
        {
            isIdle = 1;
            printf("%d|I\t", currentTime);
            currentTime++;
            lastPrint = -1;
        }
    }

    printf("|%d\n", currentTime);
}



void ShortestRemainingTimeFirst(Process processes[], int num_processes)
{
    qsort(processes, num_processes, sizeof(Process), compare);

    int currentTime = 0;
    int finishedProcesses = 0;
    int remainingTime[num_processes];
    int isIdle = 0;
    int lastPrint = -1; // To make Gantt Chart fancier

    printf("Gantt Chart:\n");

    for (int i = 0; i < num_processes; i++)
    {
        remainingTime[i] = processes[i].CPU_burst;
    }

    if (processes[0].arrival_time != 0)
    {
        isIdle = processes[0].arrival_time;
        while (isIdle > 0)
        {
            printf("%d|I\t", currentTime);
            isIdle--;
            currentTime++;
        }
    }

    while (finishedProcesses != num_processes)
    {
        int min_burst = INFINITY_INT;
        int index = -1;

        for (int i = 0; i < num_processes; i++)
        {
            if (processes[i].flag && processes[i].arrival_time <= currentTime && remainingTime[i] < min_burst)
            {
                min_burst = remainingTime[i];
                index = i;
            }
        }

        if (index != -1)
        {
            remainingTime[index]--;
            currentTime++;

            if (lastPrint != index)
            {
                printf("%d|P%d\t", currentTime - 1, processes[index].process_id);
                lastPrint = index;
            }
            else
            {
                printf(" ");
            }

            if (remainingTime[index] == 0)
            {
                processes[index].completion_time = currentTime;
                processes[index].turaround_time = processes[index].completion_time - processes[index].arrival_time;
                processes[index].waiting_time = processes[index].turaround_time - processes[index].CPU_burst;
                processes[index].response_time = processes[index].waiting_time;
                processes[index].flag = 0;

                finishedProcesses++;
            }
        }
        else
        {
            isIdle = 1;
            printf("%d|I\t", currentTime);
            currentTime++;
            lastPrint = -1;
        }
    }

    printf("|%d\n", currentTime);
}


//NONPREEMPTIVE PRIORITY ALGO
int finalTime1=0;
void NonpreemptivePriorityAlgo(Process processes[], int num_processes)
{

    //SORTING THE P'S ACCORDING TO THEIR ARRIVAL TIMES
    for (int i = 0; i<num_processes-1; i++)
    {
        for (int j = 0; j < num_processes - i - 1; j++)
        {
            if (processes[j].arrival_time > processes[j + 1].arrival_time)
            {
                //SWAPPING
                Process temp = processes[j];
                processes[j] = processes[j+1];
                processes[j + 1] = temp;
            }
        }
    }
    printf("0");

    //CURRENT TIME @ GANTT CHART
    int currentTime = 0;
    //I
    int index=0;
    //FINISHED P'S
    int finished = 0;
    //WHEN THE CPU IS IDLE
    int idle=0;
    if( processes[0].arrival_time != 0  )
    {
        idle = processes[0].arrival_time;
        while(idle > 0 )
        {
            printf("|I\t%d",currentTime+1);
            idle--;
            currentTime++;
        }

    }

    //FINISH ALL THE P'S
    while(finished != num_processes)
    {

        int prio=INFINITY_INT;
        for(int i=0; i<num_processes; i++)
        {
            if( ( (processes[i].priority < prio) && (processes[i].arrival_time < currentTime) && (processes[i].flag == 1) ) || (processes[i].arrival_time == currentTime && processes[i].flag == 1) )
            {
                //ENTERS CPU
                prio = processes[i].priority;
            }
        }
        //IDLE CPU TIME
        int idle2 = 0; // Idle time counter
        if(prio == INFINITY_INT)
        {
            for(int j=0; j<num_processes; j++)
            {
                if(processes[j].flag == 1 && processes[j].arrival_time > currentTime)
                {
                    idle2 = processes[j].arrival_time - currentTime;
                    //printf("-%d-",currentTime);
                    while (idle2 > 0)
                    {
                        printf("|I\t%d",currentTime+1);
                        idle2--;
                        currentTime++;
                    }
                    break;

                }
            }
        }


        for(int i=0; i<num_processes; i++)
        {
            if( (processes[i].priority == prio) && (processes[i].flag == 1) )
            {
                processes[i].response_time = currentTime - processes[i].arrival_time;
                currentTime+=processes[i].CPU_burst;
                processes[i].flag = 0;
                finished++;
                printf("|p%d\t%d",processes[i].process_id,currentTime);
                processes[i].completion_time = currentTime;
                break;
            }
        }


        index++;
    }
    printf("|\n");
    finalTime1 = currentTime;
}

//PREEMPTIVE PRIORITY ALGO
int finalTime2=0;
void PreemptivePriorityAlgo(Process processes[], int num_processes)
{

    //SORTING THE P'S ACCORDING TO THEIR ARRIVAL TIMES
    for (int i = 0; i<num_processes-1; i++)
    {
        for (int j = 0; j < num_processes - i - 1; j++)
        {
            if (processes[j].arrival_time > processes[j + 1].arrival_time)
            {
                //SWAPPING
                Process temp = processes[j];
                processes[j] = processes[j+1];
                processes[j + 1] = temp;
            }
        }
    }

    //CURRENT TIME @ GANTT CHART
    int currentTime = 0;
    //I
    int i=0;
    //FINISHED P'S
    int finished = 0;
    //TO MAKE THE GANTT CHART MORE FANCY
    int GanttPrinting = -1;
    //WHEN THE CPU IS IDLE
    int idle=0;
    if( processes[0].arrival_time != 0  )
    {
        idle = processes[0].arrival_time;
        while(idle > 0 )
        {
            printf("%d|I\t",currentTime);
            idle--;
            currentTime++;
        }

    }
    //FINISH ALL THE P'S
    while(finished != num_processes)
    {
        int prio=INFINITY_INT;
        int concecutiveExec;
        for(int i=0; i<num_processes; i++)
        {
            if( ( (processes[i].priority < prio) && (processes[i].arrival_time <= currentTime) && (processes[i].flag == 1) ) )
            {
                //ENTERS CPU
                prio = processes[i].priority;
            }
        }
        //IDLE CPU TIME
        int idle2 = 0; // Idle time counter
        if(prio == INFINITY_INT)
        {
            for(int j=0; j<num_processes; j++)
            {
                if(processes[j].flag == 1 && processes[j].arrival_time > currentTime)
                {
                    idle2 = processes[j].arrival_time - currentTime;
                    //printf("-%d-",currentTime);
                    while (idle2 > 0)
                    {
                        printf("%d|I\t",currentTime);
                        idle2--;
                        currentTime++;
                    }
                    break;

                }
            }
        }
        for(int i=0; i<num_processes; i++)
        {
            if( (processes[i].priority == prio) && (processes[i].flag == 1) )
            {
                if(processes[i].CPU_burst == processes[i].CPU_burst_copy)
                {
                    processes[i].response_time = currentTime - processes[i].arrival_time;
                }
                currentTime++;
                processes[i].CPU_burst_copy--;
                processes[i].exec_previously++;
                if(GanttPrinting != i)
                {

                    printf("%d|p%d\t",currentTime-1,processes[i].process_id);
                    processes[i].enteredCPU++;
                    concecutiveExec = 0;
                }
                else
                {
                    printf(" ");
                    concecutiveExec++;
                }

                if(processes[i].CPU_burst_copy == 0)
                {
                    finished++;
                    processes[i].flag = 0;
                    processes[i].completion_time = currentTime;
                    processes[i].waiting_time = currentTime - concecutiveExec;
                    processes[i].exec_previously = processes[i].CPU_burst - concecutiveExec;
                }
                //printf("p%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",processes[i].process_id,processes[i].arrival_time,processes[i].CPU_burst,processes[i].priority,currentTime,(currentTime-processes[i].arrival_time),(currentTime-processes[i].arrival_time-processes[i].CPU_burst));
                GanttPrinting = i;
                break;
            }

        }
        if(finished == num_processes)
        {
            printf("|%d\n",currentTime);
        }
        i++;
    }
    finalTime2=currentTime;
}

int main()
{
    printf("Welcome to our CPU Scheduling Simulator\n\n");
    int l = 1;//variable used in loop to keep checking until user input is int
    char checknumber[30];
    int num_proc;
    while(l)
    {
        printf("Please enter number of processes (jobs) that will enter the CPU\n");
        scanf("%s", checknumber);
        if (!checkIfDigit(checknumber))//if the input is digit only change the variable to out from loop
            l = 0;
    }
    num_proc = atoi(checknumber);//ascii to int
    Process *arr_processes = (Process *)malloc(num_proc * sizeof(Process));//define array of processes that will enter the CPU simulator as malloc to change its size if needed
    for(int i = 0 ; i < num_proc ; i++)
    {
        int time_loop = 1;//same as above to check if user enter an int or not (handling errors by using this way)
        int time_loop1 = 1;
        char checktime[30];
        while(time_loop)
        {
            printf("Enter Arrival Time for process p%d\n", i);
            scanf("%s", checktime);
            if(!checkTime(checktime))// time can be zero or more (nothing else)
            {
                arr_processes[i].arrival_time = atoi(checktime);
                time_loop = 0;
            }
        }
        while(time_loop1)
        {
            printf("Enter amount of CPU burst for the process\n");
            scanf("%s", checktime);
            if(!checkIfDigit(checktime))
            {
                arr_processes[i].CPU_burst = atoi(checktime);
                time_loop1 = 0;
            }
        }
        arr_processes[i].process_id = i;//let the processes enter in sequence p0, p1, p2, ....
        arr_processes[i].flag = 1;//each process will enter CPU
        arr_processes[i].CPU_burst_copy = arr_processes[i].CPU_burst;//copy CPU burst time to do work without changing the basic one
    }

    int choice = 0;
    while(choice != 8)
    {
        int check_loop = 1;
        char checknumber[30];
        while(check_loop)//check the choice entered by user
        {
            printList();
            scanf("%s", checknumber);
            if (!checkIfDigit(checknumber))
                check_loop = 0;
        }
        check_loop = 1;
        choice = atoi(checknumber);
        if(choice == 1)
        {
            printf("If you want to update the processes please enter 1\n");
            int update_num = 0;
            scanf("%d", &update_num);
            if(update_num == 1)
            {
                printf("Please Enter the new number of processes that will enter CPU\n");
                scanf("%d", &num_proc);
                if(num_proc == 0)
                    printf("Number of processes must be greater than or equal one!\n");
                else
                {
                    arr_processes = realloc(arr_processes, num_proc * sizeof(Process));// the goal of this statement is to change the size of the array that containing processes
                    for(int i = 0 ; i < num_proc ; i++)
                    {
                        //two loop to check if user enter digit or not for handling any error
                        int arrival_loop = 1;
                        while(arrival_loop)
                        {
                            char checkarrival[30];
                            printf("Enter Arrival Time for process p%d\n", i);
                            scanf("%s", checkarrival);
                            if(!checkTime(checkarrival))// time can be zero or more (nothing else)
                            {
                                arr_processes[i].arrival_time = atoi(checkarrival);
                                arrival_loop = 0;
                            }
                        }
                        int burst_loop = 1;
                        while(burst_loop)
                        {
                            char checkburst[30];
                            printf("Enter amount of CPU burst for the process\n");
                            scanf("%s", checkburst);
                            if(!checkIfDigit(checkburst))// burst can be one or more (nothing else)
                            {
                                arr_processes[i].CPU_burst = atoi(checkburst);
                                burst_loop = 0;
                            }
                        }
                        arr_processes[i].process_id = i;//let the processes enter in sequence p0, p1, p2, ....
                        arr_processes[i].flag = 1;
                        arr_processes[i].CPU_burst_copy = arr_processes[i].CPU_burst;
                    }
                }
            }
        }
        else if(choice == 2)
        {
            int q;
            int quantum_loop = 1;
            while(quantum_loop)
            {
                char checkq[30];
                printf("Please enter the time quantum for the Round Rubin Algorithm\n");
                scanf("%s", checkq);
                if(!checkIfDigit(checkq))// burst can be one or more (nothing else)
                {
                    q = atoi(checkq);
                    quantum_loop = 0;
                }
            }
            RoundRubin(arr_processes, num_proc, q, 0, 1);
            int count_burst_time = 0, count_waiting = 0, count_turnaround = 0, count_response = 0;
            printf("\n-------------------------------------------------------\n");
            printf("For quantum = %d The behavior of processes is :\n", q);
            printf("Processes\tArrival T\tBurst T\t     Completion T\tWaiting T\tResponse T\tTurnaround T\n");
            for(int j = 0 ; j < num_proc ; j++)
            {

                printf("p%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", arr_processes[j].process_id, arr_processes[j].arrival_time, arr_processes[j].CPU_burst, arr_processes[j].completion_time, arr_processes[j].waiting_time, arr_processes[j].response_time, arr_processes[j].turaround_time);
                count_burst_time += arr_processes[j].CPU_burst;
                count_waiting += arr_processes[j].waiting_time;
                count_turnaround += arr_processes[j].turaround_time;
                count_response += arr_processes[j].response_time;
                arr_processes[j].CPU_burst_copy = arr_processes[j].CPU_burst;//recopy the value of CPU burst because RR algorithm decrement this value
            }
            int cpu_utilization = ((float)count_burst_time / t) * 100;//count CPU utilization as number of time that CPU work divide by over all time
            printf("*CPU Utilization = %d\t\t\t", cpu_utilization);
            printf("*Avg Waiting Time = %.3f\n", ((float)count_waiting / num_proc));//count the average waiting time (which calculated inside the algorithm)
            printf("*Avg Turnaround Time = %.3f\t\t", ((float)count_turnaround / num_proc));//count the average turnaround time
            printf("*Avg Response Time = %.3f\n", ((float)count_response / num_proc));//count the average response time
            printf("-------------------------------------------------------\n");
            for(int i = 0 ; i < num_proc ; i++)//loop to remove flag from each process (if algorithm run again the result is obtained from beginning)
                arr_processes[i].flag = 1;

        }
        else if(choice == 3)
        {
            printf("-------------------------------------------------\n");
            printf("This is Priority Algorithm (LESS NUMBER -> HIGHER PRIORITY)\n\n");
            for(int i=0; i<num_proc; i++)
            {
                printf("~ PLEASE ENTER THE PRIORITY FOR p%d:\n", i);
                int prio;
                scanf("%d", &prio);
                arr_processes[i].priority = prio;
            }
            printf("CHOOSE THE TYPE OF ALGO. (NONPREEMPTIVE/PREEMPTIVE):\n1.NONPREEMPTIVE\n2.PREEMPTIVE\n");
            int priorityType;
            scanf("%d", &priorityType);
            if(priorityType == 1)
            {
                printf("#THIS IS NON-PREEMPTIVE PRIORITY ALGO. GANTT CHART:\n");
                printf("----------------------------------------------------------------------------------------------------------------------\n");
                NonpreemptivePriorityAlgo(arr_processes,num_proc);
                printf("----------------------------------------------------------------------------------------------------------------------\n");
                printf("#THIS IS THE TABLE OF THE PROCESSES:\n");
                printf("Processes    Arrival Time     Burst Time       PRIORITY  Completion Time  Turnaround Time  Waiting Time  Response Time\n");
                int totalWT=0;
                int totalTT=0;
                int totalRT=0;
                int totalBT=0;
                for(int i=0; i<num_proc; i++)
                {
                    printf("p%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",arr_processes[i].process_id,arr_processes[i].arrival_time,arr_processes[i].CPU_burst,arr_processes[i].priority,arr_processes[i].completion_time,(arr_processes[i].completion_time-arr_processes[i].arrival_time),(arr_processes[i].completion_time-arr_processes[i].arrival_time-arr_processes[i].CPU_burst),arr_processes[i].response_time);
                    totalWT+=(arr_processes[i].completion_time-arr_processes[i].arrival_time-arr_processes[i].CPU_burst);
                    totalTT+=(arr_processes[i].completion_time-arr_processes[i].arrival_time);
                    totalRT+=arr_processes[i].response_time;
                    totalBT+=arr_processes[i].CPU_burst;
                }
                printf("#SOME CALCULATIONS:\n");
                printf("- AVG WAITING TIME = %f\n",((float)totalWT/num_proc));
                printf("- AVG TURNAROUND TIME = %f\n",((float)totalTT/num_proc));
                printf("- AVG RESPONSE TIME = %f\n",((float)totalRT/num_proc));
                printf("- CPU UTILIZATION = %f\n",((float)totalBT/finalTime1)*100);
                printf("- # OF TIMES EACH PROCESS ENTERED THE CPU: ONLY ONE NO PREEMPTION !!\n");
            }
            else if(priorityType == 2)
            {
                printf("#THIS IS PREEMPTIVE PRIORITY ALGO. GANTT CHART:\n");
                printf("----------------------------------------------------------------------------------------------------------------------\n");
                PreemptivePriorityAlgo(arr_processes,num_proc);
                printf("----------------------------------------------------------------------------------------------------------------------\n");
                printf("#THIS IS THE TABLE OF THE PROCESSES:\n");
                printf("Processes    Arrival Time     Burst Time       PRIORITY  Completion Time  Turnaround Time  Waiting Time  Response Time\n");
                int totalWT=0;
                int totalTT=0;
                int totalRT=0;
                int totalBT=0;
                for(int i=0; i<num_proc; i++)
                {
                    printf("p%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n",arr_processes[i].process_id,arr_processes[i].arrival_time,arr_processes[i].CPU_burst,arr_processes[i].priority,arr_processes[i].completion_time,(arr_processes[i].completion_time-arr_processes[i].arrival_time),(arr_processes[i].waiting_time-arr_processes[i].exec_previously-arr_processes[i].arrival_time),arr_processes[i].response_time);
                    totalWT+=(arr_processes[i].waiting_time-arr_processes[i].exec_previously-arr_processes[i].arrival_time);
                    totalTT+=(arr_processes[i].completion_time-arr_processes[i].arrival_time);
                    totalRT+=arr_processes[i].response_time;
                    totalBT+=arr_processes[i].CPU_burst;
                }
                printf("#SOME CALCULATIONS:\n");
                printf("- AVG WAITING TIME = %f\n",((float)totalWT/num_proc));
                printf("- AVG TURNAROUND TIME = %f\n",((float)totalTT/num_proc));
                printf("- AVG RESPONSE TIME = %f\n",((float)totalRT/num_proc));
                printf("- CPU UTILIZATION = %f\n",((float)totalBT/finalTime2)*100);
                printf("- # OF TIMES EACH PROCESS ENTERED THE CPU: ");
                for(int i=0; i<num_proc; i++)
                {
                    printf("p%d -> #%d\t",arr_processes[i].process_id,arr_processes[i].enteredCPU);
                }
                printf("\n");
            }
            else
            {
                printf("WRONG ENTERY!\n");
            }
            for(int i = 0 ; i < num_proc ; i++)
                arr_processes[i].flag = 1;
        }
        else if(choice == 4)
        {
            printf("-------------------------------------------------\n");
            printf("This is Shortest Job First (SJF) Algorithm\n\n");
            ShortestJobFirst(arr_processes, num_proc);

            int count_burst_time = 0, count_waiting = 0, count_turnaround = 0, count_response = 0;
            printf("-------------------------------------------------------\n");
            printf("The behavior of processes is :\n");
            printf("Processes\tArrival T\tBurst T\tCompletion T\tWaiting T\tResponse T\tTurnaround T\n");
            for(int j = 0 ; j < num_proc ; j++)
            {
                printf("p%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", arr_processes[j].process_id, arr_processes[j].arrival_time, arr_processes[j].CPU_burst, arr_processes[j].completion_time, arr_processes[j].waiting_time, arr_processes[j].response_time, arr_processes[j].turaround_time);
                count_burst_time += arr_processes[j].CPU_burst;
                count_waiting += arr_processes[j].waiting_time;
                count_turnaround += arr_processes[j].turaround_time;
                count_response += arr_processes[j].response_time;
            }
            printf("Avg waiting time = %.3f\n", ((float)count_waiting / num_proc));
            printf("Avg turnaround time = %.3f\n", ((float)count_turnaround / num_proc));
            printf("Avg response time = %.3f\n", ((float)count_response / num_proc));
            printf("-------------------------------------------------------\n");
            for(int i = 0 ; i < num_proc ; i++)
                arr_processes[i].flag = 1;
        }
        else if(choice == 5)
        {
            printf("-------------------------------------------------\n");
            printf("This is Shortest Remaining Time First (SRTF) Algorithm\n\n");
            ShortestRemainingTimeFirst(arr_processes, num_proc);

            int count_burst_time = 0, count_waiting = 0, count_turnaround = 0, count_response = 0;
            printf("-------------------------------------------------------\n");
            printf("The behavior of processes is :\n");
            printf("Processes\tArrival T\tBurst T\tCompletion T\tWaiting T\tResponse T\tTurnaround T\n");
            for(int j = 0 ; j < num_proc ; j++)
            {
                printf("p%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", arr_processes[j].process_id, arr_processes[j].arrival_time, arr_processes[j].CPU_burst, arr_processes[j].completion_time, arr_processes[j].waiting_time, arr_processes[j].response_time, arr_processes[j].turaround_time);
                count_burst_time += arr_processes[j].CPU_burst;
                count_waiting += arr_processes[j].waiting_time;
                count_turnaround += arr_processes[j].turaround_time;
                count_response += arr_processes[j].response_time;
            }
            printf("Avg waiting time = %.3f\n", ((float)count_waiting / num_proc));
            printf("Avg turnaround time = %.3f\n", ((float)count_turnaround / num_proc));
            printf("Avg response time = %.3f\n", ((float)count_response / num_proc));
            printf("-------------------------------------------------------\n");
            for(int i = 0 ; i < num_proc ; i++)
                arr_processes[i].flag = 1;
        }
        else if(choice == 6)
        {
            FirstComeFirstServed(arr_processes, num_proc, 0, 1);
            int count_burst_time = 0, count_waiting = 0, count_turnaround = 0, count_response = 0;
            printf("-------------------------------------------------------\n");
            printf("The behavior of processes is :\n");
            printf("Processes\tArrival T\tBurst T\t     Completion T\tWaiting T\tResponse T\tTurnaround T\n");
            for(int j = 0 ; j < num_proc ; j++)
            {

                printf("p%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\t\t%d\n", arr_processes[j].process_id, arr_processes[j].arrival_time, arr_processes[j].CPU_burst, arr_processes[j].completion_time, arr_processes[j].waiting_time, arr_processes[j].response_time, arr_processes[j].turaround_time);
                count_burst_time += arr_processes[j].CPU_burst;
                count_waiting += arr_processes[j].waiting_time;
                count_turnaround += arr_processes[j].turaround_time;
                count_response += arr_processes[j].response_time;
                arr_processes[j].CPU_burst_copy = arr_processes[j].CPU_burst;//recopy the value of CPU burst because RR algorithm decrement this value
            }
            int cpu_utilization = ((float)count_burst_time / t) * 100;
            printf("*CPU utilization = %d\t\t\t", cpu_utilization);
            printf("*Avg waiting time = %.3f\n", ((float)count_waiting / num_proc));
            printf("*Avg turnaround time = %.3f\t\t", ((float)count_turnaround / num_proc));
            printf("*Avg response time = %.3f\n", ((float)count_response / num_proc));
            printf("-------------------------------------------------------\n");
            for(int i = 0 ; i < num_proc ; i++)
                arr_processes[i].flag = 1;

        }
        else if(choice == 7)
        {
            printf("=========================================================\n");
            printf("Processes are :\n");
            for(int i = 0 ; i < num_proc ; i++)
            {
                printf("p%d\tArrival Time = %d\tBurst Time = %d\n", arr_processes[i].process_id, arr_processes[i].arrival_time, arr_processes[i].CPU_burst);
            }
            printf("=========================================================\n");
        }
        else if(choice == 8)
            printf("Welcome to Our CPU Simulation\n");
        else
            printf("You must choose only number from the list\n");
    }
    return 0;
}
