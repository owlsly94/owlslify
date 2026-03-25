#!/usr/bin/env bash
set -euo pipefail

echo "==> Installing mesa-tkg-git (bleeding edge)..."

if ! grep -q "\[chaotic-aur\]" /etc/pacman.conf; then
    echo "ERROR: Chaotic-AUR is required for mesa-tkg-git."
    echo "       Run 'Add Chaotic AUR' from the System tab first."
    exit 1
fi

# Remove stable mesa first to avoid conflicts
echo "==> Removing stable mesa packages if present..."
sudo pacman -Rns --noconfirm mesa mesa-utils lib32-mesa \
    vulkan-radeon lib32-vulkan-radeon 2>/dev/null || true

echo "==> Installing mesa-tkg-git..."
sudo pacman -S --needed --noconfirm --ask 4 \
    mesa-tkg-git \
    lib32-mesa-tkg-git

echo ""
echo "==> mesa-tkg-git installed."
echo "    vulkan-radeon is NOT needed — RADV is bundled inside mesa-tkg-git."
echo "    Reboot recommended."
