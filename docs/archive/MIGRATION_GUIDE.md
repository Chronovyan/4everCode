# Chronovyan Documentation Migration Guide

This guide provides instructions for migrating the Chronovyan documentation to the new structure.

## Overview

The documentation is being reorganized to improve navigation, findability, and maintainability. This involves:

1. Restructuring the directory layout
2. Adding consistent frontmatter to all files
3. Updating internal links
4. Standardizing formatting and style

## Prerequisites

- Python 3.8 or higher
- pip (Python package manager)
- Git (for version control)

## Setup

1. **Clone the repository** (if you haven't already):
   ```bash
   git clone https://github.com/chronovyan/chronovyan.git
   cd chronovyan
   ```

2. **Create and activate a virtual environment** (recommended):
   ```bash
   # Windows
   python -m venv venv
   .\venv\Scripts\activate
   
   # macOS/Linux
   python3 -m venv venv
   source venv/bin/activate
   ```

3. **Install dependencies**:
   ```bash
   pip install -r scripts/requirements-migration.txt
   ```

## Migration Process

### 1. Run the Migration Script

```bash
python scripts/migrate_content.py
```

This will:
- Copy files to their new locations
- Update internal links
- Preserve frontmatter
- Generate a log file (`migration.log`)

### 2. Review Changes

1. Check the migration log for any errors or warnings:
   ```bash
   type migration.log
   ```

2. Review the new documentation structure in the `docs` directory.

3. Test the documentation locally:
   ```bash
   # Install MkDocs and theme
   pip install -r docs-requirements.txt
   
   # Serve the documentation
   mkdocs serve
   ```
   
   Open http://localhost:8000 in your browser to preview the documentation.

### 3. Manual Updates

Some content may require manual updates:

1. **Update hardcoded links** that weren't caught by the migration script
2. **Review and update frontmatter** for accuracy
3. **Update cross-references** between documents
4. **Check for broken images** and other assets

### 4. Test the Build

Build the documentation to check for errors:

```bash
mkdocs build --strict --clean
```

## Directory Structure

The new documentation structure is as follows:

```
docs/
├── getting-started/      # Getting started guides
│   ├── installation.md
│   ├── quick-start.md
│   └── tutorials/
├── language/             # Language reference
│   ├── syntax.md
│   ├── types.md
│   ├── control-flow.md
│   └── ...
├── temporal-programming/  # Temporal programming concepts
│   ├── concepts.md
│   ├── paradox-avoidance.md
│   └── ...
├── advanced/             # Advanced topics
│   ├── type-system.md
│   └── ...
├── api/                  # API reference
│   ├── standard-library.md
│   └── ...
├── development/          # Development guides
│   ├── building.md
│   └── ...
├── community/            # Community resources
│   ├── code-of-conduct.md
│   └── ...
└── assets/              # Static assets
    ├── images/
    ├── css/
    └── js/
```

## Frontmatter

All documentation files should include frontmatter with at least the following fields:

```yaml
---
title: "Page Title"
description: "Brief description for search engines and previews"
weight: 10  # Controls ordering in navigation
---
```

## Style Guide

Follow these guidelines when updating documentation:

1. **Headings**: Use sentence case (e.g., "Installation" not "INSTALLATION")
2. **Code**: Use code fences with language specification
3. **Links**: Use relative paths without file extensions
4. **Images**: Place in `assets/images/` and use relative paths
5. **Line length**: Keep lines under 100 characters

## Troubleshooting

### Common Issues

1. **Broken Links**:
   - Run `mkdocs serve` and check the console for link errors
   - Use absolute paths for cross-references

2. **Missing Dependencies**:
   - Ensure all dependencies are installed: `pip install -r scripts/requirements-migration.txt`

3. **Frontmatter Errors**:
   - Check for proper YAML syntax
   - Ensure consistent indentation

4. **Encoding Issues**:
   - Files should be saved as UTF-8
   - Run the encoding fix script if needed: `python scripts/fix_encoding.py`

## Next Steps

1. Review all migrated content for accuracy
2. Update any remaining hardcoded paths
3. Test all internal and external links
4. Update the documentation build process if needed
5. Update CI/CD pipelines for the new structure

## Contributing

Contributions to improve the documentation are welcome! Please see our [Contributing Guide](CONTRIBUTING.md) for more information.

## License

This documentation is licensed under the [MIT License](LICENSE).
