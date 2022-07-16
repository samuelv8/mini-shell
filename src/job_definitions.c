
#include "job_definitions.h"

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include "job_control.h"
#include "signal_handling.h"

job *first_job = NULL;
int jobs_len = 1;

process *create_process(char **argv) {
    process *p = (process *)malloc(sizeof(process));
    p->next = NULL;
    p->argv = argv;
    p->completed = 0;
    p->stopped = 0;
    return p;
}

job *create_job(int infile, int outfile, int errfile) {
    job *j = (job *)malloc(sizeof(job));
    j->next = NULL;
    j->first_process = NULL;
    j->stdin = infile;
    j->stdout = outfile;
    j->stderr = errfile;
    j->notified = 0;
    j->command = NULL;
    j->pgid = jobs_len;
    return j;
}

void delete_job(job *j) {
    process *p, *next_p;

    p = j->first_process;
    while (p != NULL) {
        next_p = p->next;
        free(p);
        p = next_p;
    }
    free(j);
}

void clean_job_list() {
    job *j, *next_j, *prev_j;

    j = first_job;
    prev_j = NULL;
    while (j != NULL) {
        /* Delete completed jobs.  */
        if (job_is_completed(j)) {
            next_j = j->next;
            delete_job(j);
            if (prev_j) {
                prev_j->next = next_j;
            } else {
                first_job = next_j;
            }
            j = next_j;
        } else {
            prev_j = j;
            j = j->next;
        }
    }
}

void add_job(job *new_j) {
    job *j;

    for (j = first_job; j && j->next; j = j->next)
        ;
    if (j) {
        j->next = new_j;
    } else {
        first_job = new_j;
    }
    jobs_len++;
}

void add_process_to_job(job *j, process *new_p) {
    process *p;

    for (p = j->first_process; p && p->next; p = p->next)
        ;
    if (p == NULL) {
        j->first_process = new_p;
    } else {
        p->next = new_p;
    }
}

job *find_job(pid_t pgid) {
    job *j;

    for (j = first_job; j; j = j->next)
        if (j->pgid == pgid) return j;
    return NULL;
}

int job_is_stopped(job *j) {
    process *p;

    for (p = j->first_process; p; p = p->next)
        if (!p->completed && !p->stopped) return 0;
    return 1;
}

int job_is_completed(job *j) {
    process *p;

    for (p = j->first_process; p; p = p->next)
        if (!p->completed) return 0;
    return 1;
}

void launch_process(process *p, pid_t pgid, int infile, int outfile, int errfile, int foreground) {
    pid_t pid;

    if (shell_is_interactive) {
        /* Put the process into the process group and give the process group
           the terminal, if appropriate.
           This has to be done both by the shell and in the individual
           child processes because of potential race conditions.  */
        pid = getpid();
        if (pgid == 0) pgid = pid;
        setpgid(pid, pgid);
        if (foreground) tcsetpgrp(shell_terminal, pgid);

        /* Set the handling for job control signals back to the default.  */
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);
    }

    /* Set the standard input/output channels of the new process.  */
    if (infile != STDIN_FILENO) {
        dup2(infile, STDIN_FILENO);
        close(infile);
    }
    if (outfile != STDOUT_FILENO) {
        dup2(outfile, STDOUT_FILENO);
        close(outfile);
    }
    if (errfile != STDERR_FILENO) {
        dup2(errfile, STDERR_FILENO);
        close(errfile);
    }

    /* Exec the new process.  Make sure we exit.  */
    execv(p->argv[0], p->argv);
    perror("execv");
    exit(1);
}

void launch_job(job *j, int foreground) {
    process *p;
    pid_t pid;
    int mypipe[2], infile, outfile;

    infile = j->stdin;
    for (p = j->first_process; p; p = p->next) {
        /* Set up pipes, if necessary.  */
        if (p->next) {
            if (pipe(mypipe) < 0) {
                perror("pipe");
                exit(1);
            }
            outfile = mypipe[1];
        } else
            outfile = j->stdout;

        /* Fork the child processes.  */
        pid = fork();
        if (pid == 0) /* This is the child process.  */
            launch_process(p, j->pgid, infile, outfile, j->stderr, foreground);
        else if (pid < 0) {
            /* The fork failed.  */
            perror("fork");
            exit(1);
        } else {
            /* This is the parent process.  */
            p->pid = pid;
            if (shell_is_interactive) {
                if (!j->pgid) j->pgid = pid;
                setpgid(pid, j->pgid);
            }
        }

        /* Clean up after pipes.  */
        if (infile != j->stdin) close(infile);
        if (outfile != j->stdout) close(outfile);
        infile = mypipe[0];
    }

    format_job_info(j, "launched");

    if (!shell_is_interactive)
        wait_for_job(j);
    else if (foreground)
        put_job_in_foreground(j, 0);
    else
        put_job_in_background(j, 0);
}
