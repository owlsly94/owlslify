# owlslify

A terminal-based Arch Linux setup utility written in C, inspired by [linutil](https://github.com/ChrisTitusTech/linutil).  
Navigate categories, run setup scripts, and pull your dotfiles — all from a clean ncurses TUI.

---

## Features

| Category | What's included |
|----------|----------------|
| **System** | Pacman beautify, Chaotic AUR, multilib, rate-mirrors, yay, makepkg opt, I/O scheduler, sysctl tweaks, CPU governor, TRIM, zram, earlyoom, font rendering, paccache hook, mirror timer, AMD+Wayland env, libvirt, package export |
| **Apps** | Full install list (90+ packages), gaming stack (Steam/Heroic/Faugus/Gamemode/Gamescope/MangoHud/Wine/ProtonUp), browsers, dev tools, media, utilities |
| **Desktop** | Hyprland, KDE, GNOME, Sway, SDDM autologin, theming, fonts, dotfiles for every app |
| **Security** | UFW, SSH hardening, fail2ban, ClamAV, AppArmor |

---

## Dependencies

```bash
sudo pacman -S ncurses gcc make
```

---

## Build & Run

```bash
git clone https://github.com/owlsly94/owlslify
cd owlslify
make
./bin/owlslify
```

Install system-wide:

```bash
sudo make install    # → /usr/local/bin/owlslify
sudo make uninstall
```

---

## Keybindings

| Key | Action |
|-----|--------|
| `↑` / `k` | Move up |
| `↓` / `j` | Move down |
| `←` / `h` | Previous tab |
| `→` / `l` | Next tab |
| `Tab` | Cycle tabs forward |
| `PgUp` / `PgDn` | Scroll 10 items |
| `g` / `G` | Jump to top / bottom |
| `Enter` | Run selected action |
| `q` | Quit |

---

## Dotfiles

Dotfile folder pulls use `npx degit` to fetch only `~/.config/<app>` from  
`github.com/owlsly94/dotfiles` — no full clone, no git history.  
Single files use `curl`. Archives use `curl` + `tar`.

Before any pull, existing configs are backed up to `~/.config/<app>.bak.<timestamp>`.  
Node.js / npm / degit are installed automatically if missing.

---

## Logging

Every run is logged to `~/owlslify-<timestamp>.log` automatically.  
The log path is shown in the description panel at the bottom of the TUI.

---

## Project Structure

```
owlslify/
├── Makefile
├── README.md
├── bin/              ← compiled binary (owlslify)
├── build/            ← object files
│   ├── src/
│   └── modules/
├── include/          ← all headers (.h)
│   ├── actions.h
│   ├── dotfiles.h
│   ├── log.h
│   ├── menu.h
│   └── ui.h
├── modules/          ← implementation files (.c)
│   ├── actions.c
│   ├── dotfiles.c
│   ├── log.c
│   ├── menu.c
│   └── ui.c
├── src/
│   └── main.c
└── scripts/          ← shell scripts called by menu actions
```

---

