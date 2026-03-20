#!/usr/bin/env bash
set -euo pipefail

echo "==> Writing kernel parameters to /etc/sysctl.d/99-desktop.conf..."

sudo tee /etc/sysctl.d/99-desktop.conf > /dev/null <<'EOF'
# Reduce swappiness for desktop use
vm.swappiness=10

# Reduce inode/dentry cache pressure
vm.vfs_cache_pressure=50

# Better TCP congestion control
net.core.default_qdisc=fq
net.ipv4.tcp_congestion_control=bbr
EOF

sudo sysctl --system
echo "==> Kernel parameters applied."
