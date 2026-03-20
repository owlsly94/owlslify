#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "runner.h"
#include "log.h"

/* ------------------------------------------------------------------ */
/* Layout                                                              */
/* ------------------------------------------------------------------ */
#define TITLE_H   3   /* top border + title + separator  */
#define STATUS_H  3   /* separator + status + bottom border */

/* Strip common ANSI escape sequences so they don't corrupt ncurses */
static void strip_ansi(const char *src, char *dst, size_t dstsz) {
    size_t di = 0;
    for (size_t i = 0; src[i] && di < dstsz - 1; i++) {
        if (src[i] == '\033' && src[i+1] == '[') {
            /* skip until letter */
            i += 2;
            while (src[i] && src[i] != 'm' && src[i] != 'K' &&
                   src[i] != 'A' && src[i] != 'B' && src[i] != 'C' &&
                   src[i] != 'D' && src[i] != 'J' && src[i] != 'H')
                i++;
        } else if (src[i] == '\r') {
            /* skip carriage returns */
        } else {
            dst[di++] = src[i];
        }
    }
    dst[di] = '\0';
}

/* ------------------------------------------------------------------ */
/* runner_exec                                                         */
/* ------------------------------------------------------------------ */
int runner_exec(const char *title, const char *cmd) {
    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);

    /* Panel takes up almost the whole screen with a small margin */
    int win_h = term_rows - 4;
    int win_w = term_cols - 4;
    int win_y = 2;
    int win_x = 2;
    if (win_h < 10) win_h = 10;
    if (win_w < 40) win_w = 40;

    int out_h = win_h - TITLE_H - STATUS_H; /* lines available for output */
    int out_w = win_w - 2;                  /* inner width */

    WINDOW *win = newwin(win_h, win_w, win_y, win_x);
    keypad(win, TRUE);

    /* ---- draw static chrome ---- */
    wbkgd(win, COLOR_PAIR(1));
    box(win, 0, 0);

    /* title bar */
    wattron(win, COLOR_PAIR(3) | A_BOLD);
    mvwhline(win, 0, 1, ACS_HLINE, win_w - 2);
    int tx = (win_w - (int)strlen(title) - 4) / 2;
    if (tx < 1) tx = 1;
    mvwprintw(win, 0, tx, "[ %s ]", title);
    wattroff(win, COLOR_PAIR(3) | A_BOLD);

    /* subtitle */
    wattron(win, COLOR_PAIR(7) | A_DIM);
    mvwprintw(win, 1, 2, "Running — output streaming live...");
    wattroff(win, COLOR_PAIR(7) | A_DIM);

    /* separator below title */
    wattron(win, COLOR_PAIR(6));
    mvwhline(win, TITLE_H - 1, 1, ACS_HLINE, win_w - 2);
    wattroff(win, COLOR_PAIR(6));

    /* separator above status */
    wattron(win, COLOR_PAIR(6));
    mvwhline(win, win_h - STATUS_H, 1, ACS_HLINE, win_w - 2);
    wattroff(win, COLOR_PAIR(6));

    wrefresh(win);

    /* ---- output sub-window (scrollable) ---- */
    WINDOW *out_win = newwin(out_h, out_w, win_y + TITLE_H, win_x + 1);
    scrollok(out_win, TRUE);
    wbkgd(out_win, COLOR_PAIR(1));
    wrefresh(out_win);

    /* ---- run command, pipe output ---- */
    /* redirect stderr to stdout so we capture everything */
    char full_cmd[8192];
    if (g_log_path[0]) {
        snprintf(full_cmd, sizeof(full_cmd),
                 "{ %s ; } 2>&1 | tee -a \"%s\"", cmd, g_log_path);
    } else {
        snprintf(full_cmd, sizeof(full_cmd), "{ %s ; } 2>&1", cmd);
    }

    log_write("RUNNER_START: %s", cmd);

    FILE *fp = popen(full_cmd, "r");
    int exit_code = 1;

    if (!fp) {
        wattron(out_win, COLOR_PAIR(9) | A_BOLD);
        wprintw(out_win, "ERROR: failed to start command\n");
        wattroff(out_win, COLOR_PAIR(9) | A_BOLD);
        wrefresh(out_win);
    } else {
        char raw[4096];
        char clean[4096];

        while (fgets(raw, sizeof(raw), fp)) {
            /* strip trailing newline for clean display */
            size_t rlen = strlen(raw);
            if (rlen > 0 && raw[rlen-1] == '\n')
                raw[rlen-1] = '\0';

            strip_ansi(raw, clean, sizeof(clean));
            log_write("OUT: %s", clean);

            /* truncate line to window width */
            if ((int)strlen(clean) >= out_w)
                clean[out_w - 1] = '\0';

            wattron(out_win, COLOR_PAIR(1));
            wprintw(out_win, "%s\n", clean);
            wattroff(out_win, COLOR_PAIR(1));
            wrefresh(out_win);
        }

        int status = pclose(fp);
        /* WEXITSTATUS handles both direct exit and signal */
        exit_code = (status == -1) ? 1 :
                    (WIFEXITED(status) ? WEXITSTATUS(status) : 1);
    }

    log_write("RUNNER_EXIT: %d", exit_code);

    /* ---- update subtitle ---- */
    wattron(win, COLOR_PAIR(1));
    mvwprintw(win, 1, 2, "%-*s", win_w - 4, " "); /* clear line */
    wattroff(win, COLOR_PAIR(1));

    if (exit_code == 0) {
        wattron(win, COLOR_PAIR(7) | A_DIM);
        mvwprintw(win, 1, 2, "Finished");
        wattroff(win, COLOR_PAIR(7) | A_DIM);
    } else {
        wattron(win, COLOR_PAIR(9) | A_DIM);
        mvwprintw(win, 1, 2, "Finished with errors (exit %d)", exit_code);
        wattroff(win, COLOR_PAIR(9) | A_DIM);
    }

    /* ---- status line ---- */
    int stat_y = win_h - STATUS_H + 1;

    if (exit_code == 0) {
        wattron(win, COLOR_PAIR(8) | A_BOLD);
        mvwprintw(win, stat_y, 2, " ✔  Done! ");
        wattroff(win, COLOR_PAIR(8) | A_BOLD);
    } else {
        wattron(win, COLOR_PAIR(9) | A_BOLD);
        mvwprintw(win, stat_y, 2, " ✘  Failed (exit code %d) ", exit_code);
        wattroff(win, COLOR_PAIR(9) | A_BOLD);
    }

    /* key hint right side */
    const char *hint = " Press any key to return ";
    wattron(win, COLOR_PAIR(7) | A_DIM);
    mvwprintw(win, stat_y, win_w - (int)strlen(hint) - 1, "%s", hint);
    wattroff(win, COLOR_PAIR(7) | A_DIM);

    wrefresh(win);

    /* wait for keypress */
    wtimeout(win, -1);
    wgetch(win);

    /* cleanup */
    delwin(out_win);
    delwin(win);
    clear();
    refresh();

    return exit_code;
}
