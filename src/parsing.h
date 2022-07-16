#ifndef _PARSE_H
#define _PARSE_H

#include "job_definitions.h"

typedef struct token {
    char *content;
    struct token *next;
} token;

typedef struct {
    token *first_token;
    int length;
} token_list;

token_list *create_token_list(void);

void delete_token_list(token_list *l);

// util function to get last char to be parsed
int int_min(int, int, int);

// function to read full line
int read_input(char *);

// receives the full line and break it into tokens
void parse_input(char *inp, token_list *list);

/* Finds string fmt in token list,
   returns NULL if not found.         */
token *find_str(token_list *list, const char *fmt);

void fill_job(job *j, token_list *list);

int set_file_input(char *filepath);

int set_file_output(char *filepath);

#endif