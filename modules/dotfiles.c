#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include "dotfiles.h"
#include "log.h"

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

static int cmd_exists(const char *cmd) {
    char buf[256];
    snprintf(buf, sizeof(buf), "command -v %s >/dev/null 2>&1", cmd);
    return system(buf) == 0;
}

static int run(const char *cmd) {
    printf("\n>>> %s\n", cmd);
    fflush(stdout);
    log_write("RUN: %s", cmd);
    int ret = system(cmd);
    log_write("EXIT: %d", ret);
    return ret;
}

static const char *get_home(void) {
    const char *h = getenv("HOME");
    return h ? h : "/root";
}

/* Expand leading ~/ to $HOME */
static void expand_path(const char *src, char *dst, size_t dstsz) {
    if (src[0] == '~' && src[1] == '/') {
        snprintf(dst, dstsz, "%s/%s", get_home(), src + 2);
    } else {
        snprintf(dst, dstsz, "%s", src);
    }
}

/* Back up a path (file or dir) to <path>.bak.<timestamp> */
static int backup_path(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) return 0; /* doesn't exist, nothing to do */

    time_t now = time(NULL);
    char backup[768];
    snprintf(backup, sizeof(backup), "%s.bak.%ld", path, (long)now);

    char cmd[1600];
    snprintf(cmd, sizeof(cmd), "mv \"%s\" \"%s\"", path, backup);
    printf("[owlslify] Backing up %s → %s\n", path, backup);
    log_write("BACKUP: %s -> %s", path, backup);
    return run(cmd);
}

/* ------------------------------------------------------------------ */
/* dotfiles_ensure_deps                                                */
/* ------------------------------------------------------------------ */
int dotfiles_ensure_deps(void) {
    int ok = 1;

    if (!cmd_exists("node") || !cmd_exists("npm")) {
        printf("[owlslify] Installing nodejs and npm...\n");
        if (run("sudo pacman -S --needed --noconfirm nodejs npm") != 0) {
            fprintf(stderr, "[owlslify] ERROR: failed to install nodejs/npm\n");
            ok = 0;
        }
    }

    if (ok && !cmd_exists("npx")) {
        fprintf(stderr, "[owlslify] ERROR: npx not found after npm install\n");
        ok = 0;
    }

    if (ok && system("npm list -g degit >/dev/null 2>&1") != 0) {
        printf("[owlslify] Installing degit globally...\n");
        if (run("sudo npm install -g degit") != 0) {
            fprintf(stderr, "[owlslify] ERROR: failed to install degit\n");
            ok = 0;
        }
    }

    return ok ? 0 : 1;
}

/* ------------------------------------------------------------------ */
/* dotfiles_pull_folder                                                */
/* Pull ~/.config/<app> from owlsly94/dotfiles/.config/<app>          */
/* ------------------------------------------------------------------ */
int dotfiles_pull_folder(const char *app) {
    if (!app || !*app) return 1;

    if (dotfiles_ensure_deps() != 0) return 1;

    char config_path[512];
    snprintf(config_path, sizeof(config_path), "%s/.config/%s", get_home(), app);

    if (backup_path(config_path) != 0) {
        fprintf(stderr, "[owlslify] ERROR: backup failed\n");
        return 1;
    }

    /* npx degit owlsly94/dotfiles/.config/<app> ~/.config/<app> --force */
    char degit_src[256];
    snprintf(degit_src, sizeof(degit_src),
             DOTFILES_USER "/" DOTFILES_REPO "/.config/%s", app);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
             "npx degit \"%s\" \"%s\" --force", degit_src, config_path);

    printf("[owlslify] Pulling dotfiles for '%s'...\n", app);
    int ret = run(cmd);
    if (ret != 0)
        fprintf(stderr, "[owlslify] ERROR: degit failed for '%s'\n", app);
    else
        printf("[owlslify] ✔ '%s' installed to %s\n", app, config_path);

    return ret;
}

/* ------------------------------------------------------------------ */
/* dotfiles_pull_file                                                  */
/* Pull a single file via curl with backup                             */
/* ------------------------------------------------------------------ */
int dotfiles_pull_file(const char *url, const char *dest) {
    if (!url || !dest) return 1;

    char expanded[512];
    expand_path(dest, expanded, sizeof(expanded));

    backup_path(expanded);

    /* ensure parent directory exists */
    char mkdir_cmd[600];
    snprintf(mkdir_cmd, sizeof(mkdir_cmd),
             "mkdir -p \"$(dirname '%s')\"", expanded);
    run(mkdir_cmd);

    char cmd[1024];
    snprintf(cmd, sizeof(cmd),
             "curl -fLo \"%s\" --create-dirs \"%s\"", expanded, url);

    printf("[owlslify] Pulling file: %s → %s\n", url, expanded);
    int ret = run(cmd);
    if (ret != 0)
        fprintf(stderr, "[owlslify] ERROR: curl failed for '%s'\n", url);
    else
        printf("[owlslify] ✔ File installed to %s\n", expanded);

    return ret;
}

/* ------------------------------------------------------------------ */
/* dotfiles_pull_archive                                               */
/* Download .tar.gz and extract to dest_dir                            */
/* ------------------------------------------------------------------ */
int dotfiles_pull_archive(const char *url, const char *dest_dir) {
    if (!url || !dest_dir) return 1;

    char expanded[512];
    expand_path(dest_dir, expanded, sizeof(expanded));

    /* Back up existing dest */
    backup_path(expanded);

    /* Download to /tmp */
    char tmp_file[] = "/tmp/owlslify-archive-XXXXXX.tar.gz";
    /* simple deterministic name is fine here */
    snprintf(tmp_file, sizeof(tmp_file), "/tmp/owlslify-dl-%ld.tar.gz", (long)time(NULL));

    char curl_cmd[512];
    snprintf(curl_cmd, sizeof(curl_cmd), "curl -fLo \"%s\" \"%s\"", tmp_file, url);
    if (run(curl_cmd) != 0) {
        fprintf(stderr, "[owlslify] ERROR: failed to download archive\n");
        return 1;
    }

    /* Extract */
    char tar_cmd[1600];
    snprintf(tar_cmd, sizeof(tar_cmd),
             "mkdir -p \"%s\" && tar -xzf \"%s\" -C \"%s\"",
             expanded, tmp_file, expanded);
    int ret = run(tar_cmd);

    /* Cleanup */
    char rm_cmd[256];
    snprintf(rm_cmd, sizeof(rm_cmd), "rm -f \"%s\"", tmp_file);
    run(rm_cmd);

    if (ret != 0)
        fprintf(stderr, "[owlslify] ERROR: tar extraction failed\n");
    else
        printf("[owlslify] ✔ Archive extracted to %s\n", expanded);

    return ret;
}
