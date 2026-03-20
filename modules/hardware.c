#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <ncurses.h>
#include "hardware.h"
#include "actions.h"
#include "runner.h"
#include "log.h"

/* ------------------------------------------------------------------ */
/* Generic arrow-key picker                                            */
/* ------------------------------------------------------------------ */
typedef struct {
    const char *label;
    const char *desc;
} PickerOption;

static int picker(const char *title, const PickerOption *opts, int n) {
    int sel = 0;

    int term_rows, term_cols;
    getmaxyx(stdscr, term_rows, term_cols);

    int win_h = n + 6;
    int win_w = 64;
    int win_y = (term_rows - win_h) / 2;
    int win_x = (term_cols - win_w) / 2;
    if (win_y < 0) win_y = 0;
    if (win_x < 0) win_x = 0;

    WINDOW *win = newwin(win_h, win_w, win_y, win_x);
    keypad(win, TRUE);
    box(win, 0, 0);

    while (1) {
        /* title */
        wattron(win, COLOR_PAIR(3) | A_BOLD);
        int tx = (win_w - (int)strlen(title) - 4) / 2;
        if (tx < 1) tx = 1;
        mvwprintw(win, 1, tx, "[ %s ]", title);
        wattroff(win, COLOR_PAIR(3) | A_BOLD);

        wattron(win, COLOR_PAIR(7) | A_DIM);
        mvwprintw(win, 2, 2, "Arrow keys / j,k — Enter to confirm — q to cancel");
        wattroff(win, COLOR_PAIR(7) | A_DIM);

        /* options */
        for (int i = 0; i < n; i++) {
            int row = 3 + i;
            if (i == sel) {
                wattron(win, COLOR_PAIR(2) | A_BOLD);
                mvwprintw(win, row, 2, " > %-56s", opts[i].label);
                wattroff(win, COLOR_PAIR(2) | A_BOLD);
            } else {
                wattron(win, COLOR_PAIR(1));
                mvwprintw(win, row, 2, "   %-56s", opts[i].label);
                wattroff(win, COLOR_PAIR(1));
            }
        }

        /* desc of selected */
        wattron(win, COLOR_PAIR(7) | A_DIM);
        mvwprintw(win, 3 + n + 1, 2, "%-60s", opts[sel].desc);
        wattroff(win, COLOR_PAIR(7) | A_DIM);

        wrefresh(win);

        int ch = wgetch(win);
        switch (ch) {
            case KEY_UP:   case 'k': if (sel > 0)   sel--; break;
            case KEY_DOWN: case 'j': if (sel < n-1) sel++; break;
            case '\n': case KEY_ENTER:
                delwin(win);
                clear(); refresh();
                return sel;
            case 'q': case 27:
                delwin(win);
                clear(); refresh();
                return -1;
        }
    }
}

/* ------------------------------------------------------------------ */
/* hw_select                                                           */
/* ------------------------------------------------------------------ */
int hw_select(HwSelection *out) {
    static const PickerOption cpu_opts[] = {
        { "AMD",   "Installs amd-ucode microcode"              },
        { "Intel", "Installs intel-ucode + intel-media-driver" },
    };
    int cpu_choice = picker("Select your CPU", cpu_opts, 2);
    if (cpu_choice < 0) return -1;
    out->cpu = (cpu_choice == 0) ? CPU_AMD : CPU_INTEL;

    static const PickerOption gpu_opts[] = {
        { "AMD",                  "RADV/Mesa, ROCm, LACT, xf86-video-amdgpu"          },
        { "Nvidia (proprietary)", "nvidia-dkms, nvidia-utils, lib32-nvidia-utils"      },
        { "Nvidia (open)",        "nvidia-open-dkms — Turing+ cards only"              },
        { "Intel",                "xf86-video-intel, vulkan-intel, intel-media-driver" },
    };
    int gpu_choice = picker("Select your GPU", gpu_opts, 4);
    if (gpu_choice < 0) return -1;

    switch (gpu_choice) {
        case 0:  out->gpu = GPU_AMD;                break;
        case 1:  out->gpu = GPU_NVIDIA_PROPRIETARY; break;
        case 2:  out->gpu = GPU_NVIDIA_OPEN;        break;
        default: out->gpu = GPU_INTEL;              break;
    }

    return 0;
}

/* ------------------------------------------------------------------ */
/* hw_run_flow                                                         */
/* ------------------------------------------------------------------ */
void hw_run_flow(HwFlow flow, const HwSelection *hw) {
    const char *cpu_str;
    switch (hw->cpu) {
        case CPU_AMD:   cpu_str = "amd";   break;
        default:        cpu_str = "intel"; break;
    }

    const char *gpu_str;
    switch (hw->gpu) {
        case GPU_AMD:                gpu_str = "amd";         break;
        case GPU_NVIDIA_PROPRIETARY: gpu_str = "nvidia";      break;
        case GPU_NVIDIA_OPEN:        gpu_str = "nvidia-open"; break;
        default:                     gpu_str = "intel";       break;
    }

    const char *script_name;
    const char *title;
    switch (flow) {
        case HW_FLOW_OWLSLY_DESKTOP:
            script_name = "scripts/owlsly_desktop.sh";
            title       = "Owlsly's Desktop Setup";
            break;
        default:
            script_name = "scripts/gaming_hw.sh";
            title       = "Gaming Setup";
            break;
    }

    log_write("HW_FLOW: %s cpu=%s gpu=%s", title, cpu_str, gpu_str);

    /* Resolve scripts/ path (same logic as actions.c) */
    const char *name = script_name + 8; /* skip "scripts/" */
    char resolved[1024] = {0};
    struct stat st;

    /* 1 — system install */
    char sys_path[512];
    snprintf(sys_path, sizeof(sys_path),
             "/usr/local/share/owlslify/scripts/%s", name);
    if (stat(sys_path, &st) == 0) {
        snprintf(resolved, sizeof(resolved),
                 "bash \"%s\" %s %s", sys_path, cpu_str, gpu_str);
        goto run;
    }

    /* 2 — next to binary */
    {
        char exe[512] = {0};
        ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
        if (len > 0) {
            exe[len] = '\0';
            char *slash = strrchr(exe, '/');
            if (slash) {
                *slash = '\0';
                char bin_path[768];
                snprintf(bin_path, sizeof(bin_path),
                         "%s/scripts/%s", exe, name);
                if (stat(bin_path, &st) == 0) {
                    snprintf(resolved, sizeof(resolved),
                             "bash \"%s\" %s %s", bin_path, cpu_str, gpu_str);
                    goto run;
                }
            }
        }
    }

    /* 3 — cwd fallback */
    snprintf(resolved, sizeof(resolved),
             "bash \"%s\" %s %s", script_name, cpu_str, gpu_str);

run:
    runner_exec(title, resolved);
}
