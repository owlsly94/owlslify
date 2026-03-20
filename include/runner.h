#ifndef RUNNER_H
#define RUNNER_H

/* Run a shell command inside a live ncurses output panel.
   Streams stdout+stderr line by line into a scrollable window.
   Shows green ✔ or red ✘ when done.
   Returns the exit code of the command.                        */
int runner_exec(const char *title, const char *cmd);

#endif /* RUNNER_H */
