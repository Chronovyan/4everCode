# Script to reorganize documentation files
# Run this script from the project root

# Move getting started guides
if (Test-Path "docs/getting_started.md") {
    Move-Item -Path "docs/getting_started.md" -Destination "docs/guides/getting-started/index.md" -Force
}

# Move language reference
if (Test-Path "docs/reference/language_specification.md") {
    Move-Item -Path "docs/reference/language_specification.md" -Destination "docs/reference/language/specification.md" -Force
}

# Move standard library documentation
if (Test-Path "docs/reference/stdlib/README.md") {
    Move-Item -Path "docs/reference/stdlib/README.md" -Destination "docs/reference/stdlib/index.md" -Force
}

# Move development guides
if (Test-Path "docs/BUILD.md") {
    Move-Item -Path "docs/BUILD.md" -Destination "docs/development/building/index.md" -Force
}

if (Test-Path "CONTRIBUTING.md") {
    Copy-Item -Path "CONTRIBUTING.md" -Destination "docs/development/contributing/index.md" -Force
}

# Move community resources
if (Test-Path "CODE_OF_CONDUCT.md") {
    Copy-Item -Path "CODE_OF_CONDUCT.md" -Destination "docs/community/code-of-conduct.md" -Force
}

if (Test-Path "SECURITY.md") {
    Copy-Item -Path "SECURITY.md" -Destination "docs/community/security.md" -Force
}

# Create a new README for the docs directory
@"
# Chronovyan Documentation

Welcome to the Chronovyan documentation! This directory contains comprehensive documentation for the Chronovyan programming language.

## Documentation Structure

- **Guides**: Tutorials and how-to guides
  - [Getting Started](./guides/getting-started/)
  - [Advanced Topics](./guides/advanced/)
  - [Examples](./guides/examples/)

- **Reference**
  - [Language Reference](./reference/language/)
  - [Standard Library](./reference/stdlib/)

- **Development**
  - [Building Chronovyan](./development/building/)
  - [Contributing](./development/contributing/)

- **Community**
  - [Code of Conduct](./community/code-of-conduct.md)
  - [Security Policy](./community/security.md)

## Building the Documentation

To build the documentation locally:

1. Install the required dependencies:
   ```bash
   pip install -r docs-requirements.txt
   ```

2. Build the documentation:
   ```bash
   mkdocs build
   ```

3. Serve the documentation locally:
   ```bash
   mkdocs serve
   ```

## Contributing to Documentation

We welcome contributions to improve our documentation! Please see our [Contributing Guide](./development/contributing/) for more information.
"@ | Out-File -FilePath "docs/README.md" -Encoding utf8

Write-Host "Documentation reorganization complete!"
