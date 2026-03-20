#!/usr/bin/env bash
set -euo pipefail

echo "==> Configuring libvirt / QEMU..."
sudo cp /etc/libvirt/libvirtd.conf /etc/libvirt/libvirtd.conf.backup

sudo sed -i 's/^#unix_sock_group = "libvirt"/unix_sock_group = "libvirt"/' \
    /etc/libvirt/libvirtd.conf
sudo sed -i 's/^#unix_sock_rw_perms = "0770"/unix_sock_rw_perms = "0770"/' \
    /etc/libvirt/libvirtd.conf

sudo systemctl enable --now libvirtd.service
sudo usermod -aG libvirt "$USER"

echo "==> libvirt configured. Added $USER to libvirt group."
echo "    Log out and back in for group membership to take effect."
