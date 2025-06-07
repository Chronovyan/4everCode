---
title: Chronovyan and) runtime") Build Guide
description: Documentation for BUILD.md
weight: 100
draft: true
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Chronovyan Build Guide

This guide provides detailed instructions for building Chronovyan from source on various platforms.

##

 [Prerequisites](#))
)
)) )[)Q)u)i)c)k) )S)t)a)r)t)])()#))
)
)) )[)B)u)i)l)d) )O)p)t)i)o)n)s)])()#))
)
)) )[)B)u)i)l)d) )C)o)n)f)i)g)u)r)a)t)i)o)n)s)])()#))
)
)) )[)P)l)a)t)f)o)r)m)-)S)p)e)c)i)f)i)c) )I)n)s)t)r)u)c)t)i)o)n)s)])()#))
)
)) )-) )[)W)i)n)d)o)w)s)])()#))
)
)) )-) )[)m)a)c)O)S)])()#))
)
)) )-) )[)L)i)n)u)x)])()#))
)
)) )-) )[)O)t)h)e)r) )U)n)i)x)-)l)i)k)e) )S)y)s)t)e)m)s)])()#))
)
)) [Using) vcpkg](#))
)
) [Troubleshooting](#))
)
) [Advanced) Topics](#)a)d)v)a)n)c)e)d)-)t)o)p)i)c)s)

## Prerequisites

###

 Git
- CMake 3.15 or higher
- C++17 compatible compiler:
  - GCC 9+
  - Clang 10+
  - MSVC 2019+ (Windows)
  - Apple Clang 12.0+ (macOS)
- Python 3.6+ (for some build scripts)

###

 [vcpkg](https://github.com/Chronovyan/Chronovyan.github.io)t)t)p)s):)/)/)v)c)p)k)g).)i)o)/) - For dependency management
- Doxygen - For building documentation
- Graphviz - For generating documentation diagrams

## Quick Start

1. Clone the repository:
```bash
    git clone https://github.com/yourusername/chronovyan.git
    cd chronovyan
```text

    2. Configure the build:
```bash
    cmake -B build -DCMAKE_BUILD_TYPE=Release
```text

    3. Build the project:
```bash
    cmake --build build --config Release
```text

    4. Run tests:
```bash
    cd build
    ctest --output-on-failure
```text

    ## Build Options

    Chronovyan provides several CMake options to customize the build:

    | Option | Type | Description | Default | --- |------| --- |---------| `BUILD_COMPILER` | BOOL | Build the Chronovyan compiler | ON | `BUILD_INTERPRETER` | BOOL | Build the Chronovyan interpreter | ON | `BUILD_TESTS` | BOOL | Build test suites | ON | `BUILD_EXAMPLES` | BOOL | Build example programs | ON | `BUILD_DOCS` | BOOL | Build documentation (requires Doxygen) | OFF | `USE_VCPKG` | STRING | Use vcpkg for dependencies (AUTO/ON/OFF) | AUTO | `ENABLE_LTO` | BOOL | Enable Link Time Optimization | OFF | `ENABLE_COVERAGE` | BOOL | Enable code coverage reporting | OFF | `ENABLE_ASAN` | BOOL | Enable Address Sanitizer | OFF | `ENABLE_UBSAN` | BOOL | Enable Undefined Behavior Sanitizer | OFF |

    Example usage:
```bash
cmake -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON \
  -DBUILD_EXAMPLES=ON \
  -DENABLE_ASAN=OFF
```text

## Build Configurations

Chronovyan supports the standard CMake build types
*Debug**: Full debug symbols, no optimizations (`-g -O0`)
- **Release**: Optimized for performance (`-O3`)
- **RelWithDebInfo**: Optimized with debug symbols (`-O2 -g`)
- **MinSizeRel**: Optimized for size (`-Os`)

## Platform-Specific Instructions

### Windows

####

 Visual Studio 2019 or later with C++ CMake tools
- Git for Windows
- [vcpkg](https://github.com/Chronovyan/Chronovyan.github.io)t)t)p)s):)/)/)v)c)p)k)g).)i)o)/) (recommended)

####

``powershell
    #

   git clone https://github.com/yourusername/chronovyan.git
    cd chronovyan

    #

   cmake -B build -G "Visual Studio 16 2019" -A x64

    #

   cmake --build build --config Release
```text

####

``powershell
    #

   choco install ninja

    #

   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

    #

   cmake --build build
```text

### macOS

####

 Xcode Command Line Tools
- Homebrew (recommended)
```bash
    #

   brew install cmake ninja

    #

   cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
    cmake --build build
```text

### Linux

####

``bas

#

udo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    ninja-build \
    git \
    python3

#

make -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build
```text

####

``bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    ninja-build \
    git \
    python3
```text

### Other Unix-like Systems

For other Unix-like systems, ensure you have:
- A C++17 compatible compiler
- CMake 3.15+
- Git
- Python 3.6+

## Using vcpkg

vcpkg simplifies dependency management:
```bash
    #

   cd ~
    git clone https://github.com/microsoft/vcpkg
    ./vcpkg/bootstrap-vcpkg.sh

    #

   ./vcpkg/vcpkg install fmt doctest

    #

   cmake -B build \
      -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake \
      -DCMAKE_BUILD_TYPE=Release
```text

## Troubleshooting

### Common Issues

1. **CMake can't find compiler**Ensure you have the necessary build tools installed
    On Windows, run from the appropriate Visual Studio command prompt

2.**Missing dependencies**Install required system packages
    Or use vcpkg for dependency management

3.**Build failures**Clean the build directory and reconfigure
    Check for error messages in the build log
    Ensure your compiler is up to date

4.**Test failures**
    Run tests with `ctest --output-on-failure` for more details
    Check if all dependencies are correctly installed

## Advanced Topics

### Cross-Compiling

To cross-compile for a different architecture, create a toolchain file:
```cmake
    #

   set(CMAKE_SYSTEM_NAME Linux)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)
    set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++)
```text

Then configure with:
```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=../toolchain.cmake
```text

### Building Documentation

To build the documentation:
```bash
    #

   sudo apt-get install -y doxygen graphviz

    #

   cmake -B build -DBUILD_DOCS=ON

    #

   cmake --build build --target doc

    #

   xdg-open build/docs/html/index.html  #

   open build/docs/html/index.html      #

``text

### Package Installation

To install Chronovyan system-wide:
```bash
    cmake --build build --target install
```text

By default, this installs to `/usr/local` on Unix-like systems. To change the installation prefix:```bash
    cmake -B build -DCMAKE_INSTALL_PREFIX=/path/to/install
```