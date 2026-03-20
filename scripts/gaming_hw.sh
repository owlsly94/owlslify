#!/usr/bin/env bash
# Usage: gaming_hw.sh <cpu: amd|intel> <gpu: amd|nvidia|nvidia-open|intel>
set -euo pipefail

CPU="${1:-amd}"
GPU="${2:-amd}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "╔══════════════════════════════════════════════════════╗"
echo "║          owlslify — Gaming Setup                     ║"
echo "║                                                      ║"
printf  "║  CPU: %-46s ║\n" "$CPU"
printf  "║  GPU: %-46s ║\n" "$GPU"
echo "╚══════════════════════════════════════════════════════╝"
echo ""

AUR=${AUR:-yay}
command -v $AUR &>/dev/null || { echo "ERROR: $AUR not found. Install yay first."; exit 1; }

# Ensure multilib
if ! grep -q "^\[multilib\]" /etc/pacman.conf; then
    sudo sed -i '/\[multilib\]/,/Include/s/^#//' /etc/pacman.conf
    sudo pacman -Sy
fi

# --- CPU drivers ---
echo "==> Installing CPU drivers..."
bash "$SCRIPT_DIR/drivers_cpu.sh" "$CPU"

# --- GPU drivers ---
echo "==> Installing GPU drivers..."
bash "$SCRIPT_DIR/drivers_gpu.sh" "$GPU"

# --- Core gaming packages ---
echo "==> Installing gaming stack..."
sudo pacman -S --needed --noconfirm \
    steam \
    gamemode lib32-gamemode \
    gamescope \
    mangohud lib32-mangohud \
    lutris

$AUR -S --needed --noconfirm \
    heroic-games-launcher-bin \
    faugus-launcher \
    protonup-qt

# Wine + deps
sudo pacman -S --needed --noconfirm \
    wine-staging wine-mono wine-gecko winetricks \
    lib32-libpulse lib32-alsa-lib lib32-alsa-plugins \
    lib32-libx11 lib32-libxext lib32-libxrandr \
    lib32-gnutls lib32-mpg123 lib32-openal \
    lib32-vulkan-icd-loader vulkan-icd-loader \
    lib32-vkd3d vkd3d

# --- GPU-specific gaming extras ---
case "$GPU" in
    amd)
        echo "==> AMD gaming extras (lib32-mesa, radeontop, xboxdrv)..."
        sudo pacman -S --needed --noconfirm \
            radeontop \
            xboxdrv
        ;;
    nvidia|nvidia-open)
        echo "==> Nvidia gaming extras..."
        sudo pacman -S --needed --noconfirm \
            lib32-nvidia-utils \
            xboxdrv
        # Steam Proton needs this
        $AUR -S --needed --noconfirm dxvk-bin 2>/dev/null || true
        ;;
    intel)
        echo "==> Intel gaming extras..."
        sudo pacman -S --needed --noconfirm \
            lib32-vulkan-intel \
            xboxdrv
        ;;
esac

# --- Steam config ---
echo "==> Applying Steam download optimisations..."
mkdir -p "$HOME/.steam/steam/"
cat > "$HOME/.steam/steam/steam_dev.cfg" <<'EOF'
@nClientDownloadEnableHTTP2PlatformLinux 0
@fDownloadRateImprovementToAddAnotherConnection 1.0
EOF

echo ""
echo "==> Gaming setup complete!"
echo "    CPU: $CPU | GPU: $GPU"
echo ""
echo "    Tips:"
echo "    • Open ProtonUp-Qt to install Proton-GE"
echo "    • Add 'gamemoderun %command%' to Steam launch options"
echo "    • Add 'MANGOHUD=1 %command%' for the overlay"
echo "    • REBOOT for driver changes to take effect"
