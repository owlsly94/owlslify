#!/usr/bin/env bash
set -euo pipefail

# Check for AUR helper
if command -v yay &>/dev/null; then
    AUR=yay
elif command -v paru &>/dev/null; then
    AUR=paru
else
    echo "ERROR: No AUR helper found. Run 'Install yay' from the System tab first."
    exit 1
fi

echo "==> Installing full app list via $AUR..."
echo "    This will take a while. Output is logged."
echo ""

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
    eza \
    fastfetch \
    faugus-launcher \
    ffmpegthumbnailer \
    firefox \
    flatpak \
    gamemode \
    gamescope \
    git \
    glmark2 \
    glow \
    gnome-keyring \
    google-chrome \
    grim \
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
    iwd \
    jdk21-graalvm-bin \
    jellyfin-desktop \
    kdenlive \
    kitty \
    kvantum \
    kvantum-qt5 \
    lact \
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
    libpulse \
    libva-utils \
    localsend \
    lutris \
    man-db \
    mangohud \
    megatools \
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
    radeontop \
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
    wget \
    wine-mono \
    wine-staging \
    winetricks \
    wireplumber \
    wl-clipboard \
    wlogout \
    xdg-desktop-portal-gtk \
    xdg-desktop-portal-hyprland \
    xdg-user-dirs \
    xdg-utils \
    xf86-video-amdgpu \
    yay \
    zen-browser-bin \
    zip \
    zoxide \
    zram-generator \
    zsh \
    zsh-completions

echo ""
echo "==> Full app installation complete!"
