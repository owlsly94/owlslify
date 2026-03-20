#!/usr/bin/env bash
set -euo pipefail

sudo pacman -S --needed --noconfirm \
    plasma-desktop plasma-wayland-session \
    sddm sddm-kcm \
    dolphin konsole kate spectacle ark gwenview okular \
    plasma-nm plasma-pa powerdevil kscreen bluedevil \
    kde-gtk-config breeze-gtk xdg-user-dirs

sudo systemctl enable sddm.service
echo "==> KDE Plasma installed. Reboot to start SDDM."
