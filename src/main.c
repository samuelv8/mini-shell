#include "job_control.h"
#include "job_definitions.h"
#include "parsing.h"
#include "shell.h"
#include "signal_handling.h"

int main() {
    print_init();
    char *input;
    while (1) {
        if (!read_input(input)) continue;

        char **tokens;
        int tokens_len;
        tokens_len = parse_input(input, tokens);

        int pipe, in, out;
        pipe = parse_pipe(tokens, tokens_len, "|");
        in = parse_pipe(tokens, tokens_len, "<");
        out = parse_pipe(tokens, tokens_len, ">");

        if (check_own_cmd(tokens)) continue;

        if (pipe > 0) {
            exec_cmd_piped(tokens, pipe, tokens_len, in, out);
        } else {
            exec_cmd(tokens, 0, int_min(in, out, tokens_len), in, out);
        }
    }

    return 0;
}