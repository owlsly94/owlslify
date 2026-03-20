#!/usr/bin/env bash
set -euo pipefail

RAW="https://raw.githubusercontent.com/owlsly94/dotfiles/main"
REPO="owlsly94/dotfiles"

echo "==> Installing Node / npm / degit if needed..."
command -v node &>/dev/null || sudo pacman -S --needed --noconfirm nodejs npm
npm list -g degit &>/dev/null || sudo npm install -g degit

CONFIG_FOLDERS=(
    hypr kitty alacritty dunst fastfetch
    mpv MangoHud nvim nwg-look pypr
    ranger wallpapers waybar wlogout wofi rofi
)

echo "==> Pulling config folders..."
for folder in "${CONFIG_FOLDERS[@]}"; do
    DEST="$HOME/.config/$folder"
    if [ -d "$DEST" ]; then
        mv "$DEST" "${DEST}.bak.$(date +%s)"
        echo "  Backed up $folder"
    fi
    if npx --yes degit "$REPO/.config/$folder" "$DEST" --force; then
        echo "  ✔ $folder"
    else
        echo "  ✘ $folder (skipped)"
    fi
done

echo "==> Pulling single config files..."

# starship.toml
curl -fLo "$HOME/.config/starship.toml" --create-dirs \
    "$RAW/.config/starship.toml" && echo "  ✔ starship.toml" \
    || echo "  ✘ starship.toml (skipped)"

# chrome-flags.conf
curl -fLo "$HOME/.config/chrome-flags.conf" --create-dirs \
    "$RAW/.config/chrome-flags.conf" && echo "  ✔ chrome-flags.conf" \
    || echo "  ✘ chrome-flags.conf (skipped)"

# .zshrc
[ -f "$HOME/.zshrc" ] && mv "$HOME/.zshrc" "$HOME/.zshrc.bak.$(date +%s)"
curl -fLo "$HOME/.zshrc" "$RAW/.zshrc" && echo "  ✔ .zshrc" \
    || echo "  ✘ .zshrc (skipped)"

# zsh archive
ZSH_URL="https://github.com/owlsly94/dotfiles/raw/refs/heads/main/.config/zsh.tar.gz"
TMP="/tmp/owlslify-zsh-$(date +%s).tar.gz"
if curl -fLo "$TMP" "$ZSH_URL"; then
    [ -d "$HOME/.config/zsh" ] && mv "$HOME/.config/zsh" "$HOME/.config/zsh.bak.$(date +%s)"
    tar -xzf "$TMP" -C "$HOME/.config/"
    rm -f "$TMP"
    echo "  ✔ zsh archive"
else
    echo "  ✘ zsh archive (skipped)"
fi

echo ""
echo "==> All dotfiles pulled!"
