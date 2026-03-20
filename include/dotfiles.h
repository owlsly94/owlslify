#ifndef DOTFILES_H
#define DOTFILES_H

#define DOTFILES_USER    "owlsly94"
#define DOTFILES_REPO    "dotfiles"
#define DOTFILES_RAW     "https://raw.githubusercontent.com/" DOTFILES_USER "/" DOTFILES_REPO "/main"

/* Pull ~/.config/<app> via npx degit. Backs up existing dir first. */
int dotfiles_pull_folder(const char *app);

/* Pull a single file via curl. Backs up existing file first.
   dest is the full destination path, e.g. ~/.zshrc               */
int dotfiles_pull_file(const char *url, const char *dest);

/* Pull a .tar.gz archive via curl and extract to ~/.config/ */
int dotfiles_pull_archive(const char *url, const char *dest_dir);

/* Ensure nodejs / npm / npx / degit are installed */
int dotfiles_ensure_deps(void);

#endif /* DOTFILES_H */
