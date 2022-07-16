#ifndef _SHELL_H
#define _SHELL_H

#include "parsing.h"

const char *OWN_CMDS[2] = {"exit", "help"};

// initial print
void print_init();

// help command execution
void shell_help();

// exit command execution
void shell_exit();

// check if an inputed command is own
int check_own_cmd(token_list *list);

#endif