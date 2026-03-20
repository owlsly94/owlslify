#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "actions.h"
#include "log.h"

/* ------------------------------------------------------------------ */
/* Layout                                                              */
/* ------------------------------------------------------------------ */
#define TAB_HEIGHT    3
#define DESC_HEIGHT   5
#define STATUS_HEIGHT 1

/* ------------------------------------------------------------------ */
/* Colour init                                                         */
/* ------------------------------------------------------------------ */
static void init_colors(void) {
    start_color();
    use_default_colors();
    init_pair(CP_NORMAL,       COLOR_WHITE,  -1);
    init_pair(CP_SELECTED,     COLOR_BLACK,  COLOR_CYAN);
    init_pair(CP_TITLE,        COLOR_CYAN,   -1);
    init_pair(CP_TAB_ACTIVE,   COLOR_BLACK,  COLOR_CYAN);
    init_pair(CP_TAB_INACTIVE, COLOR_CYAN,   -1);
    init_pair(CP_BORDER,       COLOR_CYAN,   -1);
    init_pair(CP_DESC,         COLOR_WHITE,  -1);
    init_pair(CP_SUCCESS,      COLOR_GREEN,  -1);
    init_pair(CP_ERROR,        COLOR_RED,    -1);
}

static int list_height(UIWindows *ui) {
    return ui->term_rows - 1 - TAB_HEIGHT - DESC_HEIGHT - STATUS_HEIGHT - 2;
}

/* ------------------------------------------------------------------ */
/* ui_init                                                             */
/* ------------------------------------------------------------------ */
void ui_init(UIWindows *ui) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    set_escdelay(25);
    init_colors();

    getmaxyx(stdscr, ui->term_rows, ui->term_cols);

    int list_h = list_height(ui);
    int list_y = 1 + TAB_HEIGHT;
    int desc_y = list_y + list_h + 1;
    int stat_y = ui->term_rows - 1;

    ui->win_tabs   = newwin(TAB_HEIGHT,    ui->term_cols, 1,      0);
    ui->win_list   = newwin(list_h,        ui->term_cols, list_y, 0);
    ui->win_desc   = newwin(DESC_HEIGHT,   ui->term_cols, desc_y, 0);
    ui->win_status = newwin(STATUS_HEIGHT, ui->term_cols, stat_y, 0);

    keypad(ui->win_list, TRUE);
}

/* ------------------------------------------------------------------ */
/* ui_destroy                                                          */
/* ------------------------------------------------------------------ */
void ui_destroy(UIWindows *ui) {
    delwin(ui->win_tabs);
    delwin(ui->win_list);
    delwin(ui->win_desc);
    delwin(ui->win_status);
    endwin();
}

/* ------------------------------------------------------------------ */
/* ui_resize                                                           */
/* ------------------------------------------------------------------ */
void ui_resize(UIWindows *ui) {
    getmaxyx(stdscr, ui->term_rows, ui->term_cols);

    int list_h = list_height(ui);
    int list_y = 1 + TAB_HEIGHT;
    int desc_y = list_y + list_h + 1;
    int stat_y = ui->term_rows - 1;

    wresize(ui->win_tabs,   TAB_HEIGHT,    ui->term_cols);
    wresize(ui->win_list,   list_h,        ui->term_cols);
    wresize(ui->win_desc,   DESC_HEIGHT,   ui->term_cols);
    wresize(ui->win_status, STATUS_HEIGHT, ui->term_cols);

    mvwin(ui->win_tabs,   1,      0);
    mvwin(ui->win_list,   list_y, 0);
    mvwin(ui->win_desc,   desc_y, 0);
    mvwin(ui->win_status, stat_y, 0);

    clear();
    refresh();
}

/* ------------------------------------------------------------------ */
/* Title bar                                                           */
/* ------------------------------------------------------------------ */
static void draw_title(UIWindows *ui) {
    const char *title = " owlslify — Arch Linux setup utility ";
    const char *keys  = " [↑↓] move  [←→/Tab] tabs  [Enter] run  [q] quit ";

    attron(COLOR_PAIR(CP_TITLE) | A_BOLD);
    mvhline(0, 0, ' ', ui->term_cols);
    int tx = (ui->term_cols - (int)strlen(title)) / 2;
    if (tx < 0) tx = 0;
    mvprintw(0, tx, "%s", title);
    attroff(COLOR_PAIR(CP_TITLE) | A_BOLD);

    attron(COLOR_PAIR(CP_NORMAL) | A_DIM);
    int kx = ui->term_cols - (int)strlen(keys);
    if (kx < 0) kx = 0;
    mvprintw(0, kx, "%s", keys);
    attroff(COLOR_PAIR(CP_NORMAL) | A_DIM);
}

/* ------------------------------------------------------------------ */
/* ui_draw_tabs                                                        */
/* ------------------------------------------------------------------ */
void ui_draw_tabs(UIWindows *ui, MenuState *state) {
    WINDOW *w = ui->win_tabs;
    werase(w);

    wattron(w, COLOR_PAIR(CP_BORDER));
    mvwhline(w, 0, 0, ACS_HLINE, ui->term_cols);
    wattroff(w, COLOR_PAIR(CP_BORDER));

    int x = 2;
    for (int i = 0; i < state->cat_count; i++) {
        char tab[MAX_LABEL + 4];
        snprintf(tab, sizeof(tab), "  %s  ", state->categories[i].label);

        if (i == state->selected_cat) {
            wattron(w, COLOR_PAIR(CP_TAB_ACTIVE) | A_BOLD);
            mvwprintw(w, 1, x, "%s", tab);
            wattroff(w, COLOR_PAIR(CP_TAB_ACTIVE) | A_BOLD);
        } else {
            wattron(w, COLOR_PAIR(CP_TAB_INACTIVE));
            mvwprintw(w, 1, x, "%s", tab);
            wattroff(w, COLOR_PAIR(CP_TAB_INACTIVE));
        }
        x += (int)strlen(tab) + 1;
    }

    wattron(w, COLOR_PAIR(CP_BORDER));
    mvwhline(w, 2, 0, ACS_HLINE, ui->term_cols);
    wattroff(w, COLOR_PAIR(CP_BORDER));

    wnoutrefresh(w);
}

/* ------------------------------------------------------------------ */
/* ui_draw_list                                                        */
/* ------------------------------------------------------------------ */
void ui_draw_list(UIWindows *ui, MenuState *state) {
    WINDOW *w = ui->win_list;
    int rows, cols;
    getmaxyx(w, rows, cols);
    werase(w);

    Category *cat   = &state->categories[state->selected_cat];
    int       visible = rows;

    /* clamp scroll */
    if (state->selected_item < state->scroll_offset)
        state->scroll_offset = state->selected_item;
    if (state->selected_item >= state->scroll_offset + visible)
        state->scroll_offset = state->selected_item - visible + 1;

    for (int i = 0; i < visible; i++) {
        int idx = i + state->scroll_offset;
        if (idx >= cat->item_count) break;

        /* prefix icon by type */
        const char *icon = " > ";
        if (cat->items[idx].type == ACTION_DOTFILES ||
            cat->items[idx].type == ACTION_DOTFILES_FILE)
            icon = " * ";

        /* heap-allocate line sized to actual terminal cols — prevents stack smash */
        int bufsz = (cols > 8) ? cols + 1 : 256;
        char *line = calloc(bufsz, 1);
        if (!line) break;

        snprintf(line, bufsz, "%s%s", icon, cat->items[idx].label);

        /* pad to full width so selection highlight fills the row */
        int len = (int)strlen(line);
        if (len < bufsz - 1) {
            memset(line + len, ' ', bufsz - 1 - len);
            line[bufsz - 1] = '\0';
        }

        if (idx == state->selected_item) {
            wattron(w, COLOR_PAIR(CP_SELECTED) | A_BOLD);
            mvwprintw(w, i, 0, "%s", line);
            wattroff(w, COLOR_PAIR(CP_SELECTED) | A_BOLD);
        } else {
            wattron(w, COLOR_PAIR(CP_NORMAL));
            mvwprintw(w, i, 0, "%s", line);
            wattroff(w, COLOR_PAIR(CP_NORMAL));
        }
        free(line);
    }

    /* scrollbar */
    if (cat->item_count > visible) {
        wattron(w, COLOR_PAIR(CP_BORDER) | A_DIM);
        int bar_y = (state->selected_item * (visible - 1)) / (cat->item_count - 1);
        mvwaddch(w, bar_y, cols - 1, ACS_CKBOARD);
        wattroff(w, COLOR_PAIR(CP_BORDER) | A_DIM);
    }

    wnoutrefresh(w);
}

/* ------------------------------------------------------------------ */
/* ui_draw_desc                                                        */
/* ------------------------------------------------------------------ */
void ui_draw_desc(UIWindows *ui, MenuState *state) {
    WINDOW *w = ui->win_desc;
    werase(w);

    wattron(w, COLOR_PAIR(CP_BORDER));
    mvwhline(w, 0, 0, ACS_HLINE, ui->term_cols);
    wattroff(w, COLOR_PAIR(CP_BORDER));

    Category *cat = &state->categories[state->selected_cat];
    if (cat->item_count > 0) {
        MenuItem *item = &cat->items[state->selected_item];

        wattron(w, COLOR_PAIR(CP_TITLE) | A_BOLD);
        mvwprintw(w, 1, 2, "▶ %s", item->label);
        wattroff(w, COLOR_PAIR(CP_TITLE) | A_BOLD);

        wattron(w, COLOR_PAIR(CP_DESC));
        /* word-wrap desc at term width - 6 */
        int max_w = ui->term_cols - 6;
        if (max_w < 20) max_w = 20;
        char desc_copy[MAX_DESC];
        strncpy(desc_copy, item->desc, MAX_DESC - 1);
        /* simple truncation for now */
        if ((int)strlen(desc_copy) > max_w)
            desc_copy[max_w] = '\0';
        mvwprintw(w, 2, 4, "%s", desc_copy);
        wattroff(w, COLOR_PAIR(CP_DESC));

        /* show log path hint */
        if (g_log_path[0]) {
            wattron(w, COLOR_PAIR(CP_NORMAL) | A_DIM);
            mvwprintw(w, 3, 4, "Log: %s", g_log_path);
            wattroff(w, COLOR_PAIR(CP_NORMAL) | A_DIM);
        }
    }

    wnoutrefresh(w);
}

/* ------------------------------------------------------------------ */
/* ui_set_status                                                       */
/* ------------------------------------------------------------------ */
void ui_set_status(UIWindows *ui, const char *msg, int is_error) {
    WINDOW *w = ui->win_status;
    werase(w);
    int pair = is_error ? CP_ERROR : CP_SUCCESS;
    wattron(w, COLOR_PAIR(pair) | A_BOLD);
    mvwprintw(w, 0, 1, "%s", msg);
    wattroff(w, COLOR_PAIR(pair) | A_BOLD);
    wnoutrefresh(w);
    doupdate();
}

/* ------------------------------------------------------------------ */
/* ui_draw                                                             */
/* ------------------------------------------------------------------ */
void ui_draw(UIWindows *ui, MenuState *state) {
    draw_title(ui);
    ui_draw_tabs(ui, state);
    ui_draw_list(ui, state);
    ui_draw_desc(ui, state);
    doupdate();
}

/* ------------------------------------------------------------------ */
/* ui_run_loop                                                         */
/* ------------------------------------------------------------------ */
void ui_run_loop(UIWindows *ui, MenuState *state) {
    ui_draw(ui, state);

    while (1) {
        int ch = wgetch(ui->win_list);
        Category *cat = &state->categories[state->selected_cat];

        switch (ch) {

            case KEY_UP: case 'k':
                if (state->selected_item > 0)
                    state->selected_item--;
                break;

            case KEY_DOWN: case 'j':
                if (state->selected_item < cat->item_count - 1)
                    state->selected_item++;
                break;

            case KEY_LEFT: case 'h':
                if (state->selected_cat > 0) {
                    state->selected_cat--;
                    state->selected_item = 0;
                    state->scroll_offset = 0;
                }
                break;

            case KEY_RIGHT: case 'l':
                if (state->selected_cat < state->cat_count - 1) {
                    state->selected_cat++;
                    state->selected_item = 0;
                    state->scroll_offset = 0;
                }
                break;

            case '\t':
                state->selected_cat = (state->selected_cat + 1) % state->cat_count;
                state->selected_item = 0;
                state->scroll_offset = 0;
                break;

            case KEY_PPAGE: /* Page Up */
                state->selected_item -= 10;
                if (state->selected_item < 0) state->selected_item = 0;
                break;

            case KEY_NPAGE: /* Page Down */
                state->selected_item += 10;
                if (state->selected_item >= cat->item_count)
                    state->selected_item = cat->item_count - 1;
                break;

            case KEY_HOME: case 'g':
                state->selected_item = 0;
                state->scroll_offset = 0;
                break;

            case KEY_END: case 'G':
                state->selected_item = cat->item_count - 1;
                break;

            case '\n': case KEY_ENTER: {
                if (cat->item_count > 0) {
                    MenuItem *item = &cat->items[state->selected_item];
                    ui_set_status(ui, " Running…", 0);
                    log_write("USER_RUN: [%s] %s",
                              state->categories[state->selected_cat].label,
                              item->label);
                    action_dispatch(item);
                    ui_set_status(ui, " Done.", 0);
                }
                break;
            }

            case KEY_RESIZE:
                ui_resize(ui);
                break;

            case 'q': case 'Q':
                return;

            default:
                break;
        }

        ui_draw(ui, state);
    }
}
