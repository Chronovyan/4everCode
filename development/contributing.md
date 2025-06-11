# Contributing to 4ever

Thank you for your interest in contributing to 4ever! We welcome contributions from the community.

## Code of Conduct

Please review our [Code of Conduct](CODE_OF_CONDUCT.md) before contributing.

## Getting Started

1. **Fork** the repository on GitHub
2. **Clone** your fork locally
3. Create a new **branch** for your changes
4. Make your changes
5. **Test** your changes
6. **Commit** and **push** to your fork
7. Open a **Pull Request**

## Development Environment

### Prerequisites

- C++20 compatible compiler
- CMake 3.15+
- Git
- Python 3.6+ (for documentation)

### Building

```bash
git clone https://github.com/your-username/4ever.git
cd 4ever
mkdir build && cd build
cmake ..
cmake --build .
```

### Running Tests

```bash
cd build
ctest --output-on-failure
```

## Code Style

We follow the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).

### Formatting

We use `clang-format` for code formatting. Run the following before committing:

```bash
# Format all source files
find . -name '*.h' -o -name '*.cpp' | xargs clang-format -i
```

### Naming Conventions

- **Classes**: `PascalCase`
- **Functions**: `camelCase`
- **Variables**: `snake_case`
- **Constants**: `UPPER_SNAKE_CASE`
- **Namespaces**: `lowercase`

## Documentation

We use MkDocs for documentation. To build the documentation locally:

```bash
# Install dependencies
pip install -r docs-requirements.txt

# Serve documentation locally
mkdocs serve
```

### Documentation Guidelines

- Use clear, concise language
- Include examples for all public APIs
- Document all parameters and return values
- Keep documentation up-to-date with code changes

## Testing

### Writing Tests

- Create a new test file in the `tests` directory
- Use Catch2 for unit tests
- Test both success and failure cases
- Include descriptive test names

### Running Tests

```bash
cd build
ctest --output-on-failure
```

## Pull Requests

1. Keep PRs focused on a single feature or bug fix
2. Include tests for new features
3. Update documentation as needed
4. Ensure all tests pass
5. Request reviews from maintainers

## Reporting Issues

When reporting issues, please include:

1. Description of the issue
2. Steps to reproduce
3. Expected behavior
4. Actual behavior
5. Environment details (OS, compiler, etc.)
6. Any relevant code or error messages

## License

By contributing to 4ever, you agree that your contributions will be licensed under the project's [LICENSE](LICENSE) file.
