/*
    COMP3511 Fall 2023 
    PA2: Simplified Multi-Level Feedback Queue (MLFQ)

    Your name: Lam Leung Kin
    Your ITSC email: lklamad@connect.ust.hk 

    Declaration:

    I declare that I am not involved in plagiarism
    I understand that both parties (i.e., students providing the codes and students copying the codes) will receive 0 marks. 

*/

// Note: Necessary header files are included
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Define MAX_* constants
#define MAX_NUM_PROCESS 10
#define MAX_QUEUE_SIZE 10
#define MAX_PROCESS_NAME 5
#define MAX_GANTT_CHART 300

// Keywords (to be used when parsing the input)
#define KEYWORD_TQ0 "tq0"
#define KEYWORD_TQ1 "tq1"
#define KEYWORD_PROCESS_TABLE_SIZE "process_table_size"
#define KEYWORD_PROCESS_TABLE "process_table"

// Assume that we only need to support 2 types of space characters: 
// " " (space), "\t" (tab)
#define SPACE_CHARS " \t"

// Process data structure
// Helper functions:
//  process_init: initialize a process entry
//  process_table_print: Display the process table
struct Process {
    char name[MAX_PROCESS_NAME];
    int arrival_time ;
    int burst_time;
    int remain_time; // remain_time is needed in the intermediate steps of MLFQ 
};

void process_init(struct Process* p, char name[MAX_PROCESS_NAME], int arrival_time, int burst_time) {
    strcpy(p->name, name);
    p->arrival_time = arrival_time;
    p->burst_time = burst_time;
    p->remain_time = 0;
}

void process_table_print(struct Process* p, int size) {
    int i;
    printf("Process\tArrival\tBurst\n");
    for (i=0; i<size; i++) {
        printf("%s\t%d\t%d\n", p[i].name, p[i].arrival_time, p[i].burst_time);
    }
}

// A simple GanttChart structure
// Helper functions:
//   gantt_chart_append: append one item to the end of the chart (or update the last item if the new item is the same as the last item)
//   gantt_chart_print: display the current chart
struct GanttChartItem {
    char name[MAX_PROCESS_NAME];
    int duration;
};

void gantt_chart_update(struct GanttChartItem chart[MAX_GANTT_CHART], int* n, char name[MAX_PROCESS_NAME], int duration) {
    int i;
    i = *n;
    // The new item is the same as the last item
    if ( i > 0 && strcmp(chart[i-1].name, name) == 0) 
    {
        chart[i-1].duration += duration; // update duration
    } 
    else
    {
        strcpy(chart[i].name, name);
        chart[i].duration = duration;
        *n = i+1;
    }
}

void gantt_chart_print(struct GanttChartItem chart[MAX_GANTT_CHART], int n) {
    int t = 0;
    int i = 0;
    printf("Gantt Chart = ");
    printf("%d ", t);
    for (i=0; i<n; i++) {
        t = t + chart[i].duration;     
        printf("%s %d ", chart[i].name, t);
    }
    printf("\n");
}

// Global variables
int tq0 = 0, tq1 = 0;
int process_table_size = 0;
struct Process process_table[MAX_NUM_PROCESS];

// Helper function: Check whether the line is a blank line (for input parsing)
int is_blank(char *line) {
    char *ch = line;
    while ( *ch != '\0' ) {
        if ( !isspace(*ch) )
            return 0;
        ch++;
    }
    return 1;
}

// Helper function: Check whether the input line should be skipped
int is_skip(char *line) {
    if ( is_blank(line) )
        return 1;
    char *ch = line ;
    while ( *ch != '\0' ) {
        if ( !isspace(*ch) && *ch == '#')
            return 1;
        ch++;
    }
    return 0;
}

// Helper: parse_tokens function
void parse_tokens(char **argv, char *line, int *numTokens, char *delimiter) {
    int argc = 0;
    char *token = strtok(line, delimiter);
    while (token != NULL)
    {
        argv[argc++] = token;
        token = strtok(NULL, delimiter);
    }
    *numTokens = argc;
}

// Helper: parse the input file
void parse_input() {
    FILE *fp = stdin;
    char *line = NULL;
    ssize_t nread;
    size_t len = 0;
    char *two_tokens[2]; // buffer for 2 tokens
    int numTokens = 0, i=0;
    char equal_plus_spaces_delimiters[5] = "";
    char process_name[MAX_PROCESS_NAME];
    int process_arrival_time = 0;
    int process_burst_time = 0;

    strcpy(equal_plus_spaces_delimiters, "=");
    strcat(equal_plus_spaces_delimiters,SPACE_CHARS);

    while ( (nread = getline(&line, &len, fp)) != -1 ) {
        if ( is_skip(line) == 0)  {
            line = strtok(line,"\n");

            if (strstr(line, KEYWORD_TQ0)) {
                // parse tq0
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &tq0);
                }
            } 
            else if (strstr(line, KEYWORD_TQ1)) {
                // parse tq0
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &tq1);
                }
            }
            else if (strstr(line, KEYWORD_PROCESS_TABLE_SIZE)) {
                // parse process_table_size
                parse_tokens(two_tokens, line, &numTokens, equal_plus_spaces_delimiters);
                if (numTokens == 2) {
                    sscanf(two_tokens[1], "%d", &process_table_size);
                }
            } 
            else if (strstr(line, KEYWORD_PROCESS_TABLE)) {

                // parse process_table
                for (i=0; i<process_table_size; i++) {

                    getline(&line, &len, fp);
                    line = strtok(line,"\n");  

                    sscanf(line, "%s %d %d", process_name, &process_arrival_time, &process_burst_time);
                    process_init(&process_table[i], process_name, process_arrival_time, process_burst_time);

                }
            }

        }
        
    }
}

// Helper: Display the parsed values
void print_parsed_values() {
    printf("%s = %d\n", KEYWORD_TQ0, tq0);
    printf("%s = %d\n", KEYWORD_TQ1, tq1);
    printf("%s = \n", KEYWORD_PROCESS_TABLE);
    process_table_print(process_table, process_table_size);
}

// Helper: Insert process into the queue
void queueInsert(struct Process* queue, struct Process p, int* size) {
    int index = *size;
    queue[index] = p;
    *size = *size + 1;

    return;
}

// TODO: Implementation of MLFQ algorithm
void mlfq() {

    // Initialize the gantt chart
    struct GanttChartItem gantt_chart[MAX_GANTT_CHART];
    int chart_size = 0;

    // TODO: implement your MLFQ algorithm here
    // Tips: A simple array is good enough to implement a queue
    struct Process waitingQueue1[MAX_QUEUE_SIZE] = {0};
    struct Process waitingQueue2[MAX_QUEUE_SIZE] = {0};
    struct Process waitingQueue3[MAX_QUEUE_SIZE] = {0};

    int queue1Length = 0, queue2Length = 0, queue3Length = 0;

    int numOfCompletedProcess = 0;

    int currentQueueIndex = 1;
    int arrivedProcessIndex = -1;

    int retainedTimeQ1 = 0, retainedTimeQ2 = 0; // check how long a process has been in the queue

    for (int i = 0; i < process_table_size; i++) {
        process_table[i].remain_time = process_table[i].burst_time;
    }

    for (int i = 0; i < MAX_GANTT_CHART; i++) {
        if (numOfCompletedProcess == process_table_size)
            break;
        // printf("At time %d, the currentQueueIndex is: %d\n", i, currentQueueIndex);

        if (i > 0) {
            switch (currentQueueIndex) {                
                case 1: {
                    waitingQueue1[0].remain_time--;
                    retainedTimeQ1++;
                    // printf("At time %d Q1, Process name: %s has %d remain_time\n", i, waitingQueue1[0].name, waitingQueue1[0].remain_time);
                    gantt_chart_update(gantt_chart, &chart_size, waitingQueue1[0].name, 1);

                    if (waitingQueue1[0].remain_time == 0 || gantt_chart[chart_size - 1].duration % tq0 == 0 || retainedTimeQ1 == tq0) {                    
                        if (waitingQueue1[0].remain_time != 0)
                            queueInsert(waitingQueue2, waitingQueue1[0], &queue2Length);
                        else numOfCompletedProcess++;

                        retainedTimeQ1 = 0;

                        if (queue1Length == 1) {
                            currentQueueIndex = 2;             
                        } else {
                            for (int i = 0; i < queue1Length; i++) {
                                waitingQueue1[i] = waitingQueue1[i + 1];
                            }                            
                        }

                        queue1Length--;                  
                    }
                    break; 
                }
                case 2: {
                    waitingQueue2[0].remain_time--;
                    retainedTimeQ2++;
                    // printf("At time %d Q2, Process name: %s has %d remain_time\n", i, waitingQueue2[0].name, waitingQueue2[0].remain_time);
                    gantt_chart_update(gantt_chart, &chart_size, waitingQueue2[0].name, 1);
                    
                    if (waitingQueue2[0].remain_time == 0 || gantt_chart[chart_size - 1].duration % tq1 == 0 || retainedTimeQ2 == tq1) {                    
                        if (waitingQueue2[0].remain_time != 0)
                            queueInsert(waitingQueue3, waitingQueue2[0], &queue3Length);
                        else numOfCompletedProcess++;                                   

                        retainedTimeQ2 = 0;

                        if (queue2Length == 1) {
                            if (queue3Length == 0) {
                                currentQueueIndex = 1;        
                            } else {
                                currentQueueIndex = 3;
                            }
                        } 

                        for (int i = 0; i < queue2Length; i++) {
                            waitingQueue2[i] = waitingQueue2[i + 1];
                        }
                        queue2Length--;
                    }                    
                    break;
                }
                case 3: {
                    waitingQueue3[0].remain_time--;
                    // printf("At time %d Q3, Process name: %s has %d remain_time\n", i, waitingQueue3[0].name, waitingQueue3[0].remain_time);
                    gantt_chart_update(gantt_chart, &chart_size, waitingQueue3[0].name, 1);

                    if (waitingQueue3[0].remain_time == 0) {   
                        numOfCompletedProcess++;

                        if (queue3Length == 1) {
                            currentQueueIndex = 1;        
                        } 

                        for (int i = 0; i < queue3Length; i++) {
                            waitingQueue3[i] = waitingQueue3[i + 1];
                        }
                        queue3Length--;
                    }                      
                    break;
                }
            }
        }

        int index = arrivedProcessIndex + 1;
        if (i == process_table[index].arrival_time) {             
            // printf("New process arrived at Time %d\n", i);        
            arrivedProcessIndex++;
            currentQueueIndex = 1;        

            waitingQueue1[queue1Length] = process_table[arrivedProcessIndex];
            queue1Length++;
        }         
        
    }

    // At the end, display the final Gantt chart
    gantt_chart_print(gantt_chart, chart_size);
}


int main() {
    parse_input();
    print_parsed_values();
    mlfq();
    return 0;
}