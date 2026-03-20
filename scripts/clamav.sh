#!/usr/bin/env bash
set -euo pipefail
sudo pacman -S --needed --noconfirm clamav
sudo freshclam
sudo systemctl enable --now clamav-daemon.service
echo "==> ClamAV ready."
