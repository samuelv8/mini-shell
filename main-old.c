/* Keep track of attributes of the shell.  */

#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define LINEMAX 1000
#define TOKENMAX 100

const char *OWN_CMDS[2] = {"exit", "help"};

typedef struct int_queue_elm {
    struct int_queue_elm *next;
    struct int_queue_elm *prev;
    int value;
} int_queue_elm;

typedef struct int_queue {
    struct int_queue_elm *first;
    struct int_queue_elm *last;
    int length;
} int_queue;

void int_enqueue(int_queue *q, int x) {
    int_queue_elm *tmp;
    tmp = (int_queue_elm *)malloc(sizeof(int_queue_elm));
    tmp->next = NULL;
    tmp->prev = q->last;
    tmp->value = x;
    q->last->next = tmp;
    q->last = tmp;
    q->length += 1;
}

int int_dequeue(int_queue *q) {
    int_queue_elm *tmp = q->last;
    int x = tmp->value;
    tmp->prev->next = NULL;
    q->last = tmp->prev;
    q->length -= 1;
    free(tmp);
    return x;
}

// /* A process is a single process.  */
// typedef struct process {
//     struct process *next; /* next process in pipeline */
//     char **argv;          /* for exec */
//     pid_t pid;            /* process ID */
//     char completed;       /* true if process has completed */
//     char stopped;         /* true if process has stopped */
//     int status;           /* reported status value */
// } process;

// /* A job is a pipeline of processes.  */
// typedef struct job {
//     struct job *next;          /* next active job */
//     char *command;             /* command line, used for messages */
//     process *first_process;    /* list of processes in this job */
//     pid_t pgid;                /* process group ID */
//     char notified;             /* true if user told about stopped job */
//     struct termios tmodes;     /* saved terminal modes */
//     int stdin, stdout, stderr; /* standard i/o channels */
// } job;

// /* The active jobs are linked into a list.  This is its head.   */
// job *first_job = NULL;

// pid_t shell_pgid;
// struct termios shell_tmodes;
// int shell_terminal;
// int shell_is_interactive;

void print_init() {
    printf(
        "  //---------------------\\\\\n"
        " //-----------------------\\\\\n"
        "//----- V8 MINI-SHELL -----\\\\\n"
        "\\\\-------------------------//\n"
        " \\\\-----------------------//\n"
        "  \\\\---------------------//\n\n");
    printf("Welcome, user!\n");
}

void print_help() {
    printf(
        "Having some trouble?\n"
        "This shell accepts any linux cmd\n"
        "You can use one pipe '|' at max\n"
        "You can use file redirection too ('>' or '<')\n"
        "To close this shell, just type 'exit'\n"
        "\n\n\t\t\t\t - author: @samuelv8");
}

// /* Make sure the shell is running interactively as the foreground job
//    before proceeding. */
// void init_shell() {
//     /* See if we are running interactively.  */
//     shell_terminal = STDIN_FILENO;
//     shell_is_interactive = isatty(shell_terminal);

//     if (shell_is_interactive) {
//         /* Loop until we are in the foreground.  */
//         while (tcgetpgrp(shell_terminal) != (shell_pgid = getpgrp())) kill(-shell_pgid, SIGTTIN);

//         /* Ignore interactive and job-control signals.  */
//         // signal(SIGINT, SIG_IGN);
//         // signal(SIGQUIT, SIG_IGN);
//         // signal(SIGTSTP, SIG_IGN);
//         // signal(SIGTTIN, SIG_IGN);
//         // signal(SIGTTOU, SIG_IGN);
//         // signal(SIGCHLD, SIG_IGN);

//         /* Put ourselves in our own process group.  */
//         shell_pgid = getpid();
//         if (setpgid(shell_pgid, shell_pgid) < 0) {
//             perror("Couldn't put the shell in its own process group");
//             exit(1);
//         }

//         /* Grab control of the terminal.  */
//         tcsetpgrp(shell_terminal, shell_pgid);

//         /* Save default terminal attributes for shell.  */
//         tcgetattr(shell_terminal, &shell_tmodes);
//     }
//     printf("Shell initialized successfully.");
// }

// /* Find the active job with the indicated pgid.  */
// job *find_job(pid_t pgid) {
//     job *j;

//     for (j = first_job; j; j = j->next)
//         if (j->pgid == pgid) return j;
//     return NULL;
// }

// /* Return true if all processes in the job have stopped or completed.  */
// int job_is_stopped(job *j) {
//     process *p;

//     for (p = j->first_process; p; p = p->next)
//         if (!p->completed && !p->stopped) return 0;
//     return 1;
// }

// /* Return true if all processes in the job have completed.  */
// int job_is_completed(job *j) {
//     process *p;

//     for (p = j->first_process; p; p = p->next)
//         if (!p->completed) return 0;
//     return 1;
// }

bool read_input(char *inp) {
    char *tmp;
    tmp = readline("\n$");
    if (strlen(tmp) > 0) {
        add_history(tmp);
        strcpy(inp, tmp);
        free(tmp);
        return true;
    }
    return false;
}

int parse_input(char *inp, char **tok) {
    char *tmp;
    int i = 0;
    for (i = 0; i < TOKENMAX; i++) {
        tmp = (i == 0) ? strtok(inp, " ") : strtok(NULL, " ");
        if (tmp == NULL) break;
        tok[i] = tmp;
    }
    return i;
}

int parse_pipe(char **tok, int len, const char *fmt) {
    int i;
    for (i = 0; i < len; i++) {
        if (strcmp(tok[i], fmt) == 0) break;
    }
    return (i < len) ? i : -1;
}

int check_own_cmd(char **tok) {
    if (strcmp(tok[0], OWN_CMDS[0]) == 0) {
        printf("Goodbye!\n");
        exit(0);
    }
    if (strcmp(tok[0], OWN_CMDS[1]) == 0) {
        print_help();
        return 1;
    }
    return 0;
}

int main() {
    print_init();
    // init_shell();
    char input[LINEMAX];
    while (true) {
        if (!read_input(input)) continue;

        char *tokens[TOKENMAX];
        int tokens_len;
        tokens_len = parse_input(input, tokens);
        int pipe, in, out;
        pipe = parse_pipe(tokens, tokens_len, "|");
        in = parse_pipe(tokens, tokens_len, "<");
        out = parse_pipe(tokens, tokens_len, ">");
        if (pipe >= 0) {
            printf("pipe 1:\n");
            for (size_t i = 0; i < pipe; i++) {
                puts(tokens[i]);
            }
            printf("pipe 2:\n");
            for (size_t i = pipe + 1; i < tokens_len; i++) {
                puts(tokens[i]);
            }
        } else {
            printf("no pipe\n");
        }
        if (in >= 0) {
            printf("stdin: %s\n", tokens[in + 1]);
        }
        if (out >= 0) {
            printf("stdout: %s\n", tokens[out + 1]);
        }
        if (check_own_cmd(tokens)) continue;

        // process own cmd input

        // spawn processes

        // clean up
    }

    return 0;
}
