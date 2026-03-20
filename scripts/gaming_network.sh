#!/usr/bin/env bash
set -euo pipefail

echo "╔══════════════════════════════════════════════════════╗"
echo "║       owlslify — Gaming Network Optimizations        ║"
echo "╚══════════════════════════════════════════════════════╝"
echo ""

# ------------------------------------------------------------------ #
# 1. DNS safety check — never touch DNS if private/homelab           #
# ------------------------------------------------------------------ #
echo "==> Checking DNS configuration..."

DNS_SAFE=true
if [ -f /etc/resolv.conf ]; then
    # Extract nameserver IPs and check for private ranges
    while IFS= read -r line; do
        if [[ "$line" =~ ^nameserver[[:space:]]+([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+) ]]; then
            IP="${BASH_REMATCH[1]}"
            FIRST_OCTET="${IP%%.*}"
            SECOND_OCTET=$(echo "$IP" | cut -d. -f2)

            # RFC1918 private ranges: 192.168.x.x, 10.x.x.x, 172.16-31.x.x
            if [ "$FIRST_OCTET" -eq 10 ]; then
                DNS_SAFE=false
                echo "    Detected homelab DNS: $IP (10.x.x.x range)"
            elif [ "$FIRST_OCTET" -eq 192 ] && [ "$SECOND_OCTET" -eq 168 ]; then
                DNS_SAFE=false
                echo "    Detected homelab DNS: $IP (192.168.x.x range)"
            elif [ "$FIRST_OCTET" -eq 172 ] && [ "$SECOND_OCTET" -ge 16 ] && [ "$SECOND_OCTET" -le 31 ]; then
                DNS_SAFE=false
                echo "    Detected homelab DNS: $IP (172.16-31.x.x range)"
            fi
        fi
    done < /etc/resolv.conf
fi

if [ "$DNS_SAFE" = false ]; then
    echo "    => Private/homelab DNS detected (AdGuard, Pi-hole, etc.)"
    echo "    => Skipping DNS configuration — your setup is preserved."
else
    echo "    => No private DNS detected — DNS left unchanged."
    echo "    => (owlslify never modifies resolv.conf or systemd-resolved)"
fi

echo ""

# ------------------------------------------------------------------ #
# 2. Kernel TCP / network tuning                                      #
# ------------------------------------------------------------------ #
echo "==> Writing /etc/sysctl.d/99-gaming-network.conf..."

sudo tee /etc/sysctl.d/99-gaming-network.conf > /dev/null <<'EOF'
# ------------------------------------------------------------------ #
# owlslify — Gaming Network Optimizations                            #
# Safe kernel TCP/network tuning for improved download throughput.   #
# Does NOT touch DNS, resolv.conf or systemd-resolved.               #
# ------------------------------------------------------------------ #

# Increase socket receive/send buffer maximums (64 MiB)
# Allows the kernel to buffer more data in-flight — critical for
# high-bandwidth connections with any latency (broadband, VPN, etc.)
net.core.rmem_max = 67108864
net.core.wmem_max = 67108864

# Default socket buffer sizes (256 KiB)
# Applied to new sockets before the app sets its own size.
net.core.rmem_default = 262144
net.core.wmem_default = 262144

# TCP receive buffer: min / default / max (32 MiB max)
# Lets TCP auto-tune its window size up to 32 MiB per connection.
net.ipv4.tcp_rmem = 4096 87380 33554432

# TCP send buffer: min / default / max (32 MiB max)
net.ipv4.tcp_wmem = 4096 65536 33554432

# TCP Fast Open — speeds up repeated connections by sending data
# in the SYN packet. 3 = enable for both client and server.
net.ipv4.tcp_fastopen = 3

# Fair Queue scheduler — pairs with BBR to pace packets evenly,
# reducing bufferbloat and improving latency under load.
net.core.default_qdisc = fq

# BBR congestion control — Google's algorithm that models bandwidth
# and RTT instead of reacting to packet loss. Significantly better
# throughput on broadband, especially for large downloads.
net.ipv4.tcp_congestion_control = bbr
EOF

echo "    => Written."
echo ""

# ------------------------------------------------------------------ #
# 3. Apply immediately                                                #
# ------------------------------------------------------------------ #
echo "==> Applying sysctl settings..."
sudo sysctl --system 2>&1 | grep -E "99-gaming|rmem|wmem|tcp_fast|qdisc|congestion" || true
echo ""

# ------------------------------------------------------------------ #
# 4. Verification                                                     #
# ------------------------------------------------------------------ #
echo "==> Verification:"
echo ""

CC=$(sysctl -n net.ipv4.tcp_congestion_control 2>/dev/null || echo "unknown")
QDISC=$(sysctl -n net.core.default_qdisc 2>/dev/null || echo "unknown")
RMEM=$(sysctl -n net.core.rmem_max 2>/dev/null || echo "unknown")
WMEM=$(sysctl -n net.core.wmem_max 2>/dev/null || echo "unknown")
TFO=$(sysctl -n net.ipv4.tcp_fastopen 2>/dev/null || echo "unknown")

printf "    %-40s %s\n" "TCP congestion control:"  "$CC"
printf "    %-40s %s\n" "Default qdisc:"            "$QDISC"
printf "    %-40s %s\n" "Max receive buffer:"       "$RMEM bytes"
printf "    %-40s %s\n" "Max send buffer:"          "$WMEM bytes"
printf "    %-40s %s\n" "TCP Fast Open:"            "$TFO"
echo ""

if [ "$CC" = "bbr" ] && [ "$QDISC" = "fq" ]; then
    echo "    ✔ BBR + FQ active — network optimizations applied successfully."
else
    echo "    ⚠ Settings written but may need a reboot to fully activate."
    echo "      (BBR requires kernel module — run: sudo modprobe tcp_bbr)"
fi

echo ""

# ------------------------------------------------------------------ #
# 5. Steam download optimizations                                     #
# ------------------------------------------------------------------ #
echo "==> Applying Steam download optimizations..."

STEAM_DIRS=(
    "$HOME/.steam/steam"
    "$HOME/.local/share/Steam"
)

STEAM_CFG_CONTENT='@nClientDownloadEnableHTTP2PlatformLinux 0
@fDownloadRateImprovementToAddAnotherConnection 0.01
@cMaxInitialDownloadSources 10'

for DIR in "${STEAM_DIRS[@]}"; do
    mkdir -p "$DIR"
    DEST="$DIR/steam_dev.cfg"

    if [ -f "$DEST" ]; then
        # Back up existing cfg
        cp "$DEST" "${DEST}.bak.$(date +%s)"
        echo "    Backed up existing: $DEST"
    fi

    echo "$STEAM_CFG_CONTENT" > "$DEST"
    echo "    ✔ Written: $DEST"
done

echo ""

# ------------------------------------------------------------------ #
# 6. Summary                                                          #
# ------------------------------------------------------------------ #
echo "╔══════════════════════════════════════════════════════╗"
echo "║              What was optimized:                     ║"
echo "╠══════════════════════════════════════════════════════╣"
echo "║  TCP buffers    64 MiB max — more data in-flight     ║"
echo "║  BBR + FQ       Better throughput, less bufferbloat  ║"
echo "║  TCP Fast Open  Faster reconnects to known servers   ║"
echo "║  Steam cfg      HTTP/2 off, 10 sources, better rate  ║"
echo "║  DNS            NOT touched (your config preserved)  ║"
echo "╚══════════════════════════════════════════════════════╝"
echo ""
echo "No reboot required — changes are live immediately."
echo "(Steam cfg takes effect on next Steam launch)"
