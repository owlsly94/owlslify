#!/usr/bin/env bash
set -euo pipefail
sudo pacman -S --needed --noconfirm apparmor
if command -v grub-mkconfig &>/dev/null; then
    GRUB_CFG=/etc/default/grub
    sudo cp "$GRUB_CFG" "${GRUB_CFG}.bak.$(date +%s)"
    sudo sed -i 's/GRUB_CMDLINE_LINUX_DEFAULT="\(.*\)"/GRUB_CMDLINE_LINUX_DEFAULT="\1 apparmor=1 lsm=landlock,lockdown,yama,integrity,apparmor"/' "$GRUB_CFG"
    sudo grub-mkconfig -o /boot/grub/grub.cfg
fi
sudo systemctl enable --now apparmor.service
echo "==> AppArmor enabled. Reboot for kernel param to take effect."
