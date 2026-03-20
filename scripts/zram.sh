#!/usr/bin/env bash
set -euo pipefail

echo "==> Installing zram-generator..."
sudo pacman -S --needed --noconfirm zram-generator

echo "==> Writing /etc/systemd/zram-generator.conf..."
sudo tee /etc/systemd/zram-generator.conf > /dev/null <<'EOF'
[zram0]
zram-size = ram / 2
compression-algorithm = zstd
EOF

sudo systemctl daemon-reload
sudo systemctl start /dev/zram0 2>/dev/null || true
echo "==> zram configured. Current status:"
zramctl 2>/dev/null || echo "(zramctl not available yet — reboot to activate)"
