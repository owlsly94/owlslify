#!/usr/bin/env bash
set -euo pipefail

echo "==> Configuring /etc/environment for AMD GPU + Wayland..."

[ -f /etc/environment ] && sudo cp /etc/environment /etc/environment.backup

sudo tee /etc/environment > /dev/null <<'EOF'
# AMD / RADV Vulkan
AMD_VULKAN_ICD=RADV
RADV_PERFTEST=gpl,transfer_queue

# VA-API / VDPAU hardware decode
LIBVA_DRIVER_NAME=radeonsi
VDPAU_DRIVER=radeonsi

# Mesa shader cache
MESA_SHADER_CACHE_DISABLE=false
MESA_DISK_CACHE_SINGLE_FILE=1

# Wayland flags
MOZ_ENABLE_WAYLAND=1
ELECTRON_OZONE_PLATFORM_HINT=wayland
QT_QPA_PLATFORM=wayland
SDL_VIDEODRIVER=wayland
EOF

echo "==> /etc/environment written. Reboot or re-login for effect."
