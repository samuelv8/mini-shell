#include "shell.h"

#include "parsing.h"

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

int check_own_cmd(token_list *list) {
    if (strcmp(list->first_token->content, OWN_CMDS[0]) == 0) {
        shell_exit();
    }
    if (strcmp(list->first_token->content, OWN_CMDS[1]) == 0) {
        shell_help();
        return 1;
    }
    return 0;
}