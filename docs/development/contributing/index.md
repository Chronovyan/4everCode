# Contributing to Chronovyan

Thank you for your interest in contributing to Chronovyan! We're excited to have you on board. This guide will help you get started with contributing to the project.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [Ways to Contribute](#ways-to-contribute)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Setting Up the Development Environment](#setting-up-the-development-environment)
  - [Building from Source](#building-from-source)
- [Development Workflow](#development-workflow)
  - [Git Workflow](#git-workflow)
  - [Code Style](#code-style)
  - [Testing](#testing)
  - [Documentation](#documentation)
- [Pull Request Guidelines](#pull-request-guidelines)
- [Reporting Issues](#reporting-issues)
- [Feature Requests](#feature-requests)
- [Code Review Process](#code-review-process)
- [Community](#community)
- [License](#license)

## Code of Conduct

This project and everyone participating in it is governed by our [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report any unacceptable behavior to [contact@chronovyan.org](mailto:contact@chronovyan.org).

## Ways to Contribute

There are many ways to contribute to Chronovyan:

- **Code Contributions**: Implement new features or fix bugs
- **Documentation**: Improve documentation, add examples, or fix typos
- **Testing**: Write tests, report bugs, or help reproduce issues
- **Community**: Help others, answer questions, or improve guides
- **Feedback**: Share your experience and suggest improvements

## Getting Started

### Prerequisites

- **For Development**:
  - Git
  - Rust (latest stable version)
  - Cargo (Rust's package manager)
  - LLVM and Clang (for some components)
  - CMake (for building dependencies)

- **For Documentation**:
  - Python 3.8+
  - Sphinx
  - Pandoc (for documentation conversion)

### Setting Up the Development Environment

1. **Fork the Repository**
   ```bash
   # Fork on GitHub first, then clone your fork
   git clone https://github.com/your-username/Chronovyan.git
   cd Chronovyan
   ```

2. **Set Up the Build Environment**
   ```bash
   # Install Rust (if not already installed)
   curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
   
   # Install build dependencies
   # On Ubuntu/Debian:
   sudo apt-get update
   sudo apt-get install -y build-essential cmake clang llvm-dev libclang-dev
   
   # On macOS (using Homebrew):
   brew install cmake llvm
   ```

3. **Build the Project**
   ```bash
   # Build in debug mode (fast compilation, slower execution)
   cargo build
   
   # Build in release mode (slower compilation, faster execution)
   cargo build --release
   ```

4. **Run Tests**
   ```bash
   # Run all tests
   cargo test
   
   # Run specific test
   cargo test test_name
   ```

### Building from Source

For advanced users who want to build with specific features:

```bash
# Clone with submodules
git clone --recursive https://github.com/Chronovyan/Chronovyan.git
cd Chronovyan

# Configure the build
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build . --config Release

# Run tests
ctest -C Release
```

## Development Workflow

### Git Workflow

1. **Branch Naming**
   - `feature/` - New features or enhancements
   - `bugfix/` - Bug fixes
   - `docs/` - Documentation improvements
   - `refactor/` - Code refactoring
   - `test/` - Test additions or improvements
   - `chore/` - Maintenance tasks

2. **Commit Message Format**
   ```
   <type>(<scope>): <subject>
   
   <body>
   
   <footer>
   ```
   
   Example:
   ```
   feat(parser): add support for pattern matching
   
   Implemented basic pattern matching syntax with support for literals,
   variables, and wildcards. Added corresponding tests and documentation.
   
   Fixes #123
   ```

### Code Style

Follow these guidelines to maintain consistent code style:

1. **Formatting**
   - Run `cargo fmt` before committing
   - Maximum line length: 100 characters
   - Use 4 spaces for indentation

2. **Naming Conventions**
   - `snake_case` for variables and functions
   - `PascalCase` for types and traits
   - `SCREAMING_SNAKE_CASE` for constants
   - `'a` for lifetime parameters

3. **Documentation**
   - Document all public APIs with /// comments
   - Include examples in documentation
   - Update CHANGELOG.md for user-facing changes

### Testing

1. **Unit Tests**
   - Place tests in the same file as the code being tested
   - Use `#[test]` attribute for test functions
   - Follow the pattern:
     ```rust
     #[test]
     fn test_feature() {
         // Setup
         // Action
         // Assert
     }
     ```

2. **Integration Tests**
   - Place in `tests/` directory
   - Test public API only
   - Each file is a separate crate

3. **Benchmarks**
   - Use `#[bench]` for microbenchmarks
   - Place in `benches/` directory
   - Include comparison with previous versions

### Documentation

1. **API Documentation**
   - Document all public items
   - Include examples
   - Use markdown for formatting
   - Document panics and errors

2. **Examples**
   - Add to `examples/` directory
   - Include a README.md
   - Keep them simple and focused

3. **Guides**
   - Write in `docs/` directory
   - Use markdown format
   - Include code samples with expected output

## Pull Request Guidelines

1. **Before Submitting**
   - Run all tests
   - Update documentation
   - Ensure code is properly formatted
   - Update CHANGELOG.md if needed

2. **PR Description**
   - Reference related issues
   - Explain the changes
   - Document breaking changes
   - Include test instructions

3. **Review Process**
   - At least one approval required
   - All CI checks must pass
   - Address all review comments
   - Squash commits if needed

## Reporting Issues

When reporting issues, please include:

1. **Summary** - Brief description
2. **Steps to Reproduce**
3. **Expected vs Actual Behavior**
4. **Environment**
   - OS and version
   - Rust version
   - Chronovyan version
5. **Additional Context**
   - Logs
   - Screenshots
   - Related issues

## Feature Requests

1. **Before Submitting**
   - Search existing issues
   - Check the roadmap
   - Discuss in Discord

2. **Feature Request Template**
   ```markdown
   ## Problem
   
   ## Proposed Solution
   
   ## Alternatives Considered
   
   ## Additional Context
   ```

## Code Review Process

1. **Review Guidelines**
   - Be constructive
   - Focus on the code
   - Explain reasoning
   - Suggest improvements
   - Respect the author

2. **Review Checklist**
   - [ ] Code works as intended
   - [ ] Tests are included
   - [ ] Documentation is updated
   - [ ] Follows style guide
   - [ ] No performance regressions

## Community

- **Discord**: [Join our community](https://discord.gg/chronovyan)
- **Twitter**: [@ChronovyanLang](https://twitter.com/ChronovyanLang)
- **Email**: [contact@chronovyan.org](mailto:contact@chronovyan.org)
- **Weekly Meetings**: Tuesdays at 3 PM UTC

## License

By contributing to Chronovyan, you agree that your contributions will be licensed under the [MIT License](LICENSE).
