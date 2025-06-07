---
title: Readme
description: Documentation for README
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Chronovyan Documentation Tools

This directory contains scripts and tools for managing the Chronovyan documentation.

## Available Scripts

### Setup and Build

- `setup_docs.ps1` - Sets up the development environment for the documentation (Windows)
- `setup_docs.sh` - Sets up the development environment (macOS/Linux)
- `build_docs.ps1` - Builds and validates the documentation (Windows)

### Migration and Maintenance

- `migrate_docs.py` - Migrates documentation to the new structure
- `add_frontmatter.py` - Adds YAML frontmatter to markdown files
- `fix_encoding.py` - Fixes file encoding issues in documentation
- `audit-docs.py` - Audits documentation files and generates reports
- `validate_docs.py` - Validates documentation for common issues

## Usage

### Setting Up the Environment (Windows)

```powershell
# Run the setup script
.\scripts\setup_docs.ps1

# Activate the virtual environment
.\venv\Scripts\Activate.ps1
```

### Building the Documentation

```powershell
# Build the documentation
.\scripts\build_docs.ps1

# Build and serve locally
.\scripts\build_docs.ps1 -Serve

# Build and deploy to GitHub Pages
.\scripts\build_docs.ps1 -Deploy
```

### Running Individual Scripts

```powershell
# Run the documentation validator
python .\scripts\validate_docs.py

# Migrate documentation to new structure
python .\scripts\migrate_docs.py

# Add frontmatter to markdown files
python .\scripts\add_frontmatter.py
```

## Documentation Structure

- `docs/` - Main documentation source
  - `getting-started/` - Getting started guides
  - `language/` - Language reference
  - `temporal-programming/` - Temporal programming concepts
  - `advanced/` - Advanced topics
  - `api/` - API reference
  - `development/` - Development guides
  - `community/` - Community resources
  - `assets/` - Static assets (images, CSS, JS)

## Contributing

1. Fork the repository
2. Create a new branch for your changes
3. Make your changes following the [style guide](../docs/contributing/style-guide.md)
4. Run the validation script: `python .\scripts\validate_docs.py`
5. Submit a pull request

## License

This project is licensed under the MIT License - see the [LICENSE](../LICENSE) file for details.
