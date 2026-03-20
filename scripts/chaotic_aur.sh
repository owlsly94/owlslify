#!/usr/bin/env bash
set -euo pipefail

echo "==> Adding Chaotic-AUR..."

# Import key — try primary keyserver first, fall back to openpgp.org
if ! timeout 30 sudo pacman-key --recv-key 3056513887B78AEB \
        --keyserver keyserver.ubuntu.com; then
    echo "Primary keyserver failed, trying keys.openpgp.org..."
    sudo pacman-key --recv-key 3056513887B78AEB \
        --keyserver keys.openpgp.org
fi

sudo pacman-key --lsign-key 3056513887B78AEB

sudo pacman -U --noconfirm \
    'https://cdn-mirror.chaotic.cx/chaotic-aur/chaotic-keyring.pkg.tar.zst'
sudo pacman -U --noconfirm \
    'https://cdn-mirror.chaotic.cx/chaotic-aur/chaotic-mirrorlist.pkg.tar.zst'

if ! grep -q "\[chaotic-aur\]" /etc/pacman.conf; then
    printf '\n[chaotic-aur]\nInclude = /etc/pacman.d/chaotic-mirrorlist\n' \
        | sudo tee -a /etc/pacman.conf
    echo "==> Chaotic-AUR added."
else
    echo "==> Chaotic-AUR already in pacman.conf."
fi

sudo pacman -Sy
echo "==> Done."
