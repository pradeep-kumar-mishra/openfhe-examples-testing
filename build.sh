#!/bin/bash
# Usage: ./build.sh [1]    builds and runs ckks, ckks_fbt, bfv and bgv; 1 = clean rebuild
set -e

if [ "$1" == "1" ]; then
    rm -rf build
fi

cmake -S . -B build
cmake --build build --parallel
for name in ckks ckks_fbt bfv bgv; do
    echo "===== $name"
    ./build/$name
done
