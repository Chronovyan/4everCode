<div align="center">
  <h1>⏳ Chronovyan</h1>
  <h3>A Modern C++ Library for Dates, Times, and Time Zones</h3>
  
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
  [![Documentation](https://img.shields.io/badge/Documentation-Read%20the%20Docs-blue)](https://chronovyan.github.io/Chronovyan/)
  [![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/compiler_support)
  [![Build Status](https://github.com/Chronovyan/Chronovyan/actions/workflows/ci.yml/badge.svg)](https://github.com/Chronovyan/Chronovyan/actions)
  
  *"Precision timing for modern C++ applications"*
</div>

## 🚀 Features

- **Modern C++20 API** - Clean, type-safe interface using the latest C++ standards
- **Comprehensive Time Zone Support** - Full IANA time zone database integration
- **Header-Only** - Easy integration into any project
- **Cross-Platform** - Works on Windows, Linux, and macOS
- **High Performance** - Optimized for speed and low latency
- **Thread-Safe** - Designed for concurrent applications

## 📦 Installation

### Using vcpkg
```bash
vcpkg install chronovyan
```

### From Source
```bash
git clone https://github.com/Chronovyan/Chronovyan.git
cd Chronovyan
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=/path/to/install
cmake --build . --target install
```

## 🎯 Quick Start

```cpp
#include <chronovyan/chronovyan.hpp>
#include <iostream>

int main() {
    using namespace chronovyan;
    
    // Get current time
    auto now = system_clock::now();
    std::cout << "Current time: " << now << std::endl;
    
    // Format time
    std::cout << format("{:%Y-%m-%d %H:%M:%S}", now) << std::endl;
    
    // Time zone conversion
    auto ny_zone = locate_zone("America/New_York");
    auto ny_time = zoned_time{ny_zone, now};
    std::cout << "New York time: " << ny_time << std::endl;
    
    // Date arithmetic
    auto tomorrow = now + days{1};
    std::cout << "Tomorrow: " << tomorrow << std::endl;
    
    return 0;
}
```

## 📚 Documentation

- [Getting Started](https://chronovyan.github.io/Chronovyan/getting-started/installation/)
- [API Reference](https://chronovyan.github.io/Chronovyan/api/chrono/)
- [Time Zone Guide](https://chronovyan.github.io/Chronovyan/guides/timezones/)
- [Performance Tips](https://chronovyan.github.io/Chronovyan/guides/performance/)

## 🛠 Building from Source

### Prerequisites

- C++20 compatible compiler (GCC 11+, Clang 12+, MSVC 2019+)
- CMake 3.15 or later
- Git

### Build Commands

```bash
git clone https://github.com/Chronovyan/Chronovyan.git
cd Chronovyan
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --target install
```

## 🤝 Contributing

Contributions are welcome! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

## 📄 License

Chronovyan is licensed under the [MIT License](LICENSE).

## 🌐 Community

- [GitHub Discussions](https://github.com/Chronovyan/Chronovyan/discussions)
- [Issue Tracker](https://github.com/Chronovyan/Chronovyan/issues)
- [Discord](https://discord.gg/chronovyan)

### TemporalSynchronizer

A thread-safe component for managing temporal flow synchronization in multi-threaded applications:

- **Thread-Safe Operations**: Safe for concurrent access from multiple threads
- **Asynchronous Processing**: Background thread handles synchronization tasks
- **Metrics Tracking**: Monitors synchronization quality with multiple metrics
- **Event Callbacks**: Get notified when synchronization completes

Example usage:
```cpp
#include <chronovyan/temporal_synchronizer_simple.hpp>

// Create a synchronizer
chronovyan::sync::TemporalSynchronizer sync;

// Set a callback for sync completion
sync.set_sync_callback([](double progress) {
    if (progress >= 1.0) {
        std::cout << "Synchronization complete!" << std::endl;
    }
});

// Trigger synchronization
sync.synchronize_temporal_flows();
```

For more details, see the [TemporalSynchronizer Documentation](docs/temporal_synchronizer.md).

### Temporal Runtime

The core execution environment that manages temporal resources and cycle progression:

- Tracks and manages chronons and aethel
- Controls cycle advancement and rollbacks
- Provides the foundation for temporal operations
- Monitors resource usage and enforces limits

### Temporal Debt Tracker

A component responsible for tracking and managing temporal debt:

- Tracks borrowing and repayment of chronons and aethel
- Maintains records of all debt operations
- Applies interest rates based on operation criticality
- Calculates system stability and paradox risk
- Provides basic alert functionality for debt thresholds

### Temporal Debt Alert System

An advanced alerting and management system built on top of the Debt Tracker:

- Multi-level alerts (LOW, MEDIUM, HIGH, CRITICAL) based on configurable thresholds
- Multiple notification methods (console, log files, custom callbacks, etc.)
- Continuous real-time monitoring of debt levels
- Alert history tracking and acknowledgment
- Debt trend visualization and future projections
- Paradox risk calculation and monitoring
- Proactive debt management recommendations

### Temporal Data Structures

A collection of time-aware data structures:

- **TemporalMap**: A map structure that tracks the history of key-value pairs
- **TemporalVector**: A vector with built-in temporal tracking
- **TemporalSequence**: A sequence that can be rewound and manipulated across time
- **MultiDimensionalVector**: A vector that can operate across multiple dimensions

### User-Definable TYPE System

A system for defining custom types with temporal semantics:

- Define structs, enums, unions, and type aliases
- Track changes to type instances across cycles
- Query the state of instances at any point in time
- Build complex type hierarchies with nested types
- Support for required fields, default values, and type constraints

### Interpreter and Language Support

A scripting language designed for temporal programming:

- Chronovyan scripting language with temporal primitives
- Native function integration for temporal operations
- Support for user-defined types and custom semantics
- Resource tracking and optimization

## 🚀 Getting Started

### Your First Chronovyan Program

Create a file named `hello_time.cvy`:

```chronovyan
// A simple Chronovyan program
IMPORT "chrono.core";

DECLARE CONF main : WEAVE_PATTERN = define_pattern("hello_world", [
    // Print a greeting
    PRINT("Hello, Temporal World!");
    
    // Track time
    DECLARE CONF now : TIMESTAMP = NOW();
    PRINT("Current time: " + TO_STRING(now));
    
    // Return success
    RETURN 0;
]);
```

Compile and run:

```bash
# Compile
chronovyan_compiler hello_time.cvy -o hello_time

# Run
./hello_time
```

## 🧪 Testing

### Running Tests

```bash
# Run all tests
ctest

# Run specific test suite
./tests/temporal_map_test

# Run with verbose output
ctest --output-on-failure
```

### Running Examples

Explore the examples directory for various use cases:

```bash
# List available examples
ls examples

# Run an example
./examples/01_beginner/01_hello_world
```

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guide](CONTRIBUTING.md) for details on how to get started.

## 📜 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📞 Support

For help and support:
- Check the [documentation](docs/README.md)
- Open an [issue](https://github.com/yourusername/chronovyan/issues)
- Join our [community forum](https://github.com/yourusername/chronovyan/discussions)

## 📚 Documentation

Chronovyan offers comprehensive documentation to help you get started and master temporal programming:

### 📚 Documentation

Chronovyan offers comprehensive documentation to help you get started and become an expert:

### Core Documentation
- [📖 Language Specification](docs/Chronoscript%20Language%20Specification.md) - Complete syntax and semantics
- [🔧 API Reference](docs/api_reference.md) - Detailed API documentation
- [🧠 Core Concepts](docs/concepts/README.md) - Fundamental concepts and architecture

### Learning Resources
- [🎓 Tutorials](docs/tutorials/README.md) - Step-by-step guides
- [📚 Examples](examples/README.md) - Sample code and patterns
- [📖 Guides](docs/guides/README.md) - In-depth articles on specific topics

### Development
- [⚙️ Compiler Guide](docs/README_COMPILER.md) - Building and extending the compiler
- [🧪 Testing](docs/testing/README.md) - How to test Chronovyan applications
- [📦 Packaging](docs/packaging.md) - Creating and sharing Chronovyan packages
  - [Advanced Tutorials](docs/tutorials/03_advanced/README.md)
- [📝 Examples](examples/README.md) - Practical code examples
- [🎮 Interactive Playground](examples/playground/README.md) - Try Chronovyan in your browser

### Reference

- [📚 Standard Library](docs/Chronoscript%20Standard%20Library%20Reference.md)
- [📊 Data Structures](docs/temporal_map.md)
- [⏱️ Temporal Operations](docs/temporal_sequence.md)
- [🧩 Type System](docs/user_type_system.md)

### Additional Resources

- [❓ FAQ](docs/FAQ.md) - Common questions and answers
- [🐛 Debugging Guide](docs/troubleshooting/README.md)
- [📈 Best Practices](docs/best_practices.md)
- [🔮 Roadmap](docs/ROADMAP.md) - Upcoming features and development plans

## 🛠️ Building from Source

### Prerequisites

- **C++17 compatible compiler** (GCC 9+, Clang 10+, MSVC 2019+)
- **CMake 3.15+**
- **Git**
- **Python 3.6+** (for build scripts)
- **vcpkg** (optional, for dependency management)

### Quick Start

#### Using the Build Scripts (Recommended)

We provide cross-platform build scripts that simplify the build process:

**Windows (PowerShell):**
```powershell
# Clone the repository
git clone https://github.com/yourusername/chronovyan.git
cd chronovyan

# Build with default options (Release build)
.\build.ps1

# Or customize the build
.\build.ps1 -BuildType Release -Clean -EnableTests
```

**Linux/macOS (Bash):**
```bash
# Clone the repository
git clone https://github.com/yourusername/chronovyan.git
cd chronovyan

# Make the build script executable
chmod +x build.sh

# Build with default options (Release build)
./build.sh

# Or customize the build
./build.sh --build-type=Release --clean --tests
```

#### Manual Build with CMake

If you prefer to use CMake directly:

```bash
# Clone the repository
git clone https://github.com/yourusername/chronovyan.git
cd chronovyan

# Configure with CMake
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build --config Release

# Run tests
cd build
ctest --output-on-failure
```

### Build Options

| Option | Description | Default |
|--------|-------------|---------|
| `BUILD_COMPILER` | Build the Chronovyan compiler | `ON` |
| `BUILD_INTERPRETER` | Build the Chronovyan interpreter | `ON` |
| `BUILD_TESTS` | Build test suites | `ON` |
| `BUILD_EXAMPLES` | Build example programs | `ON` |
| `BUILD_DOCS` | Build documentation | `OFF` |
| `USE_VCPKG` | Use vcpkg for dependency management | `AUTO` |
| `ENABLE_LTO` | Enable Link Time Optimization | `OFF` |

Example:
```bash
cmake -B build -DBUILD_TESTS=ON -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Release
```

### Build Configurations

- **Debug**: Full debug symbols, no optimizations (`-g -O0`)
- **Release**: Optimized for performance (`-O3`)
- **RelWithDebInfo**: Optimized with debug symbols (`-O2 -g`)
- **MinSizeRel**: Optimized for size (`-Os`)

### Using vcpkg (Optional)

Chronovyan can use vcpkg for dependency management:

```bash
# Install vcpkg
cd ~
git clone https://github.com/microsoft/vcpkg
./vcpkg/bootstrap-vcpkg.sh

# Install dependencies
./vcpkg/vcpkg install fmt doctest

# Configure with vcpkg toolchain
cmake -B build -DCMAKE_TOOLCHAIN_FILE=~/vcpkg/scripts/buildsystems/vcpkg.cmake
```

### Building on Windows

```powershell
# Using Visual Studio 2019
git clone https://github.com/yourusername/chronovyan.git
cd chronovyan
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

### Building on macOS

```bash
# Install dependencies with Homebrew
brew install cmake

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(sysctl -n hw.logicalcpu)
```

### Building on Linux

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install -y build-essential cmake

# Build
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

For more detailed build instructions, see our [Build Guide](docs/BUILD.md).

## Development History

The `chronolog` directory contains entries documenting major developments:

- [2023-10-30: Temporal Sequence Implementation](chronolog/2023-10-30-temporal-sequence-implementation.md)
- [2023-11-07: User-Definable TYPE System](chronolog/2023-11-07-user-definable-type-system.md)

## Dependencies

- C++17 compatible compiler
- Standard Template Library (STL)

## License

This project is licensed under the MIT License - see the LICENSE file for details. 