#!/usr/bin/env bash
set -euo pipefail

ZSH_URL="https://github.com/owlsly94/dotfiles/raw/refs/heads/main/.config/zsh.tar.gz"
TMP="/tmp/owlslify-zsh-$(date +%s).tar.gz"

echo "==> Downloading zsh config archive..."
curl -fLo "$TMP" "$ZSH_URL"

if [ -d "$HOME/.config/zsh" ]; then
    mv "$HOME/.config/zsh" "$HOME/.config/zsh.bak.$(date +%s)"
    echo "==> Backed up existing ~/.config/zsh"
fi

tar -xzf "$TMP" -C "$HOME/.config/"
rm -f "$TMP"
echo "==> zsh config extracted to ~/.config/zsh"
