#!/bin/bash
#
# Chronovyan Build Script
#
# A cross-platform build script for Chronovyan that handles configuration,
# building, and testing in a single command.
#
# Usage: ./scripts/build.sh [options]
#
# Options:
#   --build-type TYPE      Build type (Debug, Release, RelWithDebInfo, MinSizeRel). Default: Release
#   --build-dir DIR        Build directory. Default: ./build
#   --generator GEN        CMake generator to use. Auto-detected if not specified.
#   --target TARGET       Build target (e.g., all, install, test). Default: all
#   --clean                Clean the build directory before building.
#   --[no-]tests          Enable/disable building tests. Default: enabled
#   --[no-]examples       Enable/disable building examples. Default: enabled
#   --[no-]docs           Enable/disable building documentation. Default: disabled
#   --vcpkg MODE          Use vcpkg (AUTO/ON/OFF). Default: AUTO
#   -h, --help            Show this help message and exit
#

set -euo pipefail

# Default values
BUILD_TYPE="Release"
BUILD_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/build"
GENERATOR=""
TARGET="all"
CLEAN=false
ENABLE_TESTS=true
ENABLE_EXAMPLES=true
ENABLE_DOCS=false
USE_VCPKG="AUTO"

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    case "$1" in
        --build-type)
            BUILD_TYPE="$2"
            shift 2
            ;;
        --build-dir)
            BUILD_DIR="$2"
            shift 2
            ;;
        --generator)
            GENERATOR="$2"
            shift 2
            ;;
        --target)
            TARGET="$2"
            shift 2
            ;;
        --clean)
            CLEAN=true
            shift
            ;;
        --tests)
            ENABLE_TESTS=true
            shift
            ;;
        --no-tests)
            ENABLE_TESTS=false
            shift
            ;;
        --examples)
            ENABLE_EXAMPLES=true
            shift
            ;;
        --no-examples)
            ENABLE_EXAMPLES=false
            shift
            ;;
        --docs)
            ENABLE_DOCS=true
            shift
            ;;
        --no-docs)
            ENABLE_DOCS=false
            shift
            ;;
        --vcpkg)
            USE_VCPKG="$2"
            shift 2
            ;;
        -h|--help)
            grep '^#\($\| \)' "$0" | cut -c3-
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

# Convert to absolute path
BUILD_DIR="$(cd "$(dirname "$BUILD_DIR")" && pwd)/$(basename "$BUILD_DIR")"
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"

# Detect platform
UNAME_OUT="$(uname -s)"
case "${UNAME_OUT}" in
    Linux*)     PLATFORM=Linux ;;
    Darwin*)    PLATFORM=MacOS ;;
    CYGWIN*|MINGW*|MSYS*) PLATFORM=Windows ;;
    *)          PLATFORM="UNKNOWN:${UNAME_OUT}" ;;
esac

# Set default generator if not specified
if [ -z "$GENERATOR" ]; then
    case "$PLATFORM" in
        Windows)
            GENERATOR="Visual Studio 16 2019"
            ;;
        *)
            if command -v ninja >/dev/null 2>&1; then
                GENERATOR="Ninja"
            else
                GENERATOR="Unix Makefiles"
            fi
            ;;
    esac
fi

# Clean build directory if requested
if [ "$CLEAN" = true ] && [ -d "$BUILD_DIR" ]; then
    echo -e "\033[33mCleaning build directory: $BUILD_DIR\033[0m"
    rm -rf "$BUILD_DIR"
fi

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Common CMake arguments
CMAKE_ARGS=(
    "-G" "$GENERATOR"
    "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
    "-DBUILD_TESTS=${ENABLE_TESTS^^}"
    "-DBUILD_EXAMPLES=${ENABLE_EXAMPLES^^}"
    "-DBUILD_DOCS=${ENABLE_DOCS^^}"
    "-DUSE_VCPKG=$USE_VCPKG"
)

# Add platform-specific arguments
if [ "$PLATFORM" = "Windows" ]; then
    CMAKE_ARGS+=("-A" "x64")
fi

# Add vcpkg toolchain if available
if [ "$USE_VCPKG" = "AUTO" ] || [ "$USE_VCPKG" = "ON" ]; then
    VCPKG_ROOT="${VCPKG_ROOT:-$HOME/vcpkg}"
    VCPKG_TOOLCHAIN="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
    
    if [ -f "$VCPKG_TOOLCHAIN" ]; then
        CMAKE_ARGS+=("-DCMAKE_TOOLCHAIN_FILE=$VCPKG_TOOLCHAIN")
    fi
fi

# Configure step
echo -e "\033[36mConfiguring Chronovyan with CMake...\033[0m"
cd "$PROJECT_ROOT"
if ! cmake -B "$BUILD_DIR" "${CMAKE_ARGS[@]}" "$PROJECT_ROOT"; then
    echo -e "\033[31mCMake configuration failed\033[0m"
    exit 1
fi

# Build step
echo -e "\033[36mBuilding Chronovyan...\033[0m"
BUILD_CMD=(cmake --build "$BUILD_DIR" --config "$BUILD_TYPE")

# Add parallel build option if not using Visual Studio
if [ "$PLATFORM" != "Windows" ] || [ "$GENERATOR" = "Ninja" ]; then
    if [ "$PLATFORM" = "MacOS" ]; then
        CPU_COUNT=$(sysctl -n hw.ncpu)
    else
        CPU_COUNT=$(nproc)
    fi
    BUILD_CMD+=(-- -j$CPU_COUNT)
fi

if ! "${BUILD_CMD[@]}"; then
    echo -e "\033[31mBuild failed\033[0m"
    exit 1
fi

# Run tests if requested
if [ "$TARGET" = "test" ] && [ "$ENABLE_TESTS" = true ]; then
    echo -e "\033[36mRunning tests...\033[0m"
    cd "$BUILD_DIR"
    TEST_ARGS=("--output-on-failure")
    
    if [ "$PLATFORM" = "Windows" ]; then
        TEST_ARGS+=("-C" "$BUILD_TYPE")
    fi
    
    if ! ctest "${TEST_ARGS[@]}"; then
        echo -e "\033[31mTests failed\033[0m"
        exit 1
    fi
fi

# Install if requested
if [ "$TARGET" = "install" ]; then
    echo -e "\033[36mInstalling Chronovyan...\033[0m"
    cd "$BUILD_DIR"
    if ! cmake --build . --config "$BUILD_TYPE" --target install; then
        echo -e "\033[31mInstallation failed\033[0m"
        exit 1
    fi
fi

echo -e "\033[32mBuild completed successfully!\033[0m"

# Make the script executable
chmod +x "$0"
