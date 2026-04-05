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
 * @brief Subtract elapsed time from all sleeping processes.
 */
void decrement_sleeping(int elapsed)
{
    for (int i = 0; i < num_procs; i++)
        if (table[i].state == SLEEPING)
            table[i].time_asleep_remaining -= elapsed;
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
 * @brief Main function.
 * 
 * @param argc Argument count
 * @param argv Argument vector
 * @return int Exit status
 */
int main(int argc, char **argv)
{
    int clock = 0;

    struct queue *q = queue_new();

    init_proc_table();
    parse_command_line(argc, argv);

    for (int i = 0; i < num_procs; i++)
        queue_enqueue(q, &table[i]);

    while (1)
    {
        if (jobs_done())
            break;

        if (queue_is_empty(q))
        {
            int min_sleep = -1;
            for (int i = 0; i < num_procs; i++)
            {
                if (table[i].state == SLEEPING)
                {
                    if (min_sleep < 0 || table[i].time_asleep_remaining < min_sleep)
                        min_sleep = table[i].time_asleep_remaining;
                }
            }
            if (min_sleep > 0)
            {
                clock += min_sleep;
                decrement_sleeping(min_sleep);
            }
        }

        printf("=== Clock %d ms ===\n", clock);

        for (int i = 0; i < num_procs; i++)
        {
            if (table[i].state == SLEEPING && table[i].time_asleep_remaining <= 0)
            {
                table[i].pc++;
                if (table[i].program[table[i].pc] == 0)
                {
                    table[i].state = EXITED;
                    printf("PID %d: Exiting\n", table[i].pid);
                }
                else
                {
                    table[i].time_awake_remaining = table[i].program[table[i].pc];
                    table[i].state = READY;
                    printf("PID %d: Waking up for %d ms\n", table[i].pid, table[i].time_awake_remaining);
                    queue_enqueue(q, &table[i]);
                }
            }
        }

        struct process *p = queue_dequeue(q);
        if (p == NULL)
            continue;

        printf("PID %d: Running\n", p->pid);

        int time_slice = MIN(QUANTUM, p->time_awake_remaining);
        clock += time_slice;
        p->time_awake_remaining -= time_slice;
        decrement_sleeping(time_slice);

        if (p->time_awake_remaining > 0)
        {
            queue_enqueue(q, p);
        }
        else
        {
            p->pc++;
            if (p->program[p->pc] == 0)
            {
                p->state = EXITED;
                printf("PID %d: Exiting\n", p->pid);
            }
            else
            {
                p->time_asleep_remaining = p->program[p->pc];
                p->state = SLEEPING;
                printf("PID %d: Sleeping for %d ms\n", p->pid, p->time_asleep_remaining);
            }
        }

        printf("PID %d: Ran for %d ms\n", p->pid, time_slice);
    }
    queue_free(q);
}
