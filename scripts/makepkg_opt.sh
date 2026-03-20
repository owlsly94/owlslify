#!/usr/bin/env bash
set -euo pipefail

CORES=$(nproc)
echo "==> Optimising makepkg for $CORES cores..."

sudo cp /etc/makepkg.conf /etc/makepkg.conf.backup

# MAKEFLAGS
if grep -q "^#MAKEFLAGS=" /etc/makepkg.conf; then
    sudo sed -i "s|^#MAKEFLAGS=.*|MAKEFLAGS=\"-j${CORES}\"|" /etc/makepkg.conf
elif grep -q "^MAKEFLAGS=" /etc/makepkg.conf; then
    sudo sed -i "s|^MAKEFLAGS=.*|MAKEFLAGS=\"-j${CORES}\"|" /etc/makepkg.conf
else
    echo "MAKEFLAGS=\"-j${CORES}\"" | sudo tee -a /etc/makepkg.conf
fi

# Multi-threaded xz
if grep -q "^COMPRESSXZ=" /etc/makepkg.conf; then
    sudo sed -i "s|^COMPRESSXZ=.*|COMPRESSXZ=(xz -c -z - --threads=0)|" /etc/makepkg.conf
fi

# Multi-threaded zstd
if grep -q "^COMPRESSZST=" /etc/makepkg.conf; then
    sudo sed -i "s|^COMPRESSZST=.*|COMPRESSZST=(zstd -c -z -q - --threads=0)|" /etc/makepkg.conf
fi

echo "==> makepkg optimised. Backup at /etc/makepkg.conf.backup"
