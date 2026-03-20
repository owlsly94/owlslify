#!/usr/bin/env bash
set -euo pipefail

[ -f /etc/sddm.conf ] && sudo cp /etc/sddm.conf /etc/sddm.conf.backup

echo "==> Configuring SDDM autologin for $USER → hyprland..."
sudo tee /etc/sddm.conf > /dev/null <<EOF
[Autologin]
User=$USER
Session=hyprland
EOF

sudo systemctl enable sddm.service
echo "==> SDDM autologin configured."
