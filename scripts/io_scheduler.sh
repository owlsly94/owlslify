#!/usr/bin/env bash
set -euo pipefail

echo "==> Writing I/O scheduler udev rules..."

sudo tee /etc/udev/rules.d/60-ioschedulers.rules > /dev/null <<'EOF'
# NVMe — no scheduler needed (hardware queue)
ACTION=="add|change", KERNEL=="nvme[0-9]n[0-9]", ATTR{queue/scheduler}="none"
# SSD / eMMC — mq-deadline
ACTION=="add|change", KERNEL=="sd[a-z]|mmcblk[0-9]*", ATTR{queue/rotational}=="0", ATTR{queue/scheduler}="mq-deadline"
# HDD — bfq
ACTION=="add|change", KERNEL=="sd[a-z]", ATTR{queue/rotational}=="1", ATTR{queue/scheduler}="bfq"
EOF

sudo udevadm control --reload-rules
sudo udevadm trigger
echo "==> I/O scheduler rules applied."
