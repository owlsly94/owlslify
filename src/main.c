#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>
#include "ui.h"
#include "menu.h"
#include "log.h"

int main(void) {
    setlocale(LC_ALL, "");

    /* Check not running as root */
    if (getuid() == 0) {
        fprintf(stderr,
            "owlslify: do not run as root.\n"
            "  The tool uses sudo internally where needed.\n");
        return 1;
    }

    log_init();
    printf("[owlslify] Log: %s\n", g_log_path);

    UIWindows  ui;
    MenuState  state;

    menu_init(&state);
    ui_init(&ui);
    ui_run_loop(&ui, &state);
    ui_destroy(&ui);

    log_close();
    return 0;
}
