#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_PROCESSES 100
#define MAX_POLICIES 20

typedef struct
{
    int id;
    int parameter;
} Policy;

typedef struct
{
    char name;
    int arrival;
    int value;
} Process;



int main(void){
    char line[MAX_LINE];
    char mode[20];

    Policy policies[MAX_PROCESSES];
    Process processes[MAX_PROCESSES];

    int policy_count = 0;
    int process_count = 0;
    int last_instant;

    if(fgets(mode, sizeof(mode), stdin) == NULL){
        fprintf(stderr, "Error: missing output mode.\n");
        return 1;
    }

    mode[strcspn(mode, "\r\n")] = '\0';

    if(strcmp(mode, "trace") != 0 && strcmp(mode, "stats") != 0){
        fprintf(stderr, "Error: invalid output mode.\n");
        return 1;
    }

    if(fgets(line, sizeof(line), stdin) == NULL){
        fprintf(stderr, "Error: missing policies.\n");
        return 1;
    }

    char *token = strtok(line, ",\r\n");

    while(token != NULL){
        int id;
        int parameter = 0;
        char *dash = strchr(token, '-');

        if(dash != NULL){
            *dash = '\0';
            parameter = atoi(dash + 1);
        }

        id = atoi(token);

        if(id < 1 || id > 8 || policy_count >= MAX_POLICIES){
            fprintf(stderr, "Error: invalid policy.\n");
            return 1;
        }

        policies[policy_count].id = id;
        policies[policy_count].parameter = parameter;
        policy_count++;

        token = strtok(NULL, ",\r\n");
    }

    if(fgets(line, sizeof(line), stdin) == NULL){
        fprintf(stderr, "Error: missing final instant.\n");
        return 1;
    }

    last_instant = atoi(line);

    if(fgets(line, sizeof(line), stdin) == NULL){
        fprintf(stderr, "Error: missing process count.\n");
        return 1;
    }

    process_count = atoi(line);

    if(process_count < 1 || process_count > MAX_PROCESSES){
        fprintf(stderr, "Error: invalid process count.\n");
        return 1;
    }

    for(int i = 0; i < process_count; i++){
        if(fgets(line, sizeof(line), stdin) == NULL){
            fprintf(stderr, "Error: missing process data.\n");
            return 1;
        }

        if(sscanf(line, " %c , %d , %d", &processes[i].name, &processes[i].arrival, &processes[i].value) != 3){
            fprintf(stderr, "Error: invalid process data.\n");
            return 1;
        }
    }

    fprintf(stderr, "Input parsed successfully.\n");
    fprintf(stderr, "Mode: %s\n", mode);
    fprintf(stderr, "Policies: %d\n", policy_count);
    fprintf(stderr, "Last instant: %d\n", last_instant);
    fprintf(stderr, "Processes: %d\n", process_count);

    for(int i = 0; i < policy_count; i++){
        fprintf(stderr, "Policy %d: ID=%d, parameter=%d\n", i + 1, policies[i].id, policies[i].parameter);
    }
    for(int i = 0; i < process_count; i++){
        fprintf(stderr, "Process %c: arrival=%d, value=%d\n", processes[i].name, processes[i].arrival, processes[i].value);
    }
    return 0;
}