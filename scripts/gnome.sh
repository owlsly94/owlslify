#!/usr/bin/env bash
set -euo pipefail

sudo pacman -S --needed --noconfirm \
    gnome gnome-tweaks gnome-shell-extensions gdm xdg-user-dirs

sudo systemctl enable gdm.service
echo "==> GNOME installed. Reboot to start GDM."
