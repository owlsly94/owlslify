#!/usr/bin/env bash
set -euo pipefail

echo "==> Enabling subpixel RGB + LCD filter font rendering..."
sudo mkdir -p /etc/fonts/conf.d

sudo ln -sf /usr/share/fontconfig/conf.avail/10-sub-pixel-rgb.conf \
    /etc/fonts/conf.d/10-sub-pixel-rgb.conf

sudo ln -sf /usr/share/fontconfig/conf.avail/11-lcdfilter-default.conf \
    /etc/fonts/conf.d/11-lcdfilter-default.conf

echo "==> Font rendering improved. Log out and back in to see effect."
