#!/usr/bin/env bash
set -euo pipefail

echo "==> Setting up paccache cleanup hook..."
sudo mkdir -p /etc/pacman.d/hooks

sudo tee /etc/pacman.d/hooks/clean-cache.hook > /dev/null <<'EOF'
[Trigger]
Operation = Upgrade
Operation = Install
Operation = Remove
Type = Package
Target = *

[Action]
Description = Cleaning pacman cache (keeping last 2 versions)...
When = PostTransaction
Exec = /usr/bin/paccache -rk2
EOF

echo "==> Paccache hook installed."
