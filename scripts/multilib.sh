#!/usr/bin/env bash
set -euo pipefail

echo "==> Enabling [multilib]..."

if grep -q "^\[multilib\]" /etc/pacman.conf; then
    echo "==> [multilib] already enabled."
else
    sudo sed -i '/\[multilib\]/,/Include/s/^#//' /etc/pacman.conf
    echo "==> [multilib] enabled."
fi

sudo pacman -Sy
echo "==> Done."
