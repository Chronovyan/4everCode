# Contributing to Forever Code

Thank you for your interest in contributing to Forever Code! We appreciate your time and effort in helping us improve this project. This guide will help you get started with contributing.

## Table of Contents

- [Code of Conduct](#code-of-conduct)
- [How Can I Contribute?](#how-can-i-contribute)
  - [Reporting Bugs](#reporting-bugs)
  - [Suggesting Enhancements](#suggesting-enhancements)
  - [Your First Code Contribution](#your-first-code-contribution)
  - [Pull Requests](#pull-requests)
- [Development Environment](#development-environment)
- [Style Guides](#style-guides)
  - [Git Commit Messages](#git-commit-messages)
  - [Code Style](#code-style)
  - [Documentation Style](#documentation-style)
- [License](#license)

## Code of Conduct

This project and everyone participating in it is governed by our [Code of Conduct](CODE_OF_CONDUCT.md). By participating, you are expected to uphold this code. Please report any unacceptable behavior to [contact@forevercode.dev](mailto:contact@forevercode.dev).

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check [this list](#before-submitting-a-bug-report) as you might find out that you don't need to create one. When you are creating a bug report, please include as many details as possible.

### Suggesting Enhancements

Enhancement suggestions are tracked as [GitHub issues](https://github.com/ForeverCode/forevercode/issues). When you are creating an enhancement suggestion, please include:

1. A clear and descriptive title
2. A step-by-step description of the suggested enhancement
3. Examples of the new feature in action
4. Any additional context or screenshots

### Your First Code Contribution

1. Fork the repository
2. Create a new branch: `git checkout -b my-branch-name`
3. Make your changes
4. Test your changes
5. Commit your changes: `git commit -m 'Add some feature'`
6. Push to the branch: `git push origin my-branch-name`
7. Open a pull request

### Pull Requests

- Include screenshots and animated GIFs in your pull request whenever possible
- Follow the [style guides](#style-guides)
- Make sure all tests pass
- Update the documentation for API changes
- Include tests for new features and bug fixes

## Development Environment

### Prerequisites

- Python 3.8+
- Node.js 16+
- Git

### Setup

1. Fork and clone the repository
2. Install Python dependencies: `pip install -r requirements-dev.txt`
3. Install pre-commit hooks: `pre-commit install`
4. Start the development server: `mkdocs serve`

## Style Guides

### Git Commit Messages

- Use the present tense ("Add feature" not "Added feature")
- Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
- Limit the first line to 72 characters or less
- Reference issues and pull requests liberally
- Consider starting the commit message with an applicable emoji:
  - 🎨 `:art:` when improving the format/structure of the code
  - 🐛 `:bug:` when fixing a bug
  - 🔥 `:fire:` when removing code or files
  - ✨ `:sparkles:` when adding a new feature
  - 📝 `:memo:` when writing docs
  - 🚀 `:rocket:` when improving performance
  - ✅ `:white_check_mark:` when adding tests
  - 🔧 `:wrench:` when changing configuration
  - ⬆️ `:arrow_up:` when upgrading dependencies

### Code Style

- Follow [PEP 8](https://www.python.org/dev/peps/pep-0008/) for Python code
- Use [Black](https://github.com/psf/black) for code formatting
- Use [isort](https://github.com/timothycrosley/isort) for import sorting
- Use [Flake8](https://flake8.pycqa.org/) for linting

### Documentation Style

- Use [Google Style](https://sphinxcontrib-napoleon.readthedocs.io/en/latest/example_google.html) docstrings
- Keep lines under 100 characters
- Use [MkDocs](https://www.mkdocs.org/) for documentation
- Follow the [Google Developer Documentation Style Guide](https://developers.google.com/style)

## License

By contributing to Forever Code, you agree that your contributions will be licensed under its [MIT License](LICENSE).
