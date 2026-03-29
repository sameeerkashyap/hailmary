#!/usr/bin/env bash
# Download Apple's metal-cpp from the official source and extract to vendor/metal-cpp/.
# Run from the repository root: bash scripts/fetch_metal_cpp.sh
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(dirname "$SCRIPT_DIR")"
DEST="$REPO_ROOT/vendor/metal-cpp"

# metal-cpp is bundled inside the Metal developer package.
# The URL below points to the latest release available at time of writing.
# If it 404s, visit https://developer.apple.com/metal/cpp/ and update the URL.
METAL_CPP_URL="https://developer.apple.com/metal/cpp/files/metal-cpp_macOS15_iOS18.zip"
ZIP_FILE="/tmp/metal-cpp.zip"
UNZIP_DIR="/tmp/metal-cpp-extracted"

echo "[PHM] Downloading metal-cpp..."
curl -L --progress-bar -o "$ZIP_FILE" "$METAL_CPP_URL"

echo "[PHM] Extracting..."
rm -rf "$UNZIP_DIR"
unzip -q "$ZIP_FILE" -d "$UNZIP_DIR"

# Find the extracted directory (usually named metal-cpp or metal-cpp_macOS*)
EXTRACTED=$(find "$UNZIP_DIR" -maxdepth 1 -type d -name "metal-cpp*" | head -1)
if [[ -z "$EXTRACTED" ]]; then
    EXTRACTED="$UNZIP_DIR"
fi

mkdir -p "$DEST"
cp -r "$EXTRACTED"/* "$DEST/"
rm -rf "$UNZIP_DIR" "$ZIP_FILE"

echo "[PHM] metal-cpp installed to: $DEST"
echo "[PHM] Verify: ls $DEST/Metal/Metal.hpp"
ls "$DEST/Metal/Metal.hpp" 2>/dev/null && echo "[PHM] OK" || echo "[PHM] WARNING: Metal.hpp not found — check extraction path"
