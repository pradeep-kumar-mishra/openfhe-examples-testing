#!/bin/bash
# Builds OpenFHE in Debug (-O0, symbols, OpenMP off) next to this repo:
#   ../openfhe-development   source (kept: the debug info points here)
#   ../openfhe-install       install prefix used by CMakeLists.txt
# Usage: ./setup_openfhe.sh [git tag]   (default v1.6.1)
set -e

VERSION=${1:-v1.6.1}
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="$ROOT/openfhe-development"
PREFIX="$ROOT/openfhe-install"

if [ ! -d "$SRC" ]; then
    git clone --depth 1 --branch "$VERSION" https://github.com/openfheorg/openfhe-development.git "$SRC"
fi

cmake -S "$SRC" -B "$SRC/build" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_CXX_FLAGS_DEBUG="-O0 -g3 -fno-omit-frame-pointer" \
    -DCMAKE_INSTALL_PREFIX="$PREFIX" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DWITH_OPENMP=OFF \
    -DBUILD_UNITTESTS=OFF -DBUILD_BENCHMARKS=OFF -DBUILD_EXAMPLES=OFF
cmake --build "$SRC/build" --parallel "$(sysctl -n hw.ncpu 2>/dev/null || nproc)"
cmake --install "$SRC/build"

echo "OpenFHE installed to $PREFIX"
