#!/usr/bin/env bash
set -euo pipefail

AUR=${AUR:-yay}
command -v $AUR &>/dev/null || { echo "ERROR: $AUR not found."; exit 1; }

echo "==> Ensuring multilib is enabled..."
if ! grep -q "^\[multilib\]" /etc/pacman.conf; then
    sudo sed -i '/\[multilib\]/,/Include/s/^#//' /etc/pacman.conf
    sudo pacman -Sy
fi

echo "==> Installing gaming stack..."
sudo pacman -S --needed --noconfirm \
    steam \
    gamemode lib32-gamemode \
    gamescope \
    mangohud lib32-mangohud \
    lutris

$AUR -S --needed --noconfirm \
    heroic-games-launcher-bin \
    faugus-launcher \
    protonup-qt

# Wine + deps
sudo pacman -S --needed --noconfirm \
    wine-staging wine-mono wine-gecko winetricks \
    lib32-libpulse lib32-alsa-lib lib32-alsa-plugins \
    lib32-libx11 lib32-libxext lib32-libxrandr \
    lib32-gnutls lib32-mpg123 lib32-openal \
    lib32-vulkan-icd-loader vulkan-icd-loader \
    lib32-vkd3d vkd3d

echo ""
echo "==> Gaming setup complete!"
echo "    • Open ProtonUp-Qt to install Proton-GE for Steam."
echo "    • Add 'gamemoderun %command%' to Steam launch options."
echo "    • Add 'MANGOHUD=1 %command%' to Steam launch options."
