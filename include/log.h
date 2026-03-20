#ifndef LOG_H
#define LOG_H

#include <stdio.h>

/* Opens a timestamped log file at ~/owlslify-<timestamp>.log
   All action_run_script / dotfiles output is tee'd to it.    */
void log_init(void);
void log_close(void);

/* Write a line to the log (printf-style) */
void log_write(const char *fmt, ...);

/* Path of the current log file (set after log_init) */
extern char g_log_path[512];

#endif /* LOG_H */
