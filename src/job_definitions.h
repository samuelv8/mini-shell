#ifndef	_JOB_H
#define	_JOB_H

#include <termios.h>
#include <sys/types.h>

/* A process is a single process.  */
typedef struct process {
    struct process *next; /* next process in pipeline */
    char **argv;          /* for exec */
    pid_t pid;            /* process ID */
    char completed;       /* true if process has completed */
    char stopped;         /* true if process has stopped */
    int status;           /* reported status value */
} process;

/* A job is a pipeline of processes.  */
typedef struct job {
    struct job *next;          /* next active job */
    char *command;             /* command line, used for messages */
    process *first_process;    /* list of processes in this job */
    pid_t pgid;                /* process group ID */
    char notified;             /* true if user told about stopped job */
    struct termios tmodes;     /* saved terminal modes */
    int stdin, stdout, stderr; /* standard i/o channels */
} job;

/* The active jobs are linked into a list.  This is its head.   */
job *first_job;

/* Creates a process struct.  */
process *create_process();

/* Creates a job struct.  */
job *create_job();

/* Adds job J to jobs list.  */
void add_job(job *j);

/* Adds process P to job J.  */
void add_process_to_job(job *j, process *p);

/* Find the active job with the indicated pgid.  */
job *find_job(pid_t pgid);

/* Return true if all processes in the job have stopped or completed.  */
int job_is_stopped(job *j);

/* Return true if all processes in the job have completed.  */
int job_is_completed(job *j);

/* Start a created job in foreground/background.  */
void launch_job(job *j, int foreground);

#endif