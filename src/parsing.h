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

/* Creates a token list.  */
token_list *create_token_list(void);

/* Deletes a token list with its tokens.  */
void delete_token_list(token_list *l);

/* Reads full line.  */
int read_input(char *inp);

/* Receives the full line and break it into a token list.  */
void parse_input(char *inp, token_list *list);

/* Finds string fmt in token list,
   returns NULL if not found.         */
token *find_str(token_list *list, const char *fmt);

/* Fill job with processes in token list.  */
void fill_job(job *j, token_list *list);

/* Set input file descriptor.  */
int set_file_input(char *filepath);

/* Set output file descriptor.  */
int set_file_output(char *filepath);

#endif