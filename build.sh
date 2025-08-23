#!/bin/bash

if [ "$1" == "1" ]; then
    echo "Removing build directory and reconfiguring..."
    rm -rf build
    cmake -S . -B build
fi

echo "Building the project..."
cmake --build build --parallel
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
./build/example