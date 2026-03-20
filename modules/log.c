#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include "log.h"

char g_log_path[512] = {0};
static FILE *g_log_fp = NULL;

void log_init(void) {
    const char *home = getenv("HOME");
    if (!home) home = "/root";

    /* logs/ folder next to the binary — resolved via /proc/self/exe */
    char exe_path[512] = {0};
    char log_dir[512]  = {0};
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len > 0) {
        exe_path[len] = '\0';
        /* strip binary name to get directory */
        char *slash = strrchr(exe_path, '/');
        if (slash) *slash = '\0';
        snprintf(log_dir, sizeof(log_dir), "%s/logs", exe_path);
    } else {
        /* fallback */
        snprintf(log_dir, sizeof(log_dir), "%s/logs", home);
    }
    char mkdir_cmd[600];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd), "mkdir -p \"%s\"", log_dir);
    system(mkdir_cmd);

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char ts[64];
    strftime(ts, sizeof(ts), "%Y%m%d-%H%M%S", t);

    snprintf(g_log_path, sizeof(g_log_path),
             "%s/owlslify-%s.log", log_dir, ts);

    g_log_fp = fopen(g_log_path, "a");
    if (!g_log_fp)
        fprintf(stderr, "[owlslify] Warning: could not open log file %s\n", g_log_path);
}

void log_close(void) {
    if (g_log_fp) {
        fclose(g_log_fp);
        g_log_fp = NULL;
    }
}

void log_write(const char *fmt, ...) {
    if (!g_log_fp) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char ts[32];
    strftime(ts, sizeof(ts), "%H:%M:%S", t);

    fprintf(g_log_fp, "[%s] ", ts);

    va_list args;
    va_start(args, fmt);
    vfprintf(g_log_fp, fmt, args);
    va_end(args);

    fprintf(g_log_fp, "\n");
    fflush(g_log_fp);
}
