#!/bin/bash

# Script to build and run the Chronovyan compiler tests

# Set variables
BUILD_DIR="build"
TEST_BINARY="bin/compiler_tests"

# Print header
echo "====================================================="
echo "  Chronovyan Compiler Tests Builder and Runner"
echo "====================================================="

# Create build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    echo "Creating build directory..."
    mkdir -p "$BUILD_DIR"
fi

# Change to build directory
cd "$BUILD_DIR"

# Configure the build
echo "Configuring build..."
cmake .. -DBUILD_TESTS=ON

# Build the compiler tests
echo "Building compiler tests..."
cmake --build . --target compiler_tests -- -j$(nproc)

# Check if build was successful
if [ $? -ne 0 ]; then
    echo "Build failed. Please check the errors above."
    exit 1
fi

echo "Build successful."

# Run the tests
echo "====================================================="
echo "Running compiler tests..."
echo "====================================================="
./$TEST_BINARY

# Check if tests were successful
if [ $? -ne 0 ]; then
    echo "Tests failed. Please check the errors above."
    exit 1
fi

echo "====================================================="
echo "All compiler tests passed successfully!"
echo "====================================================="

exit 0 