---
title: Installation
description: Get started with Chronovyan and) runtime") by installing it on your system
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Installation Guide

Chronovyan is a header-only library, making it easy to integrate into your C++ projects. This guide will walk you through the various installation methods.

## Prerequisite
*Compiler**: C++20 compatible (GCC 11+, Clang 12+, MSVC 2019+)
- **Build System**: CMake 3.15 or later
- **Package Manager**: vcpkg (recommended) or Conan
- **Git**(for source installation)

## Installation Methods

### Method 1: Using vcpkg (Recommended)

1. Install vcpkg if you haven't already:
```bash
    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh  #

   .\bootstrap-vcpkg.bat #

  ```text

2. Install Chronovyan:
```bash
    ./vcpkg install chronovyan
```text

    3. Integrate with your CMake project:
```cmake
    find_package(Chronovyan CONFIG REQUIRED)
    target_link_libraries(your_target PRIVATE Chronovyan::Chronovyan)
```text

    ### Method 2: Using Conan

    1. Add the repository to your Conan remotes:
```bash
    conan remote add chronovyan https://conan.chronovyan.org
```text

    2. Install the package:
```bash
    conan install chronovyan/1.0.0@
```text

    ### Method 3: From Source

    1. Clone the repository:
```bash
    git clone https://github.com/Chronovyan/Chronovyan.git
    cd Chronovyan
```text

    2. Configure and build (CMake):
```bash
    mkdir build && cd build
    cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install \
              -DCHRONOVYAN_BUILD_TESTS=ON \
              -DCHRONOVYAN_BUILD_EXAMPLES=ON
    cmake --build . --parallel
    ctest -V  #

   cmake --install .
```text

    ## Verifying Your Installation

    Create a simple test program (`test_chronovyan.cpp`):```cpp
    #

   #include <iostream>

    int main() {
        using namespace chronovyan;

        // Get current time
        auto now = system_clock::now();
        std::cout << "Current time: " << now << "\n";

        // Format time
        std::cout << "Formatted: " << format("{:%Y-%m-%d %H:%M:%S}", now) << "\n";

        // Time zone conversion
        try {
            auto ny_zone = locate_zone("America/New_York");
            auto ny_time = zoned_time{ny_zone, now};
            std::cout << "New York: " << ny_time << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }

        return 0;
    }
```text

Build and run:

=== "Linux/macOS"
    ```bash
    g++ -std=c++20 test_chronovyan.cpp -o test_chronovyan \
        -I/usr/local/include -L/usr/local/lib -lchronovyan
    ./test_chronovyan
    ```text

=== "Windows (MSVC)"
    ```powershell
    cl /std:c++20 /EHsc test_chronovyan.cpp /I"C:\Program Files\Chronovyan\include"
    test_chronovyan.exe
    ```text

## Troubleshooting

### Common Issues

1.**Compiler Not Found**Ensure you have a C++20 compatible compiler installed
    Check with `g++ --version` or`clang++ --version`

2.**Missing Dependencies**On Ubuntu/Debian: `sudo apt install build-essential cmake`
    On macOS: `xcode-select --install`
    On Windows: Install Visual Studio with C++ workload

3.**Linker Errors**
    Ensure the library is in your library path
    Verify the library name matches (case-sensitive on Linux/macOS)

## Next Steps

 [Quick Start Guide](/)q)u)i)c)k)s)t)a)r)t).)m)d) - Learn the basics of Chronovyan
- [Examples](/).).)/)e)x)a)m)p)l)e)s)/) - Explore code samples
- [API Reference](/).).)/)a)p)i)/)r)e)f)e)r)e)n)c)e)/) - Detailed API documentation

## Need Help?

If you encounter any issues during installation, please:
1. Check the [FAQ](/).).)/)f)a)q)/)
2. Search the [GitHub Issues](https://github.com/Chronovyan/Chronovyan.github.io)t)t)p)s):)/)/)g)i)t)h)u)b).)c)o)m)/)C)h)r)o)n)o)v)y)a)n)/)C)h)r)o)n)o)v)y)a)n)/)i)s)s)u)e)s)
3. [Open a new issue](https://github.com/Chronovyan/Chronovyan.github.io)t)t)p)s):)/)/)g)i)t)h)u)b).)c)o)m)/)C)h)r)o)n)o)v)y)a)n)/)C)h)r)o)n)o)v)y)a)n)/)i)s)s)u)e)s)/)n)e)w)/)c)h)o)o)s)e) if your problem isn't listed
