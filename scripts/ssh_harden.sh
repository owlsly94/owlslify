#!/usr/bin/env bash
set -euo pipefail

SSHD=/etc/ssh/sshd_config
sudo cp "$SSHD" "${SSHD}.bak.$(date +%s)"

sudo sed -i 's/^#\?PermitRootLogin.*/PermitRootLogin no/'                  "$SSHD"
sudo sed -i 's/^#\?PasswordAuthentication.*/PasswordAuthentication no/'     "$SSHD"
sudo sed -i 's/^#\?ChallengeResponseAuthentication.*/ChallengeResponseAuthentication no/' "$SSHD"
sudo sed -i 's/^#\?X11Forwarding.*/X11Forwarding no/'                       "$SSHD"

sudo systemctl restart sshd
echo "==> SSH hardened. Make sure your SSH key is deployed before logging out!"
