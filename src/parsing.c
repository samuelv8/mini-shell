#include <fcntl.h>

#include <readline/history.h>
#include <readline/readline.h>

int int_min(int a, int b, int def) {
    return a <= b ? (a > 0 ? a : (b > 0 ? b : def)) : (b > 0 ? b : a);
}

int read_input(char *inp) {
    char *tmp;
    tmp = readline("\n$");
    if (strlen(tmp) > 0) {
        add_history(tmp);
        strcpy(inp, tmp);
        free(tmp);
        return 1;
    }
    return 0;
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



void set_argv(char **argv, char **tok, const int start, const int end) {
    int i;
    for (i = start; i < end; i++) {
        argv[i - start] = tok[i];
    }
    argv[i - start] = NULL;
}

void set_file_red(int *flredr, char **tok, int in, int out) {
    if (in > 0) {
        if ((flredr[0] = open(tok[in + 1], O_RDONLY)) == -1) {
            // file input fails
            printf("\nCan't open selected input file: %s", tok[in + 1]);
            perror("open");
            return;
        }
    }
    if (out > 0) {
        if ((flredr[1] = open(tok[out + 1], O_WRONLY | O_CREAT)) == -1) {
            // file output fails
            printf("\nCan't open selected output file: %s", tok[out + 1]);
            perror("open");
            return;
        }
    }
}