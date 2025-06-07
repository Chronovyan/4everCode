---
title: Contributing
description: Documentation for contributing
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Contributing to [Chronovyan](https://chronovyan.github.io/h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/) "The) temporal) programming) language) and) runtime")

Thank you for your interest in contributing to Chronovyan! We welcome contributions from the community.

## Code of Conduct

Please review our [Code of Conduct](/)C)O)D)E)_)O)F)_)C)O)N)D)U)C)T).)m)d) before contributing.

## Getting Started

1. **Fork**the repository on GitHub
2.**Clone**your fork locally
3. Create a new**branch**for your changes
4. Make your changes
5.**Test**your changes
6.**Commit**and**push**to your fork
7. Open a**Pull Request**

## Development Environment

### Prerequisites

 C++20 compatible compiler
- CMake 3.15+
- Git
- Python 3.6+ (for documentation)

###

``bash
    git clone https://github.com/your-username/Chronovyan.git
    cd Chronovyan
    mkdir build && cd build
    cmake ..
    cmake --build .
```text

###

``bash
    cd build
    ctest --output-on-failure
```text

## Code Style

We follow the [C++ Core Guidelines](https://github.com/Chronovyan/Chronovyan.github.io)t)t)p)s):)/)/)i)s)o)c)p)p).)g)i)t)h)u)b).)i)o)/)C)p)p)C)o)r)e)G)u)i)d)e)l)i)n)e)s)/)C)p)p)C)o)r)e)G)u)i)d)e)l)i)n)e)s).

### Formatting

We use `clang-format` for code formatting. Run the following before committing:```bash
    #

   find . -name '*.h' -o -name '*.cpp' | xargs clang-format -i
```text

### Naming Convention
*Classes**: `PascalCase`
- **Functions**: `camelCase`
- **Variables**: `snake_case`
- **Constants**: `UPPER_SNAKE_CASE`
- **Namespaces**: `lowercase`

## Documentation

We use MkDocs for documentation. To build the documentation locally:
```bash
    #

   pip install -r docs-requirements.txt

    #

   mkdocs serve
```text

### Documentation Guidelines

 Use clear, concise language
- Include examples for all public APIs
- Document all parameters and return values
- Keep documentation up-to-date with code changes

## Testing

### Writing Tests

 Create a new test file in the `tests` directory
- Use Catch2 for unit tests
- Test both success and failure cases
- Include descriptive test names

###

``bash
    cd build
    ctest --output-on-failure
```text

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

By contributing to Chronovyan, you agree that your contributions will be licensed under the project's [LICENSE](/)L)I)C)E)N)S)E) file.
