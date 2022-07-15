#include "job_control.h"
#include "job_definitions.h"
#include "parsing.h"
#include "shell.h"
#include "signal_handling.h"

#include <stdio.h>

int main() {
    init_shell();
    print_init();
    char *input;
    while (1) {
        /* Waits for input.  */
        if (!read_input(input)) continue;

        /* Parses input string to a token list.  */
        token_list *tokens;
        tokens = create_token_list();
        parse_input(input, tokens);

        /* Executes shell cmd.  */
        if (check_own_cmd(tokens)) continue;

        /* Sets file redirection, if necessary.  */
        token *in, *out;
        in = find_str(tokens, "<");
        out = find_str(tokens, ">");

        int infile = stdin, outfile = stdout;
        if (in != NULL) {
            infile = set_file_input(in->next->content);
        }
        if (out != NULL) {
            outfile = set_file_output(out->next->content);
        }

        /* Creates a new job with given tokens.  */
        job *current_job;
        current_job = create_job(infile, outfile, stderr);
        add_job(current_job);

        /* Finnaly, launches current job in foreground.  */
        launch_job(current_job, 1);

        /* Free alocated space that will be no longer used.  */
        delete_token_list(tokens);
        clean_job_list();

    }

    return 0;
}