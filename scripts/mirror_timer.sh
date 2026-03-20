#!/usr/bin/env bash
set -euo pipefail

echo "==> Creating rate-mirrors systemd service + monthly timer..."

sudo tee /etc/systemd/system/rate-mirrors.service > /dev/null <<'EOF'
[Unit]
Description=Update mirrorlist with rate-mirrors
After=network-online.target
Wants=network-online.target

[Service]
Type=oneshot
ExecStart=/usr/bin/rate-mirrors --protocol https arch --max-delay 3600 --save /etc/pacman.d/mirrorlist
EOF

sudo tee /etc/systemd/system/rate-mirrors.timer > /dev/null <<'EOF'
[Unit]
Description=Update mirrorlist monthly

[Timer]
OnCalendar=monthly
Persistent=true

[Install]
WantedBy=timers.target
EOF

sudo systemctl daemon-reload
sudo systemctl enable rate-mirrors.timer
echo "==> Monthly mirror timer enabled."
