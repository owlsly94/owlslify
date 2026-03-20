#!/usr/bin/env bash
# Usage: drivers_cpu.sh <amd|intel>
set -euo pipefail

CPU="${1:-amd}"

case "$CPU" in
    amd)
        echo "==> Installing AMD microcode..."
        sudo pacman -S --needed --noconfirm amd-ucode
        ;;
    intel)
        echo "==> Installing Intel microcode + media driver..."
        sudo pacman -S --needed --noconfirm intel-ucode intel-media-driver
        ;;
    *)
        echo "ERROR: unknown CPU vendor '$CPU'" >&2
        exit 1
        ;;
esac

echo "==> CPU drivers done."
