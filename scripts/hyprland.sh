#!/usr/bin/env bash
set -euo pipefail

AUR=${AUR:-yay}

echo "==> Installing Hyprland and companions..."

sudo pacman -S --needed --noconfirm \
    hyprland \
    waybar \
    wofi \
    dunst \
    swww \
    hyprlock \
    uwsm \
    xdg-desktop-portal-hyprland \
    xdg-desktop-portal-gtk \
    polkit-gnome \
    qt5-wayland qt6-wayland \
    wl-clipboard \
    grim slurp \
    wlogout \
    pipewire pipewire-pulse pipewire-alsa \
    wireplumber \
    noto-fonts-emoji \
    ttf-jetbrains-mono-nerd

# pyprland (scratchpads etc)
$AUR -S --needed --noconfirm pyprland

echo ""
echo "==> Hyprland installed. Log out and select Hyprland from SDDM, or run: Hyprland"
