#ifndef _SHELL_H
#define _SHELL_H

#include "parsing.h"

/* Initial shell print.  */
void print_init();

/* Executes help command.  */
void shell_help();

/* Executes exit command.  */
void shell_exit();

/* Checks if an inputed command is own.  */
int check_own_cmd(token_list *list);

#endif