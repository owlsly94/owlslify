#!/usr/bin/env bash
set -euo pipefail

echo "==> Installing cpupower..."
sudo pacman -S --needed --noconfirm cpupower

echo "==> Setting performance governor in /etc/default/cpupower..."
if grep -q "^#governor=" /etc/default/cpupower 2>/dev/null; then
    sudo sed -i "s/^#governor=.*/governor='performance'/" /etc/default/cpupower
elif grep -q "^governor=" /etc/default/cpupower 2>/dev/null; then
    sudo sed -i "s/^governor=.*/governor='performance'/" /etc/default/cpupower
else
    echo "governor='performance'" | sudo tee -a /etc/default/cpupower
fi

sudo cpupower frequency-set -g performance
sudo systemctl enable --now cpupower

# udev redundancy rule
sudo tee /etc/udev/rules.d/99-cpu-governor.rules > /dev/null <<'EOF'
ACTION=="add", SUBSYSTEM=="cpu", KERNEL=="cpu[0-9]*", ATTR{cpufreq/scaling_governor}="performance"
EOF
sudo udevadm control --reload-rules
sudo udevadm trigger

echo "==> CPU governor set to performance (persistent via cpupower)."
