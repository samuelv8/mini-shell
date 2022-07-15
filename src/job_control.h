#ifndef _JOB_CTRL_H
#define _JOB_CTRL_H

#include "job_definitions.h"

/* Put a job in the background.  If the cont argument is true, send
   the process group a SIGCONT signal to wake it up.  */
void put_job_in_background(job *j, int cont);

/* Put job j in the foreground.  If cont is nonzero,
   restore the saved terminal modes and send the process group a
   SIGCONT signal to wake it up before we block.  */
void put_job_in_foreground(job *j, int cont);

/* Check for processes that have status information available,
   blocking until all processes in the given job have reported.  */
void wait_for_job(job *j);

/* Format information about job status for the user to look at.  */
void format_job_info(job *j, const char *status);

/* Continue the job J.  */
void continue_job(job *j, int foreground);

/* Notify the user about stopped or terminated jobs.
   Delete terminated jobs from the active job list.  */
void do_job_notification(void);

#endif