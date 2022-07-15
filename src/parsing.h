#ifndef _PARSE_H
#define _PARSE_H

// util function to get last char to be parsed
int int_min(int, int, int);

// function to read full line
bool read_input(char *);

// receives the full line and break it into tokens
int parse_input(char *, char **);

// gets pipe position in input if exists
int parse_pipe(char **, int, const char *);

// creates formatted array to execute
void set_argv(char **, char **, const int, const int);

// open input or output files
void set_file_red(int *, char **, int, int);

#endif