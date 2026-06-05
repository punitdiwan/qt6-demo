#!/usr/bin/env bash
# Build script for HelloQt6
# Usage:
#   ./build.sh                          # Linux release build
#   ./build.sh Debug                    # Linux debug build
#   ./build.sh Release windows QT_ROOT=/path/to/Qt/6.x.x/mingw_64
#                                       # Cross-compile to Windows .exe

set -e

BUILD_TYPE="${1:-Release}"
TARGET="${2:-linux}"
shift 2 2>/dev/null || true   # consume first two args; ignore if missing

if [[ "$TARGET" == "windows" ]]; then
    BUILD_DIR="build-windows"
    EXTRA_CMAKE_ARGS=(-DCMAKE_TOOLCHAIN_FILE="cmake/toolchain-mingw64.cmake")
    # Allow QT_ROOT=... to be passed as a trailing argument
    for arg in "$@"; do
        if [[ "$arg" == QT_ROOT=* ]]; then
            EXTRA_CMAKE_ARGS+=("-D${arg}")
        fi
    done
    OUT_BIN="${BUILD_DIR}/HelloQt6.exe"
else
    BUILD_DIR="build"
    EXTRA_CMAKE_ARGS=()
    OUT_BIN="${BUILD_DIR}/HelloQt6"
fi

mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

cmake .. \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    "${EXTRA_CMAKE_ARGS[@]}"

cmake --build . --parallel "$(nproc)"

echo ""
echo "Build complete. Binary: ${OUT_BIN}"
