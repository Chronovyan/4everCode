# Documentation Tools

This directory contains scripts to help with 4ever documentation development.

## Available Scripts

### `validate_docs.py`

Validates documentation structure, front matter, and internal links.

```bash
# Basic usage
python scripts/validate_docs.py

# Show help
python scripts/validate_docs.py --help
```

### `new_doc.py`

Generates new documentation pages with proper front matter and structure.

```bash
# Create a new guide
python scripts/new_doc.py "My New Guide" --type guide

# Create a new API reference
python scripts/new_doc.py "My New API" --type reference

# List available document types
python scripts/new_doc.py --list-types
```

### `cleanup_docs.ps1`

PowerShell script to clean up old documentation files.

```powershell
# Run the cleanup script
.\scripts\cleanup_docs.ps1
```

### `generate_pngs.py`

Generates PNG versions of SVG assets.

```bash
python scripts/generate_pngs.py
```

## Setup

1. Install Python 3.8+
2. Install requirements:
   ```bash
   pip install -r scripts/requirements-docs.txt
   ```
3. Make scripts executable (Linux/macOS):
   ```bash
   chmod +x scripts/*.py
   ```

## Development

- Follow PEP 8 style guide
- Add type hints for better code clarity
- Document public functions with docstrings
- Add tests for new functionality

## License

This project is licensed under the same terms as the main 4ever project.
