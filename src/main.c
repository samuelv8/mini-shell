#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "job_control.h"
#include "job_definitions.h"
#include "parsing.h"
#include "shell.h"
#include "signal_handling.h"

/* Define max size due to seg fault problems.  */
#define MAX_INPUT_SIZE 1000

int main() {
    init_shell();
    print_init();
    char *input = (char *)malloc(MAX_INPUT_SIZE * sizeof(char));
    char *original_input = (char *)malloc(MAX_INPUT_SIZE * sizeof(char));
    while (1) {
        /* Waits for input.  */
        if (!read_input(input)) continue;

        /* Parses input string to a token list.  */
        token_list *tokens;
        tokens = create_token_list();
        strcpy(original_input, input);
        parse_input(input, tokens);

        /* Executes shell cmd.  */
        if (check_own_cmd(tokens)) continue;

        /* Sets file redirection, if necessary.  */
        token *in, *out;
        in = find_str(tokens, "<");
        out = find_str(tokens, ">");

        int infile = STDIN_FILENO, outfile = STDOUT_FILENO;
        if (in != NULL) {
            infile = set_file_input(in->next->content);
        }
        if (out != NULL) {
            outfile = set_file_output(out->next->content);
        }

        /* Creates a new job with given tokens.  */
        job *current_job;
        current_job = create_job(infile, outfile, STDERR_FILENO);
        current_job->command = original_input;
        fill_job(current_job, tokens);
        add_job(current_job);

        /* Finnaly, launches current job in foreground.  */
        launch_job(current_job, 1);

        /* Free alocated space that will be no longer used and updates job list.  */
        delete_token_list(tokens);
        do_job_notification();
    }

    return 0;
}