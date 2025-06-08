---
title: Installation
description: Get started with Chronovyan by installing it on your system
---

# Installation Guide

Chronovyan is a header-only library, making it easy to integrate into your C++ projects. This guide will walk you through the various installation methods.

## Prerequisites

- **Compiler**: C++20 compatible (GCC 11+, Clang 12+, MSVC 2019+)
- **Build System**: CMake 3.15 or later
- **Package Manager**: vcpkg (recommended) or Conan
- **Git** (for source installation)

## Installation Methods

### Method 1: Using vcpkg (Recommended)

1. Install vcpkg if you haven't already:
   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ./bootstrap-vcpkg.sh  # Linux/macOS
   .\bootstrap-vcpkg.bat # Windows
   ```

2. Install Chronovyan:
   ```bash
   ./vcpkg install chronovyan
   ```

3. Integrate with your CMake project:
   ```cmake
   find_package(Chronovyan CONFIG REQUIRED)
   target_link_libraries(your_target PRIVATE Chronovyan::Chronovyan)
   ```

### Method 2: Using Conan

1. Add the repository to your Conan remotes:
   ```bash
   conan remote add chronovyan https://conan.chronovyan.org
   ```

2. Install the package:
   ```bash
   conan install chronovyan/1.0.0@
   ```

### Method 3: From Source

1. Clone the repository:
   ```bash
   git clone https://github.com/Chronovyan/Chronovyan.git
   cd Chronovyan
   ```

2. Configure and build (CMake):
   ```bash
   mkdir build && cd build
   cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install \
             -DCHRONOVYAN_BUILD_TESTS=ON \
             -DCHRONOVYAN_BUILD_EXAMPLES=ON
   cmake --build . --parallel
   ctest -V  # Run tests
   cmake --install .
   ```

## Verifying Your Installation

Create a simple test program (`test_chronovyan.cpp`):

```cpp
#include <chronovyan/chronovyan.hpp>
#include <iostream>

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
```

Build and run:

=== "Linux/macOS"
    ```bash
    g++ -std=c++20 test_chronovyan.cpp -o test_chronovyan \
        -I/usr/local/include -L/usr/local/lib -lchronovyan
    ./test_chronovyan
    ```

=== "Windows (MSVC)"
    ```powershell
    cl /std:c++20 /EHsc test_chronovyan.cpp /I"C:\Program Files\Chronovyan\include"
    test_chronovyan.exe
    ```

## Troubleshooting

### Common Issues

1. **Compiler Not Found**
   - Ensure you have a C++20 compatible compiler installed
   - Check with `g++ --version` or `clang++ --version`

2. **Missing Dependencies**
   - On Ubuntu/Debian: `sudo apt install build-essential cmake`
   - On macOS: `xcode-select --install`
   - On Windows: Install Visual Studio with C++ workload

3. **Linker Errors**
   - Ensure the library is in your library path
   - Verify the library name matches (case-sensitive on Linux/macOS)

## Next Steps

- [Quick Start Guide](quickstart.md) - Learn the basics of Chronovyan
- [Examples](../examples/) - Explore code samples
- [API Reference](../api/reference/) - Detailed API documentation

## Need Help?

If you encounter any issues during installation, please:
1. Check the [FAQ](../faq/)
2. Search the [GitHub Issues](https://github.com/Chronovyan/Chronovyan/issues)
3. [Open a new issue](https://github.com/Chronovyan/Chronovyan/issues/new/choose) if your problem isn't listed
