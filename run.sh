#!/bin/bash

# Install Dependencies

# Function to check and install a brew package if not installed
install_if_missing() {
    if ! brew ls --versions "$1" > /dev/null; then
        echo "Installing $1..."
        brew install "$1"
    fi
}

# Install necessary dependencies
install_if_missing cmake
install_if_missing openssl
install_if_missing boost
install_if_missing curl
install_if_missing curlpp
install_if_missing websocketpp

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
