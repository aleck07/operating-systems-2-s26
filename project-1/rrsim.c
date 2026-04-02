#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

#define MAX_PROG_LEN 12 // Max terms in a "program" (11 instructions + a terminator)
#define MAX_PROCS 10    // Max number of processes
#define QUANTUM 40      // Time quantum, ms

#define MIN(x, y) ((x) < (y) ? (x) : (y)) // Compute the minimum

int num_procs = 0; 

/**
 * Process information.
 */
struct process
{
    int pid;
    enum
    {
        READY,
        SLEEPING,
        EXITED
    } state;
    int time_awake_remaining;
    int time_asleep_remaining;
    int pc;
    int program[MAX_PROG_LEN];
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
    for (int i = 0; i < MAX_PROCS; i++)
    {
        table[i].pid = i;
        table[i].time_awake_remaining = 0;
        table[i].state = EXITED;
    }
}

/**
 *  @brief Parse the command line.
 */
void parse_command_line(int argc, char **argv)
{
    for (int i = 1; i < argc; i++)
    {
        struct process *p = &table[num_procs];
        int j = 0;
        char *token = strtok(argv[i], ",");
        while (token != NULL)
        {
            p->program[j++] = atoi(token);
            token = strtok(NULL, ",");
        }
        p->program[j] = 0; 

        p->state = READY;
        p->pc = 0;
        p->time_awake_remaining = p->program[0];
        num_procs++;
    }
}

/**
 * @brief Check if all jobs are done.
 * 
 * @return int 1 if all jobs are done, 0 otherwise
 */
int jobs_done()
{
    for (int i = 0; i < num_procs; i++)
    {
        if (table[i].state != EXITED)
        {
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
    int clock = 0;

    struct queue *q = queue_new();

    init_proc_table();
    parse_command_line(argc, argv);

    for (int i = 0; i < num_procs; i++)
        queue_enqueue(q, &table[i]);

    while(1){
        if (jobs_done()) {
            break;
        }
    }
    queue_free(q);
}
