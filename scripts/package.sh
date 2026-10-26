#!/usr/bin/env bash
set -e
VERSION="${1:-v0.1.0}"
REPO="$(cd "$(dirname "$0")/.." && pwd)"
NAME="nova-linux-${VERSION}"
DIR="${REPO}/dist/${NAME}"

if [ ! -f "${REPO}/build/nova" ]; then
  echo "Error: build/nova not found. Build first."
  exit 1
fi

rm -rf "${DIR}"
mkdir -p "${DIR}/bin" "${DIR}/share/applications" "${DIR}/share/icons/hicolor/scalable/apps" "${DIR}/src"
cp "${REPO}/build/nova" "${DIR}/bin/"
chmod +x "${DIR}/bin/nova"
cp "${REPO}/assets/nova.desktop" "${DIR}/share/applications/"
cp "${REPO}/assets/nova.svg" "${DIR}/share/icons/hicolor/scalable/apps/"
cp -r "${REPO}/src/"* "${DIR}/src/"
cp "${REPO}/CMakeLists.txt" "${DIR}/src/"
cp "${REPO}/scripts/install.sh" "${DIR}/"
chmod +x "${DIR}/install.sh"
cd "${REPO}/dist" && tar -czf "${NAME}.tar.gz" "${NAME}"
echo "=== Packaged: dist/${NAME}.tar.gz ==="
ls -lh "${REPO}/dist/${NAME}.tar.gz"
