#!/usr/bin/env bash
set -euo pipefail

sudo pacman -S --needed --noconfirm \
    sway swaylock swayidle \
    waybar wofi mako \
    wl-clipboard grim slurp \
    xdg-desktop-portal-wlr xdg-desktop-portal-gtk \
    pipewire pipewire-pulse pipewire-alsa wireplumber \
    ttf-jetbrains-mono-nerd noto-fonts-emoji

echo "==> Sway installed. Start from a TTY: sway"
