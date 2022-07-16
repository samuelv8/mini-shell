#include "parsing.h"

#include <assert.h>
#include <fcntl.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

token_list *create_token_list() {
    token_list *l;
    l = (token_list *)malloc(sizeof(token_list));
    l->first_token = NULL;
    l->length = 0;
    return l;
}

token *create_token() {
    token *t;
    t = (token *)malloc(sizeof(token));
    t->content = "";
    t->next = NULL;
    return t;
}

void delete_token_list(token_list *l) {
    token *t, *next_t;

    t = l->first_token;
    while (t != NULL) {
        next_t = t->next;
        free(t);
        t = next_t;
    }
    free(l);
}

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

void parse_input(char *inp, token_list *list) {
    char *tmp;
    token *tok;

    tmp = strtok(inp, " ");
    list->first_token = create_token();
    tok = list->first_token;
    tok->content = tmp;

    int i;
    for (i = 1; tmp; tok = tok->next) {
        tmp = strtok(NULL, " ");
        if (tmp != NULL) {
            tok->next = create_token();
            tok->next->content = tmp;
            i++;
        }
    }
    list->length = i;
}

token *find_str(token_list *list, const char *fmt) {
    token *tok;

    tok = list->first_token;
    do {
        if (strcmp(tok->content, fmt) == 0) break;
        tok = tok->next;
    } while (tok != NULL);
    return tok;
}

token *find_proc_end(token *first) {
    token *t;

    t = first;
    while (t->next != NULL) {
        if (strcmp(t->next->content, "|") == 0 || strcmp(t->next->content, "<") == 0 ||
            strcmp(t->next->content, ">") == 0)
            break;
        t = t->next;
    }
    return t;
}

int find_distance(token *a, token *b) {
    token *t;
    int d;
    assert(a != NULL);
    assert(b != NULL);

    t = a;
    d = 0;
    while (strcmp(t->content, b->content) != 0) {
        if (t == NULL) return -1;
        t = t->next;
        d++;
    }
    return d;
}

char **get_argv(token *first_token, token *last_token) {
    token *t;
    int len;
    char **argv;

    len = find_distance(first_token, last_token);
    assert(len >= 0);
    len += 2;

    argv = (char **)malloc(len * sizeof(char *));
    t = first_token;
    for (int i = 0; i < len - 1; t = t->next) {
        argv[i++] = t->content;
    }
    argv[len - 1] = NULL;

    return argv;
}

void fill_job(job *j, token_list *list) {
    token *first_token, *last_token;
    process *p;
    char **argv;

    first_token = list->first_token;
    last_token = find_proc_end(first_token);
    argv = get_argv(first_token, last_token);
    p = create_process(argv);
    add_process_to_job(j, p);
    first_token = last_token->next;

    while (first_token != NULL) {
        first_token = first_token->next;
        last_token = find_proc_end(first_token);
        argv = get_argv(first_token, last_token);
        p = create_process(argv);
        add_process_to_job(j, p);
        first_token = last_token->next;
    }
}

int set_file_input(char *filepath) {
    int fd;

    fd = open(filepath, O_RDONLY, 0777);
    if (fd == -1) {
        perror("open");
        return STDIN_FILENO;
    }
    return fd;
}

int set_file_output(char *filepath) {
    int fd;

    fd = open(filepath, O_CREAT | O_WRONLY, 0777);
    if (fd == -1) {
        perror("open");
        return STDOUT_FILENO;
    }
    return fd;
}
