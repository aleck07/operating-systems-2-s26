#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define MAX_PROCS 10 // Max number of processes
#define QUANTUM 40 // Time quantum, ms
#define MIN(x,y) ((x)<(y)?(x):(y)) // Compute the minimum

/**
 * Process information.
 */
struct process {
    int pid;
    int time_awake_remaining;
};

/**
 * The process table.
 */
struct process table[MAX_PROCS];

/**
 * Initialize the process table.
 */
void init_proc_table(void)
{
    for (int i = 0; i < MAX_PROCS; i++) {
        table[i].pid = i;
        table[i].time_awake_remaining = 0;
    }
}

/**
 * Parse the command line.
 */
void parse_command_line(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        char *arg = argv[i];
        table[i-1].pid = i-1;
        table[i-1].time_awake_remaining = atoi(arg);
    }
}

/**
 * Check if all jobs are done.
 */
int jobs_done()
{
    for (int i = 0; i < MAX_PROCS; i++) {
        if (table[i].time_awake_remaining > 0) {
            return 0;
        }
    }
    return 1;
}

/**
 * Main.
 */
int main(int argc, char **argv)
{
    struct queue *q = queue_new();

    int clock = 0;

    init_proc_table();

    parse_command_line(argc, argv);

    for (int i = 1; i < argc; i++) {
        queue_enqueue(q, table + (i-1));
    }

    while(1){
        if (jobs_done()) {
            break;
        }
        printf("=== Clock %d ms ===\n", clock);
        struct process *p = queue_dequeue(q);
        printf("PID %d: Running\n", p->pid);
        int time_ran = MIN(QUANTUM, p->time_awake_remaining);
        clock += time_ran;
        p->time_awake_remaining -= time_ran;
        if (p->time_awake_remaining > 0) {
            queue_enqueue(q, p);
        }
        printf("PID %d: Ran for %d ms\n", p->pid, time_ran);
    }
    queue_free(q);
}
