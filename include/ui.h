#ifndef UI_H
#define UI_H

#include <ncurses.h>
#include "menu.h"

/* Colour pair IDs */
#define CP_NORMAL       1
#define CP_SELECTED     2
#define CP_TITLE        3
#define CP_TAB_ACTIVE   4
#define CP_TAB_INACTIVE 5
#define CP_BORDER       6
#define CP_DESC         7
#define CP_SUCCESS      8
#define CP_ERROR        9

typedef struct {
    WINDOW *win_tabs;
    WINDOW *win_list;
    WINDOW *win_desc;
    WINDOW *win_status;
    int     term_rows;
    int     term_cols;
} UIWindows;

void ui_init(UIWindows *ui);
void ui_destroy(UIWindows *ui);
void ui_resize(UIWindows *ui);
void ui_draw(UIWindows *ui, MenuState *state);
void ui_draw_tabs(UIWindows *ui, MenuState *state);
void ui_draw_list(UIWindows *ui, MenuState *state);
void ui_draw_desc(UIWindows *ui, MenuState *state);
void ui_set_status(UIWindows *ui, const char *msg, int is_error);
void ui_run_loop(UIWindows *ui, MenuState *state);

#endif /* UI_H */
