#ifndef	_SIGNAL_HAND_H
#define	_SIGNAL_HAND_H

#include <termios.h>
#include <unistd.h>

pid_t shell_pgid;
struct termios shell_tmodes;
int shell_terminal;
int shell_is_interactive;

/* Make sure the shell is running interactively as the foreground job
   before proceeding. */
void init_shell();

#endif