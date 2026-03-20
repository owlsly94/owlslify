#!/usr/bin/env bash
set -euo pipefail

echo "==> Backing up /etc/pacman.conf..."
sudo cp /etc/pacman.conf /etc/pacman.conf.backup

echo "==> Applying pacman beautification..."
# Color + ILoveCandy
sudo sed -i 's/^#Color/Color\nILoveCandy/' /etc/pacman.conf
# CheckSpace
sudo sed -i 's/^#CheckSpace/CheckSpace/' /etc/pacman.conf
# VerbosePkgLists
sudo sed -i 's/^#VerbosePkgLists/VerbosePkgLists/' /etc/pacman.conf
# ParallelDownloads = 10
sudo sed -i 's/^#ParallelDownloads = 5/ParallelDownloads = 10/' /etc/pacman.conf

echo "==> Done. Backup saved to /etc/pacman.conf.backup"
