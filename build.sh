#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build with make using all available cores
make -j$(nproc)

echo "Build complete! Run ./build/terrain3d to start the application."