#!/usr/bin/env bash
set -e
PKG_DIR="$(cd "$(dirname "$0")" && pwd)"
if [ -f "$PKG_DIR/bin/nova" ]; then SRC="$PKG_DIR/bin/nova"
elif [ -f "$PKG_DIR/../build/nova" ]; then SRC="$PKG_DIR/../build/nova"
else echo "Error: nova binary not found"; exit 1; fi

mkdir -p "$HOME/.local/bin" "$HOME/.local/share/applications" "$HOME/.local/share/icons/hicolor/scalable/apps"
cp "$SRC" "$HOME/.local/bin/nova"
chmod +x "$HOME/.local/bin/nova"
for d in "$PKG_DIR/share/applications/nova.desktop" "$PKG_DIR/../assets/nova.desktop"; do
  [ -f "$d" ] && cp "$d" "$HOME/.local/share/applications/nova.desktop" && break
done
for d in "$PKG_DIR/share/icons/hicolor/scalable/apps/nova.svg" "$PKG_DIR/../assets/nova.svg"; do
  [ -f "$d" ] && cp "$d" "$HOME/.local/share/icons/hicolor/scalable/apps/nova.svg" && break
done
gtk-update-icon-cache -f -t "$HOME/.local/share/icons/hicolor" 2>/dev/null || true
update-desktop-database "$HOME/.local/share/applications" 2>/dev/null || true
echo "=== Nova installed. Run: nova ==="
