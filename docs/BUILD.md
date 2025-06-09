---
title: 4ever Build Guide
description: Documentation for BUILD.md
weight: 100
draft: true
---

# 4ever Build Guide

This guide provides detailed instructions for building 4ever from source on various platforms.

## Table of Contents
- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Build Options](#build-options)
- [Build Configurations](#build-configurations)
- [Platform-Specific Instructions](#platform-specific-instructions)
  - [Windows](#windows)
  - [macOS](#macos)
  - [Linux](#linux)
  - [Other Unix-like Systems](#other-unix-like-systems)
- [Using vcpkg](#using-vcpkg)
- [Troubleshooting](#troubleshooting)
- [Advanced Topics](#advanced-topics)

## Prerequisites

### All Platforms
- Git
- CMake 3.15 or higher
- C++17 compatible compiler:
  - GCC 9+
  - Clang 10+
  - MSVC 2019+ (Windows)
  - Apple Clang 12.0+ (macOS)
- Python 3.6+ (for some build scripts)

### Optional Dependencies
- [vcpkg](https://vcpkg.io/) - For dependency management
- Doxygen - For building documentation
- Graphviz - For generating documentation diagrams

## Quick Start

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/4ever.git
   cd 4ever
   ```

2. Configure the build:
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   ```

3. Build the project:
   ```bash
   cmake --build build --config Release
   ```

4. Run tests:
   ```bash
   cd build
   ctest --output-on-failure
   ```

## Build Options

4ever provides several CMake options to customize the build:

| Option | Type | Description | Default |
|--------|------|-------------|---------|
| `BUILD_COMPILER` | BOOL | Build the 4ever compiler | ON |
| `BUILD_INTERPRETER` | BOOL | Build the 4ever interpreter | ON |
| `BUILD_TESTS` | BOOL | Build test suites | ON |
| `BUILD_EXAMPLES` | BOOL | Build example programs | ON |
| `BUILD_DOCS` | BOOL | Build documentation (requires Doxygen) | OFF |
| `USE_VCPKG` | STRING | Use vcpkg for dependencies (AUTO/ON/OFF) | AUTO |
| `ENABLE_LTO` | BOOL | Enable Link Time Optimization | OFF |
| `ENABLE_COVERAGE` | BOOL | Enable code coverage reporting | OFF |
| `ENABLE_ASAN` | BOOL | Enable Address Sanitizer | OFF |
| `ENABLE_UBSAN` | BOOL | Enable Undefined Behavior Sanitizer | OFF |

Example usage:
```bash
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  -DENABLE_ASAN=OFF
```

## Build Configurations

4ever supports the standard CMake build types:

- **Debug**: Full debug symbols, no optimizations (`-g -O0`)
- **Release**: Optimized for performance (`-O3`)
- **RelWithDebInfo**: Optimized with debug symbols (`-O2 -g`)
- **MinSizeRel**: Optimized for size (`-Os`)

## Platform-Specific Instructions

### Windows

#### Prerequisites
- Visual Studio 2019 or later with C++ CMake tools
- Git for Windows
- [vcpkg](https://vcpkg.io/) (recommended)

#### Building with Visual Studio

```powershell
# Clone the repository
git clone https://github.com/yourusername/4ever.git
cd 4ever

# Configure (x64)
cmake -B build -G "Visual Studio 16 2019" -A x64

# Build
cmake --build build --config Release
```

#### Building with Ninja (faster builds)

```powershell
# Install Ninja
choco install ninja

# Configure
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build
```

### macOS

#### Prerequisites
- Xcode Command Line Tools
- Homebrew (recommended)

```bash
# Install dependencies
brew install cmake ninja

# Configure and build
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

### Linux

#### Ubuntu/Debian
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3

# Configure and build
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

#### Fedora/RHEL
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    ninja-build \
    git \
    python3
```

### Other Unix-like Systems

For other Unix-like systems, ensure you have:
- A C++17 compatible compiler
- CMake 3.15+
- Git
- Python 3.6+

## Using vcpkg

vcpkg simplifies dependency management:

```bash
# Install vcpkg
cd ~
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh

# Install dependencies
./vcpkg/vcpkg install fmt doctest

# Configure with vcpkg toolchain
cmake -B build \
  -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Release
```

## Troubleshooting

### Common Issues

1. **CMake can't find compiler**
   - Ensure you have the necessary build tools installed
   - On Windows, run from the appropriate Visual Studio command prompt

2. **Missing dependencies**
   - Install required system packages
   - Or use vcpkg for dependency management

3. **Build failures**
   - Clean the build directory and reconfigure
   - Check for error messages in the build log
   - Ensure your compiler is up to date

4. **Test failures**
   - Run tests with `ctest --output-on-failure` for more details
   - Check if all dependencies are correctly installed

## Advanced Topics

### Cross-Compiling

To cross-compile for a different architecture, create a toolchain file:

```cmake
# Example toolchain file for ARM64
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
```

Then configure with:
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake
```

### Building Documentation

To build the documentation:

```bash
# Install Doxygen and Graphviz
sudo apt-get install -y doxygen graphviz

# Configure with docs enabled
cmake -B build -DBUILD_DOCS=ON

# Build documentation
cmake --build build --target doc

# Open documentation
xdg-open build/docs/html/index.html  # Linux
open build/docs/html/index.html      # macOS
```

### Package Installation

To install 4ever system-wide:

```bash
cmake --build build --target install
```

By default, this installs to `/usr/local` on Unix-like systems. To change the installation prefix:

```bash
cmake -B build -DCMAKE_INSTALL_PREFIX=/path/to/install
```