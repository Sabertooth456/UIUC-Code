/**
 * savvy_scheduler
 * CS 341 - Spring 2024
 */
#include "libpriqueue/libpriqueue.h"
#include "libscheduler.h"

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "print_functions.h"

static size_t total_jobs;
static double total_wait_time;
static double total_turnaround_time;
static double total_response_time;

/**
 * The struct to hold the information about a given job
 */
typedef struct _job_info {
    int id;

    /* TODO: Add any other information and bookkeeping you need into this
     * struct. */
    double priority;    // The priority with which the job should be processed (Pre-emptive Priority, Priority)
    double arrival_time;   // Time the job was first initialized and added to the Priority Queue (First Come First Serve)
    double start_time;  // Time when the job was first processed (Reponse Time)
    double elap_time;   // Total time elapsed while the job is being processed (Pre-emptive Shortest Remaining Time First, Round Robin)
    double mod_time;    // The last intitial time the job was modified by a process (Round Robin)
    double run_time;    // The required time the job expects to run for (Shortest Job First)
} job_info;

void scheduler_start_up(scheme_t s) {
    switch (s) {
    case FCFS:
        comparision_func = comparer_fcfs;
        break;
    case PRI:
        comparision_func = comparer_pri;
        break;
    case PPRI:
        comparision_func = comparer_ppri;
        break;
    case PSRTF:
        comparision_func = comparer_psrtf;
        break;
    case RR:
        comparision_func = comparer_rr;
        break;
    case SJF:
        comparision_func = comparer_sjf;
        break;
    default:
        printf("Did not recognize scheme\n");
        exit(1);
    }
    priqueue_init(&pqueue, comparision_func);
    pqueue_scheme = s;
    // Put any additional set up code you may need here
    total_jobs = 0;
    total_wait_time = 0;
    total_turnaround_time = 0;
    total_response_time = 0;
}

static int break_tie(const void *a, const void *b) {
    return comparer_fcfs(a, b);
}

int comparer_fcfs(const void *a, const void *b) {
    // TODO: Implement me!
    // Whichever job arrived on the priority queue first has priority
    job_info *a_info = ((job *)a)->metadata;
    job_info *b_info = ((job *)b)->metadata;
    if (a_info->arrival_time < b_info->arrival_time) {
        return -1;
    } else if (a_info->arrival_time > b_info->arrival_time) {
        return 1;
    }
    return 0;
}

int comparer_ppri(const void *a, const void *b) {
    // Complete as is
    return comparer_pri(a, b);
}

int comparer_pri(const void *a, const void *b) {
    // TODO: Implement me!
    // Whicever job has a lower priority number has priority
    job_info *a_info = ((job *)a)->metadata;
    job_info *b_info = ((job *)b)->metadata;
    if (a_info->priority < b_info->priority) {
        return -1;
    } else if (a_info->priority > b_info->priority) {
        return 1;
    }
    return break_tie(a, b);
}

int comparer_psrtf(const void *a, const void *b) {
    // TODO: Implement me!
    // Whichever job has the shorter remaining time until completion has priority
    job_info *a_info = ((job *)a)->metadata;
    job_info *b_info = ((job *)b)->metadata;
    double a_rem = a_info->run_time - a_info->elap_time;
    double b_rem = b_info->run_time - b_info->elap_time;
    if (a_rem < b_rem) {
        return -1;
    } else if (a_rem > b_rem) {
        return 1;
    }
    return break_tie(a, b);
}

int comparer_rr(const void *a, const void *b) {
    // TODO: Implement me!
    // Whichever job has the lower elapsed time has priority
    job_info *a_info = ((job *)a)->metadata;
    job_info *b_info = ((job *)b)->metadata;
    if (a_info->elap_time < b_info->elap_time) {
        return -1;
    } else if (a_info->elap_time > b_info->elap_time) {
        return 1;
    }
    return break_tie(a, b);
}

int comparer_sjf(const void *a, const void *b) {
    // TODO: Implement me!
    // Whichever time has the lower total run time has priority
    job_info *a_info = ((job *)a)->metadata;
    job_info *b_info = ((job *)b)->metadata;
    if (a_info->run_time < b_info->run_time) {
        return -1;
    } else if (a_info->run_time > b_info->run_time) {
        return 1;
    }
    return break_tie(a, b);
}

// Do not allocate stack space or initialize ctx. These will be overwritten by
// gtgo
void scheduler_new_job(job *newjob, int job_number, double time,
                       scheduler_info *sched_data) {
    // TODO: Implement me!
    job_info *info = calloc(1, sizeof(job_info)); // Allocate memory on the heap for the job's information
    newjob->metadata = info;
    info->id = job_number;
    info->priority = sched_data->priority;
    info->arrival_time = time;
    info->start_time = -1;
    info->elap_time = 0.0;
    info->mod_time = time;
    info->run_time = sched_data->running_time;
    priqueue_offer(&pqueue, newjob);
}

job *scheduler_quantum_expired(job *job_evicted, double time) {
    // TODO: Implement me!
    // If the last running thread has finished or there were no threads previously running, job_evicted will be NULL.
    if (job_evicted == NULL && priqueue_peek(&pqueue) == NULL) {
        // If there are no waiting threads, return NULL.
        return NULL;
    }
    if (job_evicted != NULL) {
        // If job isn't NULL, change the job's last modification time
        job_info *old_info = job_evicted->metadata;
        old_info->elap_time += time - old_info->mod_time;   // Increase elapsed time equal to the time that passed between the job's last modification time, and the current time
        old_info->mod_time = time;  // Once the elapsed time is updated, set mod_time to the current time to avoid accidental double-counting
    }
    if ((pqueue_scheme != PPRI && pqueue_scheme != PSRTF && pqueue_scheme != RR) && job_evicted != NULL) {
        // If the current scheme is non-preemptive and job_evicted is not NULL, return job_evicted.
        return job_evicted;
    }
    // In all other cases, if the current scheme is preemptive and job_evicted is not NULL, place job_evicted back on the queue and return the next job that should be ran.
    if (job_evicted != NULL) {
        priqueue_offer(&pqueue, job_evicted);
    }
    job *new_job = priqueue_poll(&pqueue);
    job_info *new_info = new_job->metadata;
    if (new_info->start_time == -1) {
        // Initialize start_time if this is the job's first time being ran
        new_info->start_time = time;
    }
    // Initialize the last initial modification time to the current time
    new_info->mod_time = time;
    return new_job;
}

void scheduler_job_finished(job *job_done, double time) {
    // TODO: Implement me!
    job_info *j_info = job_done->metadata;
    total_jobs++;
    total_wait_time += time - j_info->arrival_time - j_info->run_time;
    total_turnaround_time += time - j_info->arrival_time;
    total_response_time += j_info->start_time - j_info->arrival_time;
    free(j_info);     // This should free both j_info and job_done->metadata
    j_info = NULL;
    job_done->metadata = NULL;
}

static void print_stats() {
    fprintf(stderr, "turnaround     %f\n", scheduler_average_turnaround_time());
    fprintf(stderr, "total_waiting  %f\n", scheduler_average_waiting_time());
    fprintf(stderr, "total_response %f\n", scheduler_average_response_time());
}

double scheduler_average_waiting_time() {
    // TODO: Implement me!
    if (total_jobs == 0) {
        return 0;
    }
    return total_wait_time / total_jobs;
}

double scheduler_average_turnaround_time() {
    // TODO: Implement me!
    if (total_jobs == 0) {
        return 0;
    }
    return total_turnaround_time / total_jobs;
}

double scheduler_average_response_time() {
    // TODO: Implement me!
    if (total_jobs == 0) {
        return 0;
    }
    return total_response_time / total_jobs;
}

void scheduler_show_queue() {
    // OPTIONAL: Implement this if you need it!
}

void print_job(job *j) {
    if (j == NULL) {
        printf("Job ID: NULL\n");
        return;
    }
    job_info *j_info = j->metadata;
    printf("Job ID: %i\n", j_info->id);
    printf("Job Priority: %f\n", j_info->priority);
    printf("Job Arrival Time: %f\n", j_info->arrival_time);
    printf("Job Start Time: %f\n", j_info->start_time);
    printf("Job Priority: %f\n", j_info->priority);
    printf("Job Elapsed Time: %f\n", j_info->elap_time);
    printf("Job Modification Time: %f\n", j_info->mod_time);
    printf("Job Run Time: %f\n", j_info->run_time);
}

void scheduler_clean_up() {
    priqueue_destroy(&pqueue);
    print_stats();
}
