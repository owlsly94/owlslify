#!/usr/bin/env bash
set -euo pipefail
sudo pacman -S --needed --noconfirm ufw
sudo ufw default deny incoming
sudo ufw default allow outgoing
sudo ufw allow ssh
sudo ufw --force enable
sudo systemctl enable ufw.service
sudo ufw status verbose
echo "==> UFW enabled."
