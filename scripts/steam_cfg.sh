#!/usr/bin/env bash
set -euo pipefail

echo "==> Applying Steam download optimisations..."
mkdir -p "$HOME/.steam/steam/"
cat > "$HOME/.steam/steam/steam_dev.cfg" <<'EOF'
@nClientDownloadEnableHTTP2PlatformLinux 0
@fDownloadRateImprovementToAddAnotherConnection 1.0
EOF

echo "==> steam_dev.cfg written."
