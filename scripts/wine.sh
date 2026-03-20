#!/usr/bin/env bash
set -euo pipefail

if ! grep -q "^\[multilib\]" /etc/pacman.conf; then
    sudo sed -i '/\[multilib\]/,/Include/s/^#//' /etc/pacman.conf
    sudo pacman -Sy
fi

sudo pacman -S --needed --noconfirm \
    wine-staging wine-mono wine-gecko winetricks \
    lib32-libpulse lib32-alsa-lib lib32-alsa-plugins \
    lib32-libx11 lib32-libxext lib32-libxrandr \
    lib32-gnutls lib32-mpg123 lib32-openal \
    lib32-vulkan-icd-loader vulkan-icd-loader

echo "==> Wine staging installed. Run 'winecfg' to configure."
