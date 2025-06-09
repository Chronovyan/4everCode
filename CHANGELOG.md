# Changelog

All notable changes to the Forever project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- **Core Library**:
  - `Timeline` class for managing and executing events in temporal order
  - `Event` class for defining time-based actions with callbacks
  - Support for delayed events and event dependencies
  - Thread-safe operations for concurrent timeline management

- **Command Line Interface**:
  - `Forever` command for running timelines from the command line
  - Support for creating and managing events with delays and data

- **Examples**:
  - Basic usage example demonstrating core functionality
  - Event synchronization example showing task dependencies

- **Developer Tooling**:
  - Comprehensive test suite with pytest
  - Type annotations and static type checking with mypy
  - Code formatting with Black and isort
  - Linting with flake8
  - Automated testing with GitHub Actions

- **Documentation**:
  - API reference documentation
  - Getting started guide
  - Example code and tutorials

### Changed
- Modernized project structure to follow Python best practices
- Updated build system to use pyproject.toml (PEP 621)
- Improved error handling and validation

### Fixed
- Various bug fixes and performance improvements

## [0.1.0] - 2025-06-07
### Added
- Initial release of Forever Python package
- Core timeline and event management functionality
- Basic command-line interface
- Example applications and documentation

[Unreleased]: https://github.com/Forever/Forever/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/Forever/Forever/releases/tag/v0.1.0
