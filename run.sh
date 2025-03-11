#!/bin/bash

# Set build directory
BUILD_DIR="build"
EXECUTABLE_NAME="trading_system"  # Change this to your executable name

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    mkdir "$BUILD_DIR"
fi

# Navigate to build directory
cd "$BUILD_DIR" || exit

# Run CMake to configure the project
cmake ..

# Build the project
cmake --build .

# Check if build was successful
if [ $? -eq 0 ]; then
    clear;
    ./"$EXECUTABLE_NAME"  # Run the compiled executable
else
    echo "Build failed. Check errors above."
fi
