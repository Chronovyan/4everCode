---
title: Contributing
description: Documentation for CONTRIBUTING
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Contributing to Chronovyan

Thank you for your interest in contributing to Chronovyan! We welcome contributions from the community to help improve this project.

## Getting Started

1. **Fork** the repository on GitHub
2. **Clone** the project to your own machine
3. **Commit** changes to your own branch
4. **Push** your work back up to your fork
5. Submit a **Pull Request** so we can review your changes

## Development Environment Setup

### Prerequisites

- CMake 3.12 or higher
- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019+)
- Git
- Python 3.8+ (for build scripts)

### Setup

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/chronovyan.git
   cd chronovyan
   ```

2. Run the setup script (Windows):
   ```powershell
   .\scripts\setup_dev_env.ps1
   ```
   
   Or on Linux/macOS:
   ```bash
   chmod +x ./scripts/setup_dev_env.sh
   ./scripts/setup_dev_env.sh
   ```

3. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

## Coding Standards

- Follow the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines)
- Use `snake_case` for variables and functions
- Use `PascalCase` for classes and structs
- Use `UPPER_CASE` for constants and macros
- Keep lines under 100 characters
- Document all public APIs with Doxygen-style comments

## Testing

- Write unit tests for all new features
- Ensure all tests pass before submitting a PR
- Run the test suite:
  ```bash
  cd build
  ctest --output-on-failure
  ```

## Pull Request Process

1. Update the CHANGELOG.md with details of your changes
2. Ensure your code passes all tests
3. Submit the PR with a clear description of the changes
4. Reference any related issues in your PR

## Code of Conduct

This project adheres to the [Contributor Covenant Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.
