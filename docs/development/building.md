# Building Chronovyan

This guide explains how to build Chronovyan from source on different platforms.

## Prerequisites

### All Platforms

- CMake 3.15 or later
- Git
- C++20 compatible compiler:
  - GCC 11+
  - Clang 12+
  - MSVC 2019+ (Visual Studio 16.11+)
  - Apple Clang 13+ (Xcode 14+)

### Linux/macOS

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git

# macOS (using Homebrew)
brew update
brew install cmake
```

### Windows

1. Install [Visual Studio 2019 or later](https://visualstudio.microsoft.com/downloads/) with:
   - Desktop development with C++
   - Windows 10/11 SDK
2. Install [Git](https://git-scm.com/download/win)
3. Install [CMake](https://cmake.org/download/)

## Getting the Source

```bash
git clone https://github.com/Chronovyan/Chronovyan.git
cd Chronovyan
```

## Building

### Basic Build

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Build Options

Available CMake options:

- `-DBUILD_TESTING=ON`: Build tests (default: ON)
- `-DBUILD_BENCHMARKS=OFF`: Build benchmarks (default: OFF)
- `-DBUILD_EXAMPLES=ON`: Build examples (default: ON)
- `-DCMAKE_BUILD_TYPE=Release`: Build type (Debug, Release, RelWithDebInfo, MinSizeRel)
- `-DCMAKE_INSTALL_PREFIX=/path/to/install`: Installation prefix

Example with options:

```bash
cmake -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=ON ..
```

### Building on Windows

#### Command Line (Developer Command Prompt)

```batch
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release
```

#### Using Visual Studio

1. Open CMake GUI
2. Set source code to Chronovyan root directory
3. Set build directory to `Chronovyan/build`
4. Click "Configure"
5. Select your compiler
6. Click "Generate"
7. Click "Open Project"
8. Build the solution

## Installing

```bash
cmake --install .
```

Or on Windows:

```batch
cmake --build . --config Release --target INSTALL
```

## Running Tests

After building, run the test suite:

```bash
cd build
ctest --output-on-failure
```

## Cross-Compiling

### Cross-Compiling for ARM (Raspberry Pi)

```bash
mkdir build-arm
cd build-arm
cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-arm-linux-gnueabihf.cmake ..
make -j$(nproc)
```

## Troubleshooting

### Common Issues

#### CMake Can't Find Compiler

Ensure you have a C++20 compatible compiler installed and in your PATH.

#### Missing Dependencies

Install any missing dependencies using your system's package manager.

#### Build Failures

1. Clean the build directory and try again:
   ```bash
   rm -rf build/*
   ```
2. Check the error messages for specific issues
3. Ensure all submodules are initialized:
   ```bash
   git submodule update --init --recursive
   ```

## Advanced Topics

### Build System Integration

#### Using as a Subproject

Add to your `CMakeLists.txt`:

```cmake
include(FetchContent)

FetchContent_Declare(
  chronovyan
  GIT_REPOSITORY https://github.com/Chronovyan/Chronovyan.git
  GIT_TAG main
)
FetchContent_MakeAvailable(chronovyan)

target_link_libraries(your_target PRIVATE chronovyan::chronovyan)
```

### Generating Documentation

To build the documentation:

```bash
pip install -r docs-requirements.txt
mkdocs build
```

### Packaging

Create a package:

```bash
mkdir build-package
cd build-package
cmake -DCPACK_GENERATOR="TGZ" ..
make package
```
