#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 256
#define MAX_PROCESSES 100
#define MAX_POLICIES 20
#define MAX_TIME 10000

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

void print_stats(Policy policy, const Process processes[], int process_count, const int finish[]){
    double total_turnaround = 0.0;
    double total_norm_turnaround = 0.0;

    if(policy.id == 1){
        printf("FCFS\n");
    }else if(policy.id == 2){
        printf("RR-%d\n", policy.parameter);
    }else if(policy.id == 3){
        printf("SPN\n");
    }else if(policy.id == 4){
        printf("SRT\n");
    }

    printf("Process    |");
    for(int i = 0; i < process_count; i++){
        printf("  %c  |", processes[i].name);
    }
    printf("\n");

    printf("Arrival    |");
    for(int i = 0; i < process_count; i++){
        printf(" %2d  |", processes[i].arrival);
    }
    printf("\n");

    printf("Service    |");
    for(int i = 0; i < process_count; i++){
        printf(" %2d  |", processes[i].value);
    }
    printf(" Mean|\n");

    printf("Finish     |");
    for(int i = 0; i < process_count; i++){
        printf(" %2d  |", finish[i]);
    }
    printf("-----|\n");

    printf("Turnaround |");
    for(int i = 0; i < process_count; i++){
        int turnaround = finish[i] - processes[i].arrival;

        total_turnaround += turnaround;

        printf(" %2d  |", turnaround);
    }

    printf("%5.2f|\n", total_turnaround / process_count);

    printf("NormTurn   |");

    for(int i = 0; i < process_count; i++){
        int turnaround = finish[i] - processes[i].arrival;

        double norm_turnaround = (double)turnaround / processes[i].value;

        total_norm_turnaround += norm_turnaround;

        printf(" %4.2f|", norm_turnaround);
    }
    printf(" %4.2f|\n", total_norm_turnaround / process_count);
    printf("\n");
}

void print_trace(Policy policy, const Process processes[], int process_count, int last_instant, const int running_at[], const int finish[]){
    if(policy.id == 1){
        printf("FCFS  ");
    }else if(policy.id == 2){
        printf("RR-%d  ", policy.parameter);
    }else if(policy.id == 3){
        printf("SPN   ");
    }else if(policy.id == 4){
        printf("SRT   ");
    }

    for (int t = 0; t <= last_instant; t++) {
        printf("%d ", t % 10);
    }

    printf("\n");
    printf("------------------------------------------------\n");

    for(int i = 0; i < process_count; i++){
        printf("%-6c", processes[i].name);
        for(int t = 0; t < last_instant; t++){
            char cell = ' ';

            if(t >= processes[i].arrival && (finish[i] == -1 || t < finish[i])){
                if(running_at[t] == i){
                    cell = '*';
                }else{
                    cell = '.';
                }
            }

            printf("|%c", cell);
        }
        printf("| \n");
    }

    printf("------------------------------------------------\n");
    printf("\n");
}

void sort_processes_by_arrival(const Process processes[], int process_count, int order[]){
    for(int i = 0; i < process_count; i++){
        order[i] = i;
    }
    
    for(int i = 0; i < process_count; i++){
        for(int j = i + 1; j < process_count; j++){
            if(processes[order[j]].arrival < processes[order[i]].arrival){
                int temp = order[i];
                order[i] = order[j];
                order[j] = temp;
            }
        }
    }
}

void init_sim(const Process processes[], int process_count, int last_instant, int start[], int finish[], int running_at[], int remaining[]){
    for(int t = 0; t < last_instant; t++){
        running_at[t] = -1;
    }
    
    for(int i = 0; i < process_count; i++){
        start[i] = -1;
        finish[i] = -1;
        remaining[i] = processes[i].value;
    }
}

void fcfs(const Process processes[], int process_count, int last_instant, const int order[], int start[], int finish[], int running_at[]){
    int current_time = 0;
    for(int i = 0; i < process_count; i++){
        int index = order[i];
        
        if(current_time < processes[index].arrival){
            current_time = processes[index].arrival;
        }

        start[index] = current_time;
        finish[index] = current_time + processes[index].value;

        for(int t = start[index]; t < finish[index] && t < last_instant; t++){
            running_at[t] = index;
        }

        current_time = finish[index];
    }
}

void rr(const Process processes[], int process_count, int last_instant, int quantum, int start[], int finish[], int running_at[], int remaining[]){
    int queue[MAX_PROCESSES];
    int front = 0;
    int rear = 0;
    int count = 0;

    int current = -1;
    int quantum_left = 0;
    int requeue = 0;

    if(quantum <= 0){
        fprintf(stderr, "Round Robin quantum must be positive.\n");
    }

    for(int t = 0; t < last_instant; t++){
        for(int i = 0; i < process_count; i++){
            if(processes[i].arrival == t){
                queue[rear] = i;
                rear = (rear + 1) % MAX_PROCESSES;
                count++;
            }
        }

        if(requeue){
            queue[rear] = current;
            rear = (rear + 1) % MAX_PROCESSES;
            count++;

            current = -1;
            requeue = 0;
        }

        if(current == -1 && count > 0){
            current = queue[front];
            front = (front + 1) % MAX_PROCESSES;
            count--;

            quantum_left = quantum;
        }

        if(current != -1){
            int i = current;

            if(start[i] == -1){
                start[i] = t;
            }

            running_at[t] = i;
            remaining[i]--;
            quantum_left--;

            if(remaining[i] == 0){
                finish[i] = t + 1;
                current = -1;
            }else if(quantum_left == 0){
                requeue = 1;
            }
        }
    }
}

void spn(const Process processes[], int process_count, int last_instant, int start[], int finish[], int running_at[], int remaining[]){
    int current = -1;
    for(int t = 0; t < last_instant; t++){
        if(current == -1){
            int shortest = -1;

            for(int i = 0; i < process_count; i++){
                if(processes[i].arrival <= t && remaining[i] > 0){
                    if(shortest == -1 || processes[i].value < processes[shortest]. value){
                        shortest = i;
                    }
                }
            }

            current = shortest;

            if(current != -1 && start[current] == -1){
                start[current] = t;
            }
        }

        if(current != -1){
            running_at[t] = current;

            remaining[current]--;

            if(remaining[current] == 0){
                finish[current] = t + 1;
                current = -1;
            }
        }

    }
}

void srt(const Process processes[], int process_count, int last_instant, int start[], int finish[], int running_at[], int remaining[]){
    int current = -1;

    for(int t = 0; t < last_instant; t++){
        int shortest = -1;

        for(int i = 0; i < process_count; i++){
            if(processes[i].arrival <= t && remaining[i] > 0){
                if(shortest == -1 || remaining[i] < remaining[shortest]){
                    shortest = i;
                }
            }
        }

        if(shortest == -1){
            current = -1;
            continue;
        }

        if(current == -1 || remaining[current] <= 0 || remaining[shortest] < remaining[current]){
            current = shortest;
        }

        if(start[current] == -1){
            start[current] = t;
        }

        running_at[t] = current;
        remaining[current]--;

        if(remaining[current] == 0){
            finish[current] = t + 1;
            current = -1;
        }
    }
}

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
    if(last_instant < 0 || last_instant > MAX_TIME){
        fprintf(stderr, "Error: simulation limit is invalid or too large.\n");
        return 1;
    }

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

        if(processes[i].arrival < 0 || processes[i].value < 0){
            fprintf(stderr, "Error: negative process value.\n");
            return 1;
        }
    }

    //test
    // fprintf(stderr, "Input parsed successfully.\n");
    // fprintf(stderr, "Mode: %s\n", mode);
    // fprintf(stderr, "Policies: %d\n", policy_count);
    // fprintf(stderr, "Last instant: %d\n", last_instant);
    // fprintf(stderr, "Processes: %d\n", process_count);

    // for(int i = 0; i < policy_count; i++){
    //     fprintf(stderr, "Policy %d: ID=%d, parameter=%d\n", i + 1, policies[i].id, policies[i].parameter);
    // }
    // for(int i = 0; i < process_count; i++){
    //     fprintf(stderr, "Process %c: arrival=%d, value=%d\n", processes[i].name, processes[i].arrival, processes[i].value);
    // }

    // if(strcmp(mode, "trace") != 0){
    //     fprintf(stderr, "Statistics mode is not implemented yet.\n");
    //     return 1;
    // }

    for(int p = 0; p < policy_count; p++){
        if(policies[p].id < 1 || policies[p].id > 4){
            fprintf(stderr, "Only FCFS, Round Robin, SPN, and SRT are implemented so far.\n");
            return 1;
        }

        int start[MAX_PROCESSES];
        int finish[MAX_PROCESSES];
        int order[MAX_PROCESSES];

        int running_at[MAX_TIME];
        int remaining[MAX_PROCESSES];

        sort_processes_by_arrival(processes, process_count, order);
        init_sim(processes, process_count, last_instant, start, finish, running_at, remaining);
        
        if(policies[p].id == 1){
            fcfs(processes, process_count, last_instant, order, start, finish, running_at);
        }else if(policies[p].id == 2){
            rr(processes, process_count, last_instant, policies[p].parameter, start, finish, running_at, remaining);
        }else if(policies[p].id == 3){
            spn(processes, process_count, last_instant, start, finish, running_at, remaining);
        }else if(policies[p].id == 4){
            srt(processes, process_count, last_instant, start, finish, running_at, remaining);
        }

        if(strcmp(mode, "stats") == 0){
            print_stats(policies[p], processes, process_count, finish);
        }else{
            print_trace(policies[p], processes, process_count, last_instant, running_at, finish);
        }
    }
    return 0;
}