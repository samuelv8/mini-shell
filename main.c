#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

#define LINEMAX 1000 // max of chars
#define TOKENMAX 100 // max of words
#define READ_END 0
#define WRITE_END 1

const char *OWN_CMDS[2] = {"exit", "help"};

// initial print
void print_init();

// help command execution
void shell_help();

// exit command execution
void shell_exit();

// util function to get last char to be parsed
int int_min(int, int, int);

// function to read full line
bool read_input(char *);

// receives the full line and break it into tokens
int parse_input(char *, char **);

// gets pipe position in input if exists
int parse_pipe(char **, int, const char *);

// check if an inputed command is own
int check_own_cmd(char **);

// creates formatted array to execute
void set_argv(char **, char **, const int, const int);

// open input or output files
void set_file_red(int *, char **, int, int);

// executes a single program
void exec_cmd(char **, const int, const int, const int, const int);

// executes two programs with a pipe
void exec_cmd_piped(char **, int, int, int, int);

int main() {
    print_init();
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

        if (check_own_cmd(tokens)) continue;

        if (pipe > 0) {
            exec_cmd_piped(tokens, pipe, tokens_len, in, out);
        } else {
            exec_cmd(tokens, 0, int_min(in, out, tokens_len), in, out);
        }
    }

    return 0;
}

void print_init() {
    printf(
        "  //---------------------\\\\\n"
        " //-----------------------\\\\\n"
        "//----- V8 MINI-SHELL -----\\\\\n"
        "\\\\-------------------------//\n"
        " \\\\-----------------------//\n"
        "  \\\\---------------------//\n\n");
    printf("Welcome, user!\n");
    printf("Enter 'help' for guidance.\n");
}

void shell_help() {
    printf(
        "Having some trouble?\n"
        "This shell executes any executable file\n"
        "You can use one pipe '|' at max\n"
        "You can use file redirection too ('>' or '<')\n"
        "To close this shell, just type 'exit'\n"
        "\n\t\t\t - author: @samuelv8");
}

void shell_exit() {
    printf("Goodbye!\n");
    exit(0);
}

int int_min(int a, int b, int def) {
    return a <= b ? (a > 0 ? a : (b > 0 ? b : def)) : (b > 0 ? b : a);
}

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
        shell_exit();
    }
    if (strcmp(tok[0], OWN_CMDS[1]) == 0) {
        shell_help();
        return 1;
    }
    return 0;
}

void set_argv(char **argv, char **tok, const int start, const int end) {
    int i;
    for (i = start; i < end; i++) {
        argv[i - start] = tok[i];
    }
    argv[i - start] = NULL;
}

void set_file_red(int *flredr, char **tok, int in, int out) {
    if (in > 0) {
        if ((flredr[READ_END] = open(tok[in + 1], O_RDONLY)) == -1) {
            // file input fails
            printf("\nCan't open selected input file: %s", tok[in + 1]);
            perror("open");
            return;
        }
    }
    if (out > 0) {
        if ((flredr[WRITE_END] = open(tok[out + 1], O_WRONLY | O_CREAT)) == -1) {
            // file output fails
            printf("\nCan't open selected output file: %s", tok[out + 1]);
            perror("open");
            return;
        }
    }
}

void exec_cmd(char **tok, const int start, const int end, const int in, const int out) {
    pid_t pid;
    char *argv[TOKENMAX];
    set_argv(argv, tok, start, int_min(in, out, end));

    int flredr[2];
    set_file_red(flredr, tok, in, out);

    if ((pid = fork()) == -1) {
        // fork fails
        printf("\nFork failed..");
        perror("fork");
        return;
    }
    if (pid == 0) {
        // child process
        dup2(flredr[READ_END], STDIN_FILENO);
        close(flredr[READ_END]);
        dup2(flredr[WRITE_END], STDOUT_FILENO);
        close(flredr[WRITE_END]);

        if (execv(argv[0], argv) == -1) {
            printf("\nExecution failed...");
            perror("execv");
        }
        exit(0);
    } else {
        // parent process
        wait(NULL);
        close(flredr[READ_END]);
        close(flredr[WRITE_END]);
        return;
    }
}

void exec_cmd_piped(char **tok, int pipe_pos, int max_len, int in, int out) {
    pid_t pid1, pid2;
    char *argv1[TOKENMAX], *argv2[TOKENMAX];

    set_argv(argv1, tok, 0, (in < pipe_pos && in > 0 ? in : pipe_pos));
    set_argv(argv2, tok, pipe_pos + 1, (out < max_len && out > 0 ? out : max_len));

    int flredr[2];
    set_file_red(flredr, tok, in, out);

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        // pipe fails
        printf("\nFailed to create pipe.");
        perror("pipe");
        return;
    }
    if ((pid1 = fork()) == -1) {
        // fork fails
        printf("\nFork 1 failed..");
        perror("pipe");
        return;
    }
    if (pid1 == 0) {
        // child process 1
        dup2(flredr[READ_END], STDIN_FILENO);
        close(flredr[READ_END]);

        close(pipefd[READ_END]);
        dup2(pipefd[WRITE_END], STDOUT_FILENO);
        close(pipefd[WRITE_END]);

        if (execv(argv1[0], argv1) == -1) {
            printf("\nExecution of 1 failed...");
            perror("execv");
            exit(0);
        }
    } else {
        // parent process
        if ((pid2 = fork()) == -1) {
            // fork fails
            printf("\nFork 2 failed..");
            perror("fork");
            return;
        }
        if (pid2 == 0) {
            // child process 2
            dup2(flredr[WRITE_END], STDOUT_FILENO);
            close(flredr[WRITE_END]);

            close(pipefd[WRITE_END]);
            dup2(pipefd[READ_END], STDIN_FILENO);
            close(pipefd[READ_END]);

            if (execv(argv2[0], argv2) == -1) {
                printf("\nExecution of 2 failed...");
                perror("execv");
                exit(0);
            }
        } else {
            // parent process
            wait(NULL);
            wait(NULL);
            close(pipefd[WRITE_END]);
            close(pipefd[READ_END]);
            if (in > 0) close(flredr[READ_END]);
            if (out > 0) close(flredr[WRITE_END]);
        }
    }
}
