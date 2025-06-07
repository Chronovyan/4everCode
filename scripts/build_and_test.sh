#!/bin/bash

# Script to build the Chronovyan project and run the enhanced REWIND_FLOW tests

echo "=== Building Chronovyan with Enhanced REWIND_FLOW Controls ==="

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Debug
if [ $? -ne 0 ]; then
    echo "CMake configuration failed. Exiting."
    exit 1
fi

# Build the project
echo "=== Compiling the project ==="
make -j4
if [ $? -ne 0 ]; then
    echo "Compilation failed. Exiting."
    exit 1
fi

# Run the rewind flow tests
echo "=== Running Enhanced REWIND_FLOW Tests ==="
./test/rewind_flow_test

echo "=== All tests completed ===" 