#ifndef ACTIONS_H
#define ACTIONS_H

#include "menu.h"

/* Run a shell command, suspending ncurses so output is visible */
int  action_run_script(const char *cmd);

/* Pull ~/.config/<app> from owlsly94/dotfiles via npx degit
   Backs up existing dir to ~/.config/<app>.bak.<timestamp>    */
int  action_pull_dotfiles(const char *app);

/* Pull a single file via curl with backup */
int  action_pull_dotfiles_file(const char *url, const char *dest);

/* Dispatch an action from a menu item */
void action_dispatch(const MenuItem *item);

#endif /* ACTIONS_H */
