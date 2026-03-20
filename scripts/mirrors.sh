#!/usr/bin/env bash
set -euo pipefail

echo "==> Installing rate-mirrors (from Chaotic-AUR)..."
sudo pacman -S --needed --noconfirm rate-mirrors

echo "==> Backing up current mirrorlist..."
sudo cp /etc/pacman.d/mirrorlist /etc/pacman.d/mirrorlist.backup

echo "==> Rating mirrors — this may take a minute..."
rate-mirrors --protocol https arch --max-delay 3600 \
    | sudo tee /etc/pacman.d/mirrorlist

sudo pacman -Sy
echo "==> Mirrorlist updated. Backup at /etc/pacman.d/mirrorlist.backup"
