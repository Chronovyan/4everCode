# Contributing to Chronovyan

Thank you for your interest in contributing to Chronovyan! We welcome contributions from the community to help improve this project.

## Code of Conduct

This project and everyone participating in it is governed by our [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code.

## How to Contribute

1. **Fork** the repository on GitHub
2. **Clone** the project to your own machine
3. **Create a branch** for your changes
4. **Commit** changes to your branch
5. **Push** your work back up to your fork
6. Submit a **Pull Request** with a clear description of your changes

## Development Environment Setup

### Prerequisites

- Python 3.8 or higher
- pip (Python package manager)
- Git

### Setup

1. **Fork and clone** the repository:
   ```bash
   git clone https://github.com/yourusername/Chronovyan.git
   cd Chronovyan
   ```

2. **Set up a virtual environment** (recommended):
   ```bash
   # On Windows
   python -m venv venv
   .\venv\Scripts\activate
   
   # On macOS/Linux
   python3 -m venv venv
   source venv/bin/activate
   ```

3. **Install the package in development mode** with all development dependencies:
   ```bash
   pip install -e ".[dev]"
   ```

4. **Install pre-commit hooks** to ensure code quality:
   ```bash
   pre-commit install
   ```

## Development Workflow

1. **Create a new branch** for your feature or bugfix:
   ```bash
   git checkout -b feature/your-feature-name
   # or
   git checkout -b bugfix/issue-number-description
   ```

2. **Make your changes** following the project's coding standards

3. **Run tests** to ensure nothing is broken:
   ```bash
   pytest
   ```

4. **Format and lint** your code:
   ```bash
   black .
   isort .
   flake8
   mypy .
   ```

5. **Commit your changes** with a descriptive message:
   ```bash
   git add .
   git commit -m "feat: add new feature"
   # or
   git commit -m "fix: resolve issue with timeline"
   ```

6. **Push your changes** to your fork:
   ```bash
   git push origin your-branch-name
   ```

7. **Open a Pull Request** from your fork to the main repository

## Code Style

- Follow [PEP 8](https://www.python.org/dev/peps/pep-0008/) style guide
- Use type hints for all function signatures and variables
- Write docstrings following [Google style](https://google.github.io/styleguide/pyguide.html#38-comments-and-docstrings)
- Keep lines under 88 characters (Black's default)

## Testing

- Write tests for all new features and bug fixes
- Ensure all tests pass before submitting a pull request
- Aim for good test coverage (90%+)

## Documentation

- Update documentation when adding new features or changing behavior
- Keep docstrings up to date
- Add examples for new features

## Reporting Issues

When reporting issues, please include:

1. A clear, descriptive title
2. Steps to reproduce the issue
3. Expected vs. actual behavior
4. Python version and operating system
5. Any relevant error messages or logs

## Feature Requests

We welcome feature requests! Please:

1. Check if the feature already exists or has been requested
2. Explain why this feature would be valuable
3. Include any relevant use cases or examples

## License

By contributing to Chronovyan, you agree that your contributions will be licensed under the [MIT License](LICENSE).
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
