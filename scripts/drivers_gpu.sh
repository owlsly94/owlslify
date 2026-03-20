#!/usr/bin/env bash
# Usage: drivers_gpu.sh <amd|nvidia|nvidia-open|intel>
set -euo pipefail

GPU="${1:-amd}"

case "$GPU" in
    # ---------------------------------------------------------------- #
    # AMD                                                               #
    # ---------------------------------------------------------------- #
    amd)
        echo "==> Installing AMD GPU drivers..."

        # Ensure multilib
        if ! grep -q "^\[multilib\]" /etc/pacman.conf; then
            sudo sed -i '/\[multilib\]/,/Include/s/^#//' /etc/pacman.conf
            sudo pacman -Sy
        fi

        # Use mesa-tkg-git from Chaotic-AUR if available, else mesa
        if grep -q "\[chaotic-aur\]" /etc/pacman.conf; then
            sudo pacman -S --needed --noconfirm --ask 4 \
                mesa-tkg-git \
                lib32-mesa-tkg-git
        else
            sudo pacman -S --needed --noconfirm \
                mesa \
                lib32-mesa
        fi

        sudo pacman -S --needed --noconfirm \
            xf86-video-amdgpu \
            xf86-video-ati \
            libva-mesa-driver \
            lib32-libva-mesa-driver \
            vdpauinfo \
            radeontop

        # vulkan-radeon conflicts with mesa-tkg-git (which already includes RADV).
        # Only install it if mesa-tkg-git is NOT present.
        if ! pacman -Q mesa-tkg-git &>/dev/null; then
            sudo pacman -S --needed --noconfirm \
                vulkan-radeon \
                lib32-vulkan-radeon
        else
            echo "==> mesa-tkg-git detected — skipping vulkan-radeon (already included)"
        fi

        # ROCm for compute / GPGPU
        if command -v yay &>/dev/null; then
            yay -S --needed --noconfirm \
                rocm-opencl-runtime \
                rocm-smi-lib \
                lact \
                amf-headers
        fi

        # /etc/environment for AMD
        echo "==> Writing AMD /etc/environment..."
        [ -f /etc/environment ] && sudo cp /etc/environment /etc/environment.backup
        sudo tee /etc/environment > /dev/null <<'EOF'
AMD_VULKAN_ICD=RADV
RADV_PERFTEST=gpl,transfer_queue
LIBVA_DRIVER_NAME=radeonsi
VDPAU_DRIVER=radeonsi
MESA_SHADER_CACHE_DISABLE=false
MESA_DISK_CACHE_SINGLE_FILE=1
MOZ_ENABLE_WAYLAND=1
ELECTRON_OZONE_PLATFORM_HINT=wayland
QT_QPA_PLATFORM=wayland
SDL_VIDEODRIVER=wayland
EOF
        ;;

    # ---------------------------------------------------------------- #
    # Nvidia — proprietary                                              #
    # ---------------------------------------------------------------- #
    nvidia)
        echo "==> Installing Nvidia proprietary drivers..."

        if ! grep -q "^\[multilib\]" /etc/pacman.conf; then
            sudo sed -i '/\[multilib\]/,/Include/s/^#//' /etc/pacman.conf
            sudo pacman -Sy
        fi

        sudo pacman -S --needed --noconfirm \
            nvidia-dkms \
            nvidia-utils \
            lib32-nvidia-utils \
            nvidia-settings \
            vulkan-icd-loader \
            lib32-vulkan-icd-loader \
            libva-utils

        # Add nvidia_drm.modeset=1 to kernel params
        if command -v grub-mkconfig &>/dev/null; then
            sudo sed -i 's/GRUB_CMDLINE_LINUX_DEFAULT="\(.*\)"/GRUB_CMDLINE_LINUX_DEFAULT="\1 nvidia_drm.modeset=1"/' \
                /etc/default/grub
            sudo grub-mkconfig -o /boot/grub/grub.cfg
        fi

        # /etc/environment for Nvidia + Wayland
        echo "==> Writing Nvidia /etc/environment..."
        [ -f /etc/environment ] && sudo cp /etc/environment /etc/environment.backup
        sudo tee /etc/environment > /dev/null <<'EOF'
LIBVA_DRIVER_NAME=nvidia
MOZ_ENABLE_WAYLAND=1
ELECTRON_OZONE_PLATFORM_HINT=wayland
QT_QPA_PLATFORM=wayland
SDL_VIDEODRIVER=wayland
GBM_BACKEND=nvidia-drm
__GLX_VENDOR_LIBRARY_NAME=nvidia
EOF
        ;;

    # ---------------------------------------------------------------- #
    # Nvidia — open kernel module                                       #
    # ---------------------------------------------------------------- #
    nvidia-open)
        echo "==> Installing Nvidia open kernel drivers (Turing+ only)..."

        if ! grep -q "^\[multilib\]" /etc/pacman.conf; then
            sudo sed -i '/\[multilib\]/,/Include/s/^#//' /etc/pacman.conf
            sudo pacman -Sy
        fi

        sudo pacman -S --needed --noconfirm \
            nvidia-open-dkms \
            nvidia-utils \
            lib32-nvidia-utils \
            nvidia-settings \
            vulkan-icd-loader \
            lib32-vulkan-icd-loader

        if command -v grub-mkconfig &>/dev/null; then
            sudo sed -i 's/GRUB_CMDLINE_LINUX_DEFAULT="\(.*\)"/GRUB_CMDLINE_LINUX_DEFAULT="\1 nvidia_drm.modeset=1"/' \
                /etc/default/grub
            sudo grub-mkconfig -o /boot/grub/grub.cfg
        fi

        [ -f /etc/environment ] && sudo cp /etc/environment /etc/environment.backup
        sudo tee /etc/environment > /dev/null <<'EOF'
LIBVA_DRIVER_NAME=nvidia
MOZ_ENABLE_WAYLAND=1
ELECTRON_OZONE_PLATFORM_HINT=wayland
QT_QPA_PLATFORM=wayland
SDL_VIDEODRIVER=wayland
GBM_BACKEND=nvidia-drm
__GLX_VENDOR_LIBRARY_NAME=nvidia
EOF
        ;;

    # ---------------------------------------------------------------- #
    # Intel                                                             #
    # ---------------------------------------------------------------- #
    intel)
        echo "==> Installing Intel GPU drivers..."

        if ! grep -q "^\[multilib\]" /etc/pacman.conf; then
            sudo sed -i '/\[multilib\]/,/Include/s/^#//' /etc/pacman.conf
            sudo pacman -Sy
        fi

        sudo pacman -S --needed --noconfirm \
            xf86-video-intel \
            mesa \
            lib32-mesa \
            vulkan-intel \
            lib32-vulkan-intel \
            intel-media-driver \
            libva-utils

        [ -f /etc/environment ] && sudo cp /etc/environment /etc/environment.backup
        sudo tee /etc/environment > /dev/null <<'EOF'
LIBVA_DRIVER_NAME=iHD
MOZ_ENABLE_WAYLAND=1
ELECTRON_OZONE_PLATFORM_HINT=wayland
QT_QPA_PLATFORM=wayland
SDL_VIDEODRIVER=wayland
EOF
        ;;

    *)
        echo "ERROR: unknown GPU vendor '$GPU'" >&2
        exit 1
        ;;
esac

echo "==> GPU drivers done. Please reboot for all changes to take effect."
