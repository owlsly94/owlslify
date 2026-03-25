#!/usr/bin/env bash
# Usage: owlsly_desktop.sh <cpu: amd|intel> <gpu: amd|nvidia|nvidia-open|intel>
set -euo pipefail

CPU="${1:-amd}"
GPU="${2:-amd}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RAW="https://raw.githubusercontent.com/owlsly94/dotfiles/main"
REPO="owlsly94/dotfiles"

echo "╔══════════════════════════════════════════════════════╗"
echo "║         owlslify — Owlsly's Desktop Setup            ║"
echo "║                                                      ║"
printf  "║  CPU: %-46s ║\n" "$CPU"
printf  "║  GPU: %-46s ║\n" "$GPU"
echo "╚══════════════════════════════════════════════════════╝"
echo ""

# ------------------------------------------------------------------ #
# Sanity checks                                                       #
# ------------------------------------------------------------------ #
if [ "$EUID" -eq 0 ]; then
    echo "ERROR: do not run as root." >&2
    exit 1
fi
command -v pacman &>/dev/null || { echo "ERROR: pacman not found."; exit 1; }

# ------------------------------------------------------------------ #
# 1. Pacman setup                                                     #
# ------------------------------------------------------------------ #
echo "==> [1/12] Configuring pacman..."
bash "$SCRIPT_DIR/pacman_beautify.sh"
bash "$SCRIPT_DIR/chaotic_aur.sh"
bash "$SCRIPT_DIR/multilib.sh"
bash "$SCRIPT_DIR/mirrors.sh"

# ------------------------------------------------------------------ #
# 2. AUR helper                                                       #
# ------------------------------------------------------------------ #
echo "==> [2/12] Installing yay..."
bash "$SCRIPT_DIR/install_yay.sh"

AUR=yay

# ------------------------------------------------------------------ #
# 3. CPU & GPU drivers                                                #
# ------------------------------------------------------------------ #
echo "==> [3/12] Installing CPU drivers ($CPU)..."
bash "$SCRIPT_DIR/drivers_cpu.sh" "$CPU"

echo "==> [3/12] Installing GPU drivers ($GPU)..."
bash "$SCRIPT_DIR/drivers_gpu.sh" "$GPU"

# ------------------------------------------------------------------ #
# 4. Full app list                                                    #
# ------------------------------------------------------------------ #
echo "==> [4/12] Installing all applications..."

$AUR -S --needed --noconfirm --ask 4 \
    alacritty \
    audacity \
    base-devel \
    bash-completion \
    bibata-cursor-theme \
    bitwarden \
    bridge-utils \
    btop \
    cmake \
    cpupower \
    discord \
    dmidecode \
    dnsmasq \
    dunst \
    earlyoom \
    easyeffects \
    efibootmgr \
    emacs \
    eza \
    fastfetch \
    faugus-launcher \
    ffmpegthumbnailer \
    firefox \
    flatpak \
    flite \
    gamemode \
    gamescope \
    git \
    glmark2 \
    glow \
    gnome-keyring \
    google-chrome \
    grim \
    grub \
    gst-plugin-pipewire \
    gum \
    gvfs \
    hdparm \
    helvum \
    heroic-games-launcher-bin \
    htop \
    hugo \
    hyprland \
    hyprlock \
    imv \
    iptables-nft \
    iwd \
    jdk21-graalvm-bin \
    jellyfin-desktop \
    kdenlive \
    kitty \
    kvantum \
    kvantum-qt5 \
    less \
    lib32-alsa-lib \
    lib32-alsa-plugins \
    lib32-gamemode \
    lib32-gcc-libs \
    lib32-gtk2 \
    lib32-gtk3 \
    lib32-libcurl-gnutls \
    lib32-libpulse \
    lib32-mangohud \
    lib32-pipewire \
    lib32-pipewire-jack \
    lib32-sqlite \
    lib32-vkd3d \
    libguestfs \
    libpulse \
    libva-utils \
    linux-firmware \
    linux-zen \
    linux-zen-headers \
    localsend \
    lutris \
    man-db \
    mangohud \
    megatools \
    mkinitcpio-firmware \
    mpv \
    nbtexplorer-bin \
    neovim \
    network-manager-applet \
    networkmanager \
    nodejs \
    npm \
    noto-fonts-emoji \
    nwg-look \
    obs-studio \
    openbsd-netcat \
    orca \
    pamixer \
    papirus-icon-theme \
    pavucontrol \
    pipewire \
    pipewire-alsa \
    pipewire-jack \
    pipewire-pulse \
    polkit-gnome \
    power-profiles-daemon \
    prismlauncher \
    protonup-qt \
    pyprland \
    python-pipx \
    qbittorrent \
    qemu-full \
    qt5-wayland \
    qt5ct \
    qt6-wayland \
    qt6ct \
    ranger \
    rate-mirrors \
    rofi \
    rofimoji \
    rsync \
    rust \
    sddm \
    seahorse \
    slurp \
    smartmontools \
    starship \
    steam \
    swtpm \
    awww \
    thunar \
    thunar-archive-plugin \
    thunar-media-tags-plugin \
    thunar-volman \
    tldr \
    tokyonight-gtk-theme-git \
    topgrade \
    tree \
    ttf-jetbrains-mono \
    ttf-jetbrains-mono-nerd \
    ueberzug \
    unzip \
    uwsm \
    v4l2loopback-dkms \
    vde2 \
    vdpauinfo \
    vim \
    virt-manager \
    virt-viewer \
    visual-studio-code-bin \
    vkd3d \
    vulkan-tools \
    w3m \
    waybar \
    webkit2gtk \
    wget \
    wine-mono \
    wine-staging \
    winetricks \
    wireless_tools \
    wireplumber \
    wl-clipboard \
    wlogout \
    wpa_supplicant \
    xboxdrv \
    xdg-desktop-portal-gtk \
    xdg-desktop-portal-hyprland \
    xdg-user-dirs \
    xdg-utils \
    xorg-server \
    xorg-xinit \
    yay \
    zen-browser-bin \
    zip \
    zoxide \
    zram-generator \
    zsh \
    zsh-completions

# AMD-only packages
if [ "$GPU" = "amd" ]; then
    $AUR -S --needed --noconfirm --ask 4 \
        xf86-video-amdgpu \
        xf86-video-ati \
        radeontop \
        rocm-opencl-runtime \
        rocm-smi-lib \
        lact \
        amf-headers \
        lib32-mesa-tkg-git \
        mesa-tkg-git
fi

# ------------------------------------------------------------------ #
# 5. System tweaks                                                    #
# ------------------------------------------------------------------ #
echo "==> [5/12] Applying system tweaks..."
bash "$SCRIPT_DIR/makepkg_opt.sh"
bash "$SCRIPT_DIR/io_scheduler.sh"
bash "$SCRIPT_DIR/sysctl_tune.sh"
bash "$SCRIPT_DIR/cpu_governor.sh"
bash "$SCRIPT_DIR/zram.sh"
bash "$SCRIPT_DIR/font_rendering.sh"
bash "$SCRIPT_DIR/paccache_hook.sh"
bash "$SCRIPT_DIR/mirror_timer.sh"

sudo systemctl enable fstrim.timer
sudo systemctl start  fstrim.timer
sudo pacman -S --needed --noconfirm earlyoom
sudo systemctl enable --now earlyoom

# ------------------------------------------------------------------ #
# 6. Hyprland desktop                                                 #
# ------------------------------------------------------------------ #
echo "==> [6/12] Installing Hyprland..."
bash "$SCRIPT_DIR/hyprland.sh"

# ------------------------------------------------------------------ #
# 7. SDDM autologin                                                   #
# ------------------------------------------------------------------ #
echo "==> [7/12] Configuring SDDM..."
bash "$SCRIPT_DIR/sddm_autologin.sh"

# ------------------------------------------------------------------ #
# 8. Libvirt                                                          #
# ------------------------------------------------------------------ #
echo "==> [8/12] Configuring libvirt..."
bash "$SCRIPT_DIR/libvirt.sh"

# ------------------------------------------------------------------ #
# 9. Steam config                                                     #
# ------------------------------------------------------------------ #
echo "==> [9/12] Steam download optimisations..."
bash "$SCRIPT_DIR/steam_cfg.sh"

# ------------------------------------------------------------------ #
# 10. Node / degit                                                    #
# ------------------------------------------------------------------ #
echo "==> [10/12] Installing Node.js / degit..."
sudo pacman -S --needed --noconfirm nodejs npm
sudo npm install -g degit

# ------------------------------------------------------------------ #
# 11. Dotfiles                                                        #
# ------------------------------------------------------------------ #
echo "==> [11/12] Pulling dotfiles..."

CONFIG_FOLDERS=(
    hypr kitty alacritty dunst fastfetch
    mpv MangoHud nvim nwg-look pypr
    ranger wallpapers waybar wlogout wofi rofi
)

for folder in "${CONFIG_FOLDERS[@]}"; do
    DEST="$HOME/.config/$folder"
    if [ -d "$DEST" ]; then
        mv "$DEST" "${DEST}.bak.$(date +%s)"
        echo "  Backed up $folder"
    fi
    if npx --yes degit "$REPO/.config/$folder" "$DEST" --force; then
        echo "  ✔ $folder"
    else
        echo "  ✘ $folder (skipped)"
    fi
done

# Single config files
curl -fLo "$HOME/.config/starship.toml" --create-dirs \
    "$RAW/.config/starship.toml"   && echo "  ✔ starship.toml"   || echo "  ✘ starship.toml"
curl -fLo "$HOME/.config/chrome-flags.conf" --create-dirs \
    "$RAW/.config/chrome-flags.conf" && echo "  ✔ chrome-flags.conf" || echo "  ✘ chrome-flags.conf"

# .zshrc
[ -f "$HOME/.zshrc" ] && mv "$HOME/.zshrc" "$HOME/.zshrc.bak.$(date +%s)"
curl -fLo "$HOME/.zshrc" "$RAW/.zshrc" && echo "  ✔ .zshrc" || echo "  ✘ .zshrc"

# zsh archive
TMP="/tmp/owlslify-zsh-$(date +%s).tar.gz"
if curl -fLo "$TMP" "https://github.com/owlsly94/dotfiles/raw/refs/heads/main/.config/zsh.tar.gz"; then
    [ -d "$HOME/.config/zsh" ] && mv "$HOME/.config/zsh" "$HOME/.config/zsh.bak.$(date +%s)"
    tar -xzf "$TMP" -C "$HOME/.config/"
    rm -f "$TMP"
    echo "  ✔ zsh archive"
fi

# ------------------------------------------------------------------ #
# 12. Change shell to zsh                                             #
# ------------------------------------------------------------------ #
echo "==> [12/12] Setting default shell to zsh..."
chsh -s /bin/zsh "$USER" || echo "  (run 'chsh -s /bin/zsh' manually if this failed)"

# ------------------------------------------------------------------ #
# Summary                                                             #
# ------------------------------------------------------------------ #
echo ""
echo "╔══════════════════════════════════════════════════════╗"
echo "║        Owlsly's Desktop — Setup Complete!            ║"
echo "╚══════════════════════════════════════════════════════╝"
echo ""
echo "  CPU:     $CPU"
echo "  GPU:     $GPU"
echo ""
echo "  ✔ Pacman configured & Chaotic-AUR added"
echo "  ✔ All apps installed"
echo "  ✔ System tweaks applied"
echo "  ✔ Hyprland installed"
echo "  ✔ SDDM autologin configured"
echo "  ✔ Dotfiles deployed"
echo "  ✔ Shell set to zsh"
echo ""
echo "  REBOOT to apply all changes."
echo ""

# Save installed packages list
pacman -Qqe > "$HOME/installed-packages.txt"
echo "  Package list saved to ~/installed-packages.txt"
