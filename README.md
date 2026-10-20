# CPU Scheduling Policies
A C implementation of the CPU scheduling policies.

## Implemented Policies

| Policy ID | Policy | Description |
|---:|---|---|
| `1` | FCFS — First Come, First Served | Runs processes in arrival order. |
| `2` | RR — Round Robin | Shares CPU time using a configurable time quantum. |
| `3` | SPN — Shortest Process Next | Selects the ready process with the shortest service time; non-preemptive. |
| `4` | SRT — Shortest Remaining Time | Selects the ready process with the shortest remaining service time; preemptive. |
| `5` | HRRN — Highest Response Ratio Next | Selects the ready process with the highest response ratio; non-preemptive. |
| `6` | FB-1 — Feedback | Uses feedback priority levels with a quantum of 1. |
| `7` | FB-2i — Feedback with Increasing Quantums | Uses feedback priority levels whose quantums increase by powers of two. |
| `8` | Aging | Updates process priorities over time to help prevent starvation. Uses a configurable scheduling quantum. |


## Requirements

- A C compiler such as GCC
- `make` to build using the included Makefile

## Build

From the project directory, run:

```bash
make
```

The assignment expects the Makefile to produce an executable named `main` in the project directory.

If you want to compile the source directly with GCC, use your source filename in place of `main.c` if it is different:

```bash
gcc main.c -o main -Wall -Wextra -Wpedantic
```

## Run

The program reads input from standard input. You can redirect an input file into it:

```bash
./main < input.txt
```

On Windows, if your build produces `main.exe`, use:

```powershell
.\main.exe < input.txt
```

## Input Format

Input is supplied in this order:

1. Output mode: `trace` or `stats`
2. A comma-separated list of policy IDs, optionally with parameters
3. The last simulation instant
4. Number of processes
5. One process description per line

For policies **1–7**, each process line contains:

```text
process_name,arrival_time,service_time
```

For **Aging (policy 8)**, the third field represents the process's **initial priority**, not its service time:

```text
process_name,arrival_time,initial_priority
```

### Policy parameters

Round Robin and Aging accept a quantum parameter separated from the policy ID by a hyphen:

- `2-4`: Round Robin with quantum `4`
- `8-1`: Aging with quantum `1`

Multiple policies can be requested in the same run by separating them with commas, for example `1,2-1,3,4`.


## Output Modes

### `trace`

Prints a timeline of the simulation. An asterisk (`*`) marks the process running during a time interval, and a period (`.`) marks a process that has arrived and is ready but is not running during that interval. Empty cells represent intervals when the process has not arrived yet or is no longer waiting/running.

### `stats`

Prints a table with process information and scheduling statistics, including finish time, turnaround time, normalized turnaround time, and their means where applicable.
