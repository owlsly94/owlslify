#!/usr/bin/env bash
set -euo pipefail

if command -v yay &>/dev/null; then
    echo "==> yay already installed."
    exit 0
fi

echo "==> Installing yay..."

# Try Chaotic-AUR first (fast)
if grep -q "\[chaotic-aur\]" /etc/pacman.conf; then
    sudo pacman -S --needed --noconfirm --ask 4 yay
else
    # Build from AUR
    sudo pacman -S --needed --noconfirm git base-devel
    TMPDIR=$(mktemp -d)
    git clone https://aur.archlinux.org/yay-bin.git "$TMPDIR/yay-bin"
    cd "$TMPDIR/yay-bin"
    makepkg -si --noconfirm
    cd ~
    rm -rf "$TMPDIR"
fi

echo "==> yay installed."
