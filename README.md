<div align="center">
  <h1>⏳ Chronovyan</h1>
  <h3>A Temporal Programming Language and Runtime</h3>
  
  [![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
  [![Documentation Status](https://img.shields.io/badge/docs-latest-brightgreen.svg)](https://chronovyan.github.io/)
  [![Build Status](https://github.com/Chronovyan/Chronovyan/actions/workflows/ci.yml/badge.svg)](https://github.com/Chronovyan/Chronovyan/actions)
  [![Code Style](https://img.shields.io/badge/code%20style-black-000000.svg)](https://github.com/psf/black)
  
  *"Time is the canvas, and we are the weavers of its threads."* - Ancient Weaver Proverb
</div>

## 🌟 About Chronovyan

Chronovyan is a modern programming language designed for temporal programming, enabling developers to express time-based computations and concurrent operations with clarity and precision. It provides first-class support for temporal types, event handling, and distributed computing.

### Key Features

- **Temporal Types**: Native support for time-based data types and operations
- **Concurrent by Design**: Built-in primitives for safe concurrency and parallelism
- **Event-Driven Architecture**: First-class support for event handling and processing
- **Distributed Computing**: Seamless integration with distributed systems
- **Type Safety**: Strong, static type system with type inference
- **Performance**: Optimized for high-performance temporal computations

## 📚 Documentation

Comprehensive documentation is available in the `docs` directory and online at [https://chronovyan.github.io/](https://chronovyan.github.io/).

### Documentation Structure

- **Guides**: Tutorials and how-to guides for learning Chronovyan
  - [Getting Started](./docs/guides/getting-started/)
  - [Advanced Topics](./docs/guides/advanced/)
  - [Examples](./docs/guides/examples/)

- **Reference**: Language and API references
  - [Language Specification](./docs/reference/language/specification.md)
  - [Standard Library](./docs/reference/stdlib/)

- **Development**: Resources for contributors
  - [Building Chronovyan](./docs/development/building/)
  - [Contributing Guide](./docs/development/contributing/)

- **Design**: Architecture and design documents
  - [Compiler Architecture](./docs/architecture/compiler.md)
  - [Bytecode Format](./docs/architecture/bytecode-format.md)

- **Community**: Guidelines and policies
  - [Code of Conduct](./docs/community/code-of-conduct.md)
  - [Security Policy](./docs/community/security.md)

For a complete overview of the documentation structure, see [STRUCTURE.md](./docs/STRUCTURE.md).

## 🚀 Quick Start

### Prerequisites

- C++20 compatible compiler (GCC 11+, Clang 12+, or MSVC 2019 16.11+)
- CMake 3.20 or higher
- Python 3.8+ (for build scripts)
- Git

### Building from Source

1. Clone the repository:
   ```bash
   git clone https://github.com/Chronovyan/Chronovyan.git
   cd Chronovyan
   ```

2. Configure the build:
   ```bash
   mkdir build && cd build
   cmake ..
   ```

3. Build the project:
   ```bash
   cmake --build . --config Release
   ```

4. Run tests:
   ```bash
   ctest -C Release --output-on-failure
   ```

For more detailed build instructions, see the [Building Chronovyan](./docs/development/building/) guide.

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
