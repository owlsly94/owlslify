#include <string.h>
#include "menu.h"
#include "dotfiles.h"
#include "hardware.h"

/* ------------------------------------------------------------------ */
/* Helper macro                                                        */
/* ------------------------------------------------------------------ */
#define ADD_ITEM(cat, lbl, dsc, typ, scr, dot, url, dst) do {         \
    int _i = (cat)->item_count++;                                      \
    strncpy((cat)->items[_i].label,        (lbl), MAX_LABEL - 1);     \
    strncpy((cat)->items[_i].desc,         (dsc), MAX_DESC  - 1);     \
    (cat)->items[_i].type         = (typ);                             \
    (cat)->items[_i].script       = (scr);                             \
    (cat)->items[_i].dotfiles_app = (dot);                             \
    (cat)->items[_i].dotfiles_url = (url);                             \
    (cat)->items[_i].dotfiles_dest= (dst);                             \
} while(0)

#define SCRIPT(cat, lbl, dsc, cmd) \
    ADD_ITEM(cat, lbl, dsc, ACTION_SCRIPT, cmd, NULL, NULL, NULL)

#define DOTFOLDER(cat, lbl, dsc, app) \
    ADD_ITEM(cat, lbl, dsc, ACTION_DOTFILES, NULL, app, NULL, NULL)

#define DOTFILE(cat, lbl, dsc, url, dst) \
    ADD_ITEM(cat, lbl, dsc, ACTION_DOTFILES_FILE, NULL, NULL, url, dst)

#define HWFLOW(cat, lbl, dsc, flow) do { \
    int _i = (cat)->item_count++; \
    strncpy((cat)->items[_i].label, (lbl), MAX_LABEL - 1); \
    strncpy((cat)->items[_i].desc,  (dsc), MAX_DESC  - 1); \
    (cat)->items[_i].type    = ACTION_HW_SELECT; \
    (cat)->items[_i].hw_flow = (flow); \
} while(0)

/* ------------------------------------------------------------------ */
/* 0 — System Setup & Tweaks                                           */
/* ------------------------------------------------------------------ */
static void build_system(Category *c) {
    strncpy(c->label, "System", MAX_LABEL - 1);
    c->item_count = 0;

    SCRIPT(c,
        "Full system update",
        "Run: sudo pacman -Syu",
        "sudo pacman -Syu --noconfirm");

    SCRIPT(c,
        "Pacman beautify",
        "Enable Color, ILoveCandy, CheckSpace, VerbosePkgLists, ParallelDownloads=10",
        "scripts/pacman_beautify.sh");

    SCRIPT(c,
        "Add Chaotic AUR",
        "Import Chaotic-AUR key, install keyring + mirrorlist, append to pacman.conf",
        "scripts/chaotic_aur.sh");

    SCRIPT(c,
        "Enable multilib",
        "Uncomment [multilib] in /etc/pacman.conf and sync databases",
        "scripts/multilib.sh");

    SCRIPT(c,
        "Rate & update mirrors",
        "Install rate-mirrors from Chaotic-AUR and rank fastest HTTPS mirrors",
        "scripts/mirrors.sh");

    SCRIPT(c,
        "Install yay (AUR helper)",
        "Clone and build yay-bin from AUR (or install from Chaotic-AUR)",
        "scripts/install_yay.sh");

    SCRIPT(c,
        "Optimise makepkg",
        "Set MAKEFLAGS=-j$(nproc), enable multi-threaded xz/zstd compression",
        "scripts/makepkg_opt.sh");

    SCRIPT(c,
        "I/O scheduler (NVMe/SSD)",
        "Write udev rules: none for NVMe, mq-deadline for SSD, bfq for HDD",
        "scripts/io_scheduler.sh");

    SCRIPT(c,
        "Kernel tweaks (sysctl)",
        "swappiness=10, vfs_cache_pressure=50, BBR TCP congestion control, fq qdisc",
        "scripts/sysctl_tune.sh");

    SCRIPT(c,
        "Gaming network optimizations",
        "TCP buffer tuning, BBR+FQ, TCP Fast Open, Steam download cfg. Never touches DNS.",
        "scripts/gaming_network.sh");

    SCRIPT(c,
        "CPU governor (performance)",
        "Install cpupower, set performance governor, enable cpupower.service",
        "scripts/cpu_governor.sh");

    SCRIPT(c,
        "Enable TRIM (fstrim)",
        "Enable and start fstrim.timer for weekly SSD/NVMe TRIM",
        "sudo systemctl enable fstrim.timer && sudo systemctl start fstrim.timer");

    SCRIPT(c,
        "Setup zram",
        "Install zram-generator, configure zram0 = RAM/2 with zstd compression",
        "scripts/zram.sh");

    SCRIPT(c,
        "Install earlyoom",
        "Install earlyoom and enable the service to prevent OOM freezes",
        "sudo pacman -S --needed --noconfirm earlyoom && sudo systemctl enable --now earlyoom");

    SCRIPT(c,
        "Font rendering (LCD)",
        "Enable subpixel RGB + LCD filter via fontconfig symlinks",
        "scripts/font_rendering.sh");

    SCRIPT(c,
        "Paccache cleanup hook",
        "Install a pacman hook that runs paccache -rk2 after every transaction",
        "scripts/paccache_hook.sh");

    SCRIPT(c,
        "Monthly mirror update timer",
        "Create rate-mirrors.service + rate-mirrors.timer and enable it",
        "scripts/mirror_timer.sh");

    SCRIPT(c,
        "Configure /etc/environment (AMD + Wayland)",
        "Set RADV, LIBVA, VDPAU, Mesa cache, Wayland flags for Qt/Electron/SDL",
        "scripts/env_amd_wayland.sh");

    SCRIPT(c,
        "Configure libvirt / QEMU",
        "Enable unix socket group, start libvirtd, add user to libvirt group",
        "scripts/libvirt.sh");

    SCRIPT(c,
        "Export installed packages list",
        "Run pacman -Qqe > ~/installed-packages.txt",
        "pacman -Qqe > ~/installed-packages.txt && echo 'Saved to ~/installed-packages.txt'");
}

/* ------------------------------------------------------------------ */
/* 1 — App Installation                                                */
/* ------------------------------------------------------------------ */
static void build_apps(Category *c) {
    strncpy(c->label, "Apps", MAX_LABEL - 1);
    c->item_count = 0;

    /* --- Owlsly's Desktop --- */
    HWFLOW(c,
        "Owlsly's Desktop",
        "Full personal setup: all apps, dotfiles, Hyprland, hardware-specific drivers & tweaks",
        HW_FLOW_OWLSLY_DESKTOP);

    /* --- Full installs --- */
    SCRIPT(c,
        "Install ALL apps (full list)",
        "Install every app from your curated list via yay in one shot",
        "scripts/install_all.sh");

    /* --- Gaming --- */
    HWFLOW(c,
        "Gaming setup (hardware-aware)",
        "Steam, Heroic, Faugus, Gamemode, MangoHud, Wine, ProtonUp + GPU-specific drivers/tweaks",
        HW_FLOW_GAMING);

    SCRIPT(c,
        "Steam",
        "sudo pacman -S --needed --noconfirm steam",
        "sudo pacman -S --needed --noconfirm steam");

    SCRIPT(c,
        "Heroic Games Launcher",
        "yay -S --needed --noconfirm heroic-games-launcher-bin",
        "yay -S --needed --noconfirm heroic-games-launcher-bin");

    SCRIPT(c,
        "Faugus Launcher",
        "yay -S --needed --noconfirm faugus-launcher",
        "yay -S --needed --noconfirm faugus-launcher");

    SCRIPT(c,
        "Lutris",
        "sudo pacman -S --needed --noconfirm lutris",
        "sudo pacman -S --needed --noconfirm lutris");

    SCRIPT(c,
        "Gamemode + Gamescope",
        "sudo pacman -S --needed --noconfirm gamemode lib32-gamemode gamescope",
        "sudo pacman -S --needed --noconfirm gamemode lib32-gamemode gamescope");

    SCRIPT(c,
        "MangoHud",
        "sudo pacman -S --needed --noconfirm mangohud lib32-mangohud",
        "sudo pacman -S --needed --noconfirm mangohud lib32-mangohud");

    SCRIPT(c,
        "Wine (staging) + deps",
        "Install wine-staging, wine-mono, winetricks and all lib32 dependencies",
        "scripts/wine.sh");

    SCRIPT(c,
        "ProtonUp-Qt",
        "yay -S --needed --noconfirm protonup-qt",
        "yay -S --needed --noconfirm protonup-qt");

    SCRIPT(c,
        "Prismlauncher (Minecraft)",
        "sudo pacman -S --needed --noconfirm prismlauncher",
        "sudo pacman -S --needed --noconfirm prismlauncher");

    /* --- Browsers --- */
    SCRIPT(c,
        "Firefox",
        "sudo pacman -S --needed --noconfirm firefox",
        "sudo pacman -S --needed --noconfirm firefox");

    SCRIPT(c,
        "Google Chrome",
        "yay -S --needed --noconfirm google-chrome",
        "yay -S --needed --noconfirm google-chrome");

    SCRIPT(c,
        "Zen Browser",
        "yay -S --needed --noconfirm zen-browser-bin",
        "yay -S --needed --noconfirm zen-browser-bin");

    /* --- Dev tools --- */
    SCRIPT(c,
        "Neovim",
        "sudo pacman -S --needed --noconfirm neovim",
        "sudo pacman -S --needed --noconfirm neovim");

    SCRIPT(c,
        "Visual Studio Code",
        "yay -S --needed --noconfirm visual-studio-code-bin",
        "yay -S --needed --noconfirm visual-studio-code-bin");

    SCRIPT(c,
        "Kitty terminal",
        "sudo pacman -S --needed --noconfirm kitty",
        "sudo pacman -S --needed --noconfirm kitty");

    SCRIPT(c,
        "Alacritty terminal",
        "sudo pacman -S --needed --noconfirm alacritty",
        "sudo pacman -S --needed --noconfirm alacritty");

    SCRIPT(c,
        "Git + base-devel",
        "sudo pacman -S --needed --noconfirm git base-devel",
        "sudo pacman -S --needed --noconfirm git base-devel");

    SCRIPT(c,
        "Rust",
        "sudo pacman -S --needed --noconfirm rust",
        "sudo pacman -S --needed --noconfirm rust");

    SCRIPT(c,
        "Node.js + npm",
        "sudo pacman -S --needed --noconfirm nodejs npm",
        "sudo pacman -S --needed --noconfirm nodejs npm");

    SCRIPT(c,
        "Hugo (static site generator)",
        "sudo pacman -S --needed --noconfirm hugo",
        "sudo pacman -S --needed --noconfirm hugo");

    SCRIPT(c,
        "JDK 21 (GraalVM)",
        "yay -S --needed --noconfirm jdk21-graalvm-bin",
        "yay -S --needed --noconfirm jdk21-graalvm-bin");

    /* --- Media & creativity --- */
    SCRIPT(c,
        "OBS Studio",
        "sudo pacman -S --needed --noconfirm obs-studio",
        "sudo pacman -S --needed --noconfirm obs-studio");

    SCRIPT(c,
        "Kdenlive (video editor)",
        "sudo pacman -S --needed --noconfirm kdenlive",
        "sudo pacman -S --needed --noconfirm kdenlive");

    SCRIPT(c,
        "Audacity",
        "sudo pacman -S --needed --noconfirm audacity",
        "sudo pacman -S --needed --noconfirm audacity");

    SCRIPT(c,
        "MPV",
        "sudo pacman -S --needed --noconfirm mpv",
        "sudo pacman -S --needed --noconfirm mpv");

    SCRIPT(c,
        "Jellyfin Desktop",
        "yay -S --needed --noconfirm jellyfin-desktop",
        "yay -S --needed --noconfirm jellyfin-desktop");

    /* --- Utilities --- */
    SCRIPT(c,
        "Btop",
        "sudo pacman -S --needed --noconfirm btop",
        "sudo pacman -S --needed --noconfirm btop");

    SCRIPT(c,
        "Fastfetch",
        "sudo pacman -S --needed --noconfirm fastfetch",
        "sudo pacman -S --needed --noconfirm fastfetch");

    SCRIPT(c,
        "Thunar + plugins",
        "sudo pacman -S --needed --noconfirm thunar thunar-archive-plugin thunar-media-tags-plugin thunar-volman",
        "sudo pacman -S --needed --noconfirm thunar thunar-archive-plugin thunar-media-tags-plugin thunar-volman");

    SCRIPT(c,
        "Ranger (TUI file manager)",
        "sudo pacman -S --needed --noconfirm ranger ueberzug",
        "sudo pacman -S --needed --noconfirm ranger ueberzug");

    SCRIPT(c,
        "qBittorrent",
        "sudo pacman -S --needed --noconfirm qbittorrent",
        "sudo pacman -S --needed --noconfirm qbittorrent");

    SCRIPT(c,
        "Bitwarden",
        "sudo pacman -S --needed --noconfirm bitwarden",
        "sudo pacman -S --needed --noconfirm bitwarden");

    SCRIPT(c,
        "Discord",
        "sudo pacman -S --needed --noconfirm discord",
        "sudo pacman -S --needed --noconfirm discord");

    SCRIPT(c,
        "LocalSend",
        "yay -S --needed --noconfirm localsend",
        "yay -S --needed --noconfirm localsend");

    SCRIPT(c,
        "EasyEffects (audio)",
        "sudo pacman -S --needed --noconfirm easyeffects",
        "sudo pacman -S --needed --noconfirm easyeffects");

    SCRIPT(c,
        "Helvum (pipewire patchbay)",
        "sudo pacman -S --needed --noconfirm helvum",
        "sudo pacman -S --needed --noconfirm helvum");

    SCRIPT(c,
        "Flatpak",
        "sudo pacman -S --needed --noconfirm flatpak xdg-desktop-portal-gtk",
        "sudo pacman -S --needed --noconfirm flatpak xdg-desktop-portal-gtk");

    SCRIPT(c,
        "Virt-Manager (QEMU/KVM)",
        "sudo pacman -S --needed --noconfirm virt-manager virt-viewer qemu-full swtpm",
        "sudo pacman -S --needed --noconfirm virt-manager virt-viewer qemu-full swtpm");

    SCRIPT(c,
        "LACT (AMD GPU control)",
        "yay -S --needed --noconfirm lact",
        "yay -S --needed --noconfirm lact");

    SCRIPT(c,
        "NBTexplorer (Minecraft NBT)",
        "yay -S --needed --noconfirm nbtexplorer-bin",
        "yay -S --needed --noconfirm nbtexplorer-bin");

    SCRIPT(c,
        "Topgrade (system upgrader)",
        "sudo pacman -S --needed --noconfirm topgrade",
        "sudo pacman -S --needed --noconfirm topgrade");

    SCRIPT(c,
        "Zsh + completions",
        "sudo pacman -S --needed --noconfirm zsh zsh-completions",
        "sudo pacman -S --needed --noconfirm zsh zsh-completions");

    SCRIPT(c,
        "Starship prompt",
        "sudo pacman -S --needed --noconfirm starship",
        "sudo pacman -S --needed --noconfirm starship");

    SCRIPT(c,
        "Eza (modern ls)",
        "sudo pacman -S --needed --noconfirm eza",
        "sudo pacman -S --needed --noconfirm eza");

    SCRIPT(c,
        "Zoxide (smart cd)",
        "sudo pacman -S --needed --noconfirm zoxide",
        "sudo pacman -S --needed --noconfirm zoxide");

    SCRIPT(c,
        "Megatools (MEGA.nz CLI)",
        "sudo pacman -S --needed --noconfirm megatools",
        "sudo pacman -S --needed --noconfirm megatools");
}

/* ------------------------------------------------------------------ */
/* 2 — Desktop Environment                                             */
/* ------------------------------------------------------------------ */
static void build_desktop(Category *c) {
    strncpy(c->label, "Desktop", MAX_LABEL - 1);
    c->item_count = 0;

    /* --- Install DEs --- */
    SCRIPT(c,
        "Install Hyprland (full)",
        "Hyprland, waybar, wofi, dunst, swww, hyprlock, pyprland, uwsm, xdg-portal",
        "scripts/hyprland.sh");

    SCRIPT(c,
        "Install KDE Plasma",
        "plasma-desktop, sddm, dolphin, konsole and recommended KDE apps",
        "scripts/kde.sh");

    SCRIPT(c,
        "Install GNOME",
        "gnome group, gdm, gnome-tweaks, extensions",
        "scripts/gnome.sh");

    SCRIPT(c,
        "Install Sway",
        "sway, waybar, wofi, mako, swaylock, swayidle, xdg-desktop-portal-wlr",
        "scripts/sway.sh");

    /* --- Display manager --- */
    SCRIPT(c,
        "Configure SDDM autologin → Hyprland",
        "Write /etc/sddm.conf with Autologin User=$USER Session=hyprland",
        "scripts/sddm_autologin.sh");

    /* --- Theming --- */
    SCRIPT(c,
        "Install themes & icons",
        "tokyonight-gtk-theme-git, papirus-icon-theme, bibata-cursor-theme, kvantum",
        "yay -S --needed --noconfirm tokyonight-gtk-theme-git papirus-icon-theme bibata-cursor-theme kvantum kvantum-qt5");

    SCRIPT(c,
        "Install fonts",
        "ttf-jetbrains-mono, ttf-jetbrains-mono-nerd, noto-fonts-emoji",
        "sudo pacman -S --needed --noconfirm ttf-jetbrains-mono ttf-jetbrains-mono-nerd noto-fonts-emoji");

    SCRIPT(c,
        "Steam download optimisation",
        "Write steam_dev.cfg: disable HTTP/2, improve multi-connection threshold",
        "scripts/steam_cfg.sh");

    /* --- Dotfiles: config folders --- */
    DOTFOLDER(c,
        "Dotfiles → hypr",
        "Pull owlsly94/dotfiles/.config/hypr → ~/.config/hypr (backup existing)",
        "hypr");

    DOTFOLDER(c,
        "Dotfiles → waybar",
        "Pull owlsly94/dotfiles/.config/waybar → ~/.config/waybar",
        "waybar");

    DOTFOLDER(c,
        "Dotfiles → wofi",
        "Pull owlsly94/dotfiles/.config/wofi → ~/.config/wofi",
        "wofi");

    DOTFOLDER(c,
        "Dotfiles → rofi",
        "Pull owlsly94/dotfiles/.config/rofi → ~/.config/rofi",
        "rofi");

    DOTFOLDER(c,
        "Dotfiles → dunst",
        "Pull owlsly94/dotfiles/.config/dunst → ~/.config/dunst",
        "dunst");

    DOTFOLDER(c,
        "Dotfiles → wlogout",
        "Pull owlsly94/dotfiles/.config/wlogout → ~/.config/wlogout",
        "wlogout");

    DOTFOLDER(c,
        "Dotfiles → kitty",
        "Pull owlsly94/dotfiles/.config/kitty → ~/.config/kitty",
        "kitty");

    DOTFOLDER(c,
        "Dotfiles → alacritty",
        "Pull owlsly94/dotfiles/.config/alacritty → ~/.config/alacritty",
        "alacritty");

    DOTFOLDER(c,
        "Dotfiles → nvim",
        "Pull owlsly94/dotfiles/.config/nvim → ~/.config/nvim",
        "nvim");

    DOTFOLDER(c,
        "Dotfiles → mpv",
        "Pull owlsly94/dotfiles/.config/mpv → ~/.config/mpv",
        "mpv");

    DOTFOLDER(c,
        "Dotfiles → MangoHud",
        "Pull owlsly94/dotfiles/.config/MangoHud → ~/.config/MangoHud",
        "MangoHud");

    DOTFOLDER(c,
        "Dotfiles → fastfetch",
        "Pull owlsly94/dotfiles/.config/fastfetch → ~/.config/fastfetch",
        "fastfetch");

    DOTFOLDER(c,
        "Dotfiles → ranger",
        "Pull owlsly94/dotfiles/.config/ranger → ~/.config/ranger",
        "ranger");

    DOTFOLDER(c,
        "Dotfiles → nwg-look",
        "Pull owlsly94/dotfiles/.config/nwg-look → ~/.config/nwg-look",
        "nwg-look");

    DOTFOLDER(c,
        "Dotfiles → pypr",
        "Pull owlsly94/dotfiles/.config/pypr → ~/.config/pypr",
        "pypr");

    DOTFOLDER(c,
        "Dotfiles → wallpapers",
        "Pull owlsly94/dotfiles/.config/wallpapers → ~/.config/wallpapers",
        "wallpapers");

    /* --- Dotfiles: single files --- */
    DOTFILE(c,
        "Dotfiles → starship.toml",
        "Pull starship.toml from dotfiles repo to ~/.config/starship.toml",
        DOTFILES_RAW "/.config/starship.toml",
        "~/.config/starship.toml");

    DOTFILE(c,
        "Dotfiles → chrome-flags.conf",
        "Pull chrome-flags.conf from dotfiles repo to ~/.config/chrome-flags.conf",
        DOTFILES_RAW "/.config/chrome-flags.conf",
        "~/.config/chrome-flags.conf");

    DOTFILE(c,
        "Dotfiles → .zshrc",
        "Pull .zshrc from dotfiles repo to ~/.zshrc (backup existing)",
        DOTFILES_RAW "/.zshrc",
        "~/.zshrc");

    /* zsh archive */
    SCRIPT(c,
        "Dotfiles → zsh config archive",
        "Download zsh.tar.gz from dotfiles and extract to ~/.config/zsh",
        "scripts/dotfiles_zsh_archive.sh");

    /* pull ALL dotfiles at once */
    SCRIPT(c,
        "Pull ALL dotfiles",
        "Pull every config folder and file from owlsly94/dotfiles in one go",
        "scripts/dotfiles_all.sh");
}

/* ------------------------------------------------------------------ */
/* 3 — Security & Networking                                           */
/* ------------------------------------------------------------------ */
static void build_security(Category *c) {
    strncpy(c->label, "Security", MAX_LABEL - 1);
    c->item_count = 0;

    SCRIPT(c,
        "Enable & configure UFW",
        "Install ufw, default deny incoming, allow outgoing + SSH, enable service",
        "scripts/ufw.sh");

    SCRIPT(c,
        "SSH hardening",
        "Backup sshd_config, disable root login + password auth, restart sshd",
        "scripts/ssh_harden.sh");

    SCRIPT(c,
        "Install fail2ban",
        "Install fail2ban, create jail.local with SSH jail (ban 1h / 5 retries)",
        "scripts/fail2ban.sh");

    SCRIPT(c,
        "Install ClamAV",
        "Install clamav, run freshclam, enable clamd.service",
        "scripts/clamav.sh");

    SCRIPT(c,
        "Enable AppArmor",
        "Install apparmor, patch GRUB cmdline, enable apparmor.service",
        "scripts/apparmor.sh");
}

/* ------------------------------------------------------------------ */
/* Public init                                                          */
/* ------------------------------------------------------------------ */
void menu_init(MenuState *state) {
    memset(state, 0, sizeof(MenuState));
    state->cat_count     = 4;
    state->selected_cat  = 0;
    state->selected_item = 0;
    state->scroll_offset = 0;

    build_system(  &state->categories[0]);
    build_apps(    &state->categories[1]);
    build_desktop( &state->categories[2]);
    build_security(&state->categories[3]);
}
