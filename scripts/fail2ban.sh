#!/usr/bin/env bash
set -euo pipefail
sudo pacman -S --needed --noconfirm fail2ban
sudo tee /etc/fail2ban/jail.local > /dev/null <<'EOF'
[DEFAULT]
bantime  = 1h
findtime = 10m
maxretry = 5

[sshd]
enabled  = true
port     = ssh
logpath  = %(sshd_log)s
backend  = %(sshd_backend)s
EOF
sudo systemctl enable --now fail2ban.service
echo "==> fail2ban enabled with SSH jail."
