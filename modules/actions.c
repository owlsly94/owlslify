#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ncurses.h>
#include "actions.h"
#include "dotfiles.h"
#include "log.h"
#include "hardware.h"
#include "runner.h"

/* ------------------------------------------------------------------ */
/* Resolve script path                                                 */
/* ------------------------------------------------------------------ */
static void resolve_script(const char *cmd, char *out, size_t outsz) {
    if (strncmp(cmd, "scripts/", 8) != 0) {
        snprintf(out, outsz, "%s", cmd);
        return;
    }

    const char *name = cmd + 8;

    /* 1 — system install */
    char sys_path[512];
    snprintf(sys_path, sizeof(sys_path),
             "/usr/local/share/owlslify/scripts/%s", name);
    struct stat st;
    if (stat(sys_path, &st) == 0) {
        snprintf(out, outsz, "bash \"%s\"", sys_path);
        return;
    }

    /* 2 — next to binary */
    char exe[512] = {0};
    ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (len > 0) {
        exe[len] = '\0';
        char *slash = strrchr(exe, '/');
        if (slash) {
            *slash = '\0';
            char bin_path[768];
            snprintf(bin_path, sizeof(bin_path), "%s/scripts/%s", exe, name);
            if (stat(bin_path, &st) == 0) {
                snprintf(out, outsz, "bash \"%s\"", bin_path);
                return;
            }
        }
    }

    /* 3 — cwd fallback */
    snprintf(out, outsz, "bash \"%s\"", cmd);
}

/* ------------------------------------------------------------------ */
/* action_run_script — now uses the live runner panel                 */
/* ------------------------------------------------------------------ */
int action_run_script(const char *cmd) {
    if (!cmd || !*cmd) return 1;

    char resolved[4096];
    resolve_script(cmd, resolved, sizeof(resolved));

    log_write("ACTION_RUN: %s", resolved);

    /* Use the script name (without path) as the panel title */
    const char *title = cmd;
    const char *slash = strrchr(cmd, '/');
    if (slash) title = slash + 1;

    int ret = runner_exec(title, resolved);
    log_write("ACTION_RESULT: %d", ret);
    return ret;
}

/* ------------------------------------------------------------------ */
/* action_pull_dotfiles                                                */
/* ------------------------------------------------------------------ */
int action_pull_dotfiles(const char *app) {
    log_write("DOTFILES_PULL_FOLDER: %s", app);

    /* Build the degit command and run it through the runner panel */
    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
             "npx degit owlsly94/dotfiles/.config/%s ~/.config/%s --force",
             app, app);

    /* Back up first via a small inline shell snippet */
    char full[2048];
    snprintf(full, sizeof(full),
        "DEST=\"$HOME/.config/%s\"; "
        "[ -d \"$DEST\" ] && mv \"$DEST\" \"${DEST}.bak.$(date +%%s)\" && "
        "echo \"Backed up existing %s config\"; "
        "npx --yes degit owlsly94/dotfiles/.config/%s \"$DEST\" --force && "
        "echo \"Done: %s dotfiles installed\"",
        app, app, app, app);

    char title[128];
    snprintf(title, sizeof(title), "Dotfiles: %s", app);

    int ret = runner_exec(title, full);
    log_write("DOTFILES_RESULT: %d", ret);
    return ret;
}

/* ------------------------------------------------------------------ */
/* action_pull_dotfiles_file                                          */
/* ------------------------------------------------------------------ */
int action_pull_dotfiles_file(const char *url, const char *dest) {
    log_write("DOTFILES_PULL_FILE: %s -> %s", url, dest);

    char full[2048];
    snprintf(full, sizeof(full),
        "DEST=\"%s\"; "
        "[ -f \"$DEST\" ] && mv \"$DEST\" \"${DEST}.bak.$(date +%%s)\" && "
        "echo \"Backed up existing file\"; "
        "mkdir -p \"$(dirname $DEST)\"; "
        "curl -fLo \"$DEST\" \"%s\" && echo \"Done: $DEST installed\"",
        dest, url);

    /* derive a short title from the url */
    const char *title = url;
    const char *slash = strrchr(url, '/');
    if (slash) title = slash + 1;

    int ret = runner_exec(title, full);
    log_write("DOTFILES_FILE_RESULT: %d", ret);
    return ret;
}

/* ------------------------------------------------------------------ */
/* action_dispatch                                                     */
/* ------------------------------------------------------------------ */
void action_dispatch(const MenuItem *item) {
    if (!item) return;

    switch (item->type) {
        case ACTION_SCRIPT:
            action_run_script(item->script);
            break;
        case ACTION_DOTFILES:
            action_pull_dotfiles(item->dotfiles_app);
            break;
        case ACTION_DOTFILES_FILE:
            action_pull_dotfiles_file(item->dotfiles_url, item->dotfiles_dest);
            break;
        case ACTION_HW_SELECT: {
            HwSelection hw;
            if (hw_select(&hw) == 0)
                hw_run_flow(item->hw_flow, &hw);
            break;
        }
    }
}
