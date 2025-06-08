<div align="center">
  <h1>⏳ Chronovyan</h1>
  <h3>A Modern C++20 Library for Temporal Programming</h3>
  
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
  [![C++ Standard](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
  [![CMake](https://img.shields.io/badge/CMake-3.15%2B-064F8C)](https://cmake.org/)
  [![Documentation](https://img.shields.io/badge/docs-gh--pages-blue)](https://chronovyan.github.io/)
  
  *"Time is the canvas, and we are the weavers of its threads."* - Ancient Weaver Proverb
</div>

## 🌟 About Chronovyan

Chronovyan is a modern C++20 library designed for temporal programming, providing powerful abstractions for time-based computations and concurrent operations. It offers a clean, type-safe interface for working with temporal data and events in C++ applications.

### Key Features

- **Header-Only Option**: Use as a header-only library or build as a shared/static library
- **Temporal Types**: Comprehensive support for time points, durations, and intervals
- **Concurrency Primitives**: Safe and efficient concurrency utilities
- **Event System**: Flexible event handling with type-safe callbacks
- **Modern C++**: Leverages C++20 features for clean and expressive code
- **Cross-Platform**: Works on Windows, Linux, and macOS

## 📚 Documentation

Comprehensive documentation is available in the [docs](docs/) directory and online at [https://chronovyan.github.io/](https://chronovyan.github.io/).

### Documentation Structure

- **Getting Started**: Quick start guide and basic usage
- **API Reference**: Detailed class and function documentation
- **Examples**: Sample code demonstrating key features
- **Contributing**: Guidelines for contributing to the project

To build documentation locally:

```bash
# Install requirements
pip install -r docs/requirements.txt

# Build documentation
mkdocs serve  # For live preview
# or
mkdocs build  # For static site generation
```

## 🚀 Quick Start

### Prerequisites

- C++20 compatible compiler (GCC 11+, Clang 12+, or MSVC 2019 16.11+)
- CMake 3.15 or higher
- Git

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/Chronovyan/Chronovyan.git
   cd Chronovyan
   ```

2. Configure and build:
   ```bash
   # Create build directory
   mkdir build && cd build
   
   # Configure with CMake
   cmake .. -DCMAKE_BUILD_TYPE=Release
   
   # Build the library and tests
   cmake --build . --config Release
   
   # Run tests
   ctest -C Release --output-on-failure
   ```

### Using as a CMake Dependency

Add Chronovyan to your project using `FetchContent`:

```cmake
include(FetchContent)
FetchContent_Declare(
  chronovyan
  GIT_REPOSITORY https://github.com/Chronovyan/Chronovyan.git
  GIT_TAG main  # or a specific version tag
)
FetchContent_MakeAvailable(chronovyan)

target_link_libraries(your_target PRIVATE chronovyan::chronovyan_lib)
```

### Header-Only Usage

For header-only usage, simply copy the `include/chronovyan` directory to your project's include path.

### Your First Chronovyan Program

Create a file named `hello.chrono` with the following content:

```rust
// A simple Chronovyan program
fn main() {
    // Print a greeting
    println!("Hello, Chronovyan!");
    
    // Demonstrate a simple temporal operation
    let now = time::now();
    println!("Current time: {}", now);
    
    // Schedule a future event
    let future = now + Duration::seconds(5);
    println!("Will print again at: {}", future);
    
    // Wait for the future time
    time::sleep_until(future);
    println!("Five seconds later...");
}
```

Run the program:

```bash
./build/bin/chrono run hello.chrono
```

## 🛠 Development

### Prerequisites

- C++20 compatible compiler
- CMake 3.20+
- Python 3.8+ (for build scripts)
- Git
- [Conan](https://conan.io/) (for dependency management)
- [pre-commit](https://pre-commit.com/) (for git hooks)

### Development Setup

1. Clone the repository:
   ```bash
   git clone --recurse-submodules https://github.com/Chronovyan/Chronovyan.git
   cd Chronovyan
   ```

2. Install Python dependencies:
   ```bash
   pip install -r requirements-dev.txt
   ```

3. Set up pre-commit hooks:
   ```bash
   pre-commit install
   ```

4. Configure the build:
   ```bash
   mkdir -p build && cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   ```

5. Build the project:
   ```bash
   cmake --build . --parallel
   ```

### Running Tests

```bash
# Run all tests
cd build
ctest --output-on-failure

# Run specific test
./tests/chrono_tests

# Run with verbose output
ctest -V
```

### Code Style

We use `clang-format` for C++ code formatting. To format your code:

```bash
# Format all source files
./scripts/format.sh
```

## 🤝 Contributing

We welcome contributions from the community! Whether you're fixing bugs, adding new features, or improving documentation, your help is appreciated.

### Documentation Development

We've created several tools to make documentation development easier and more consistent:

#### Setup Documentation Environment

1. **Windows (PowerShell)**:
   ```powershell
   .\scripts\setup_docs.ps1
   ```

2. **Unix-like systems (Linux/macOS)**:
   ```bash
   chmod +x ./scripts/setup_docs.sh
   ./scripts/setup_docs.sh
   ```

#### Creating New Documentation

Use the documentation generator to create new pages with the correct structure:

- **Windows (PowerShell)**:
  ```powershell
  .\scripts\new_doc.bat
  ```

- **Unix-like systems**:
  ```bash
  ./scripts/new_doc.sh
  ```

#### Validating Documentation

Before submitting changes, validate the documentation:

- **Windows (PowerShell)**:
  ```powershell
  .\scripts\validate_docs.ps1
  ```

- **Unix-like systems**:
  ```bash
  ./scripts/validate_docs.sh
  ```

#### Serving Documentation Locally
   ```bash
   mkdocs serve
   ```
   Then open http://127.0.0.1:8000 in your browser.

3. Validate your changes:
   ```bash
   # Windows
   .\scripts\validate_docs.ps1
   
   # Unix-like systems
   chmod +x ./scripts/validate_docs.sh
   ./scripts/validate_docs.sh
   ```

Before contributing, please review our [Code of Conduct](./docs/community/code-of-conduct.md) and [Contributing Guide](./docs/development/contributing.md).

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 📬 Contact

For questions or feedback, please open an issue on our [GitHub repository](https://github.com/Chronovyan/Chronovyan/issues).

## 🙏 Acknowledgments

Chronovyan is made possible by the contributions of many people. We'd like to thank all our contributors and the open-source community for their support.

Special thanks to the following projects that inspired Chronovyan:
- Rust Programming Language
- LLVM/Clang
- Chrono (C++ date and time library)
- The broader open-source community
