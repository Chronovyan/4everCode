# Installation

This guide will walk you through installing Forever Code on your system.

## Prerequisites

- Python 3.8 or higher
- pip (Python package installer)
- Git (optional, for version control integration)

## Installation Methods

### Using pip (Recommended)

The easiest way to install Forever Code is using pip:

```bash
pip install forevercode
```

### From Source

If you want to install the latest development version:

```bash
# Clone the repository
git clone https://github.com/ForeverCode/forevercode.git
cd forevercode

# Install in development mode
pip install -e .
```

### Verifying Installation

To verify that Forever Code is installed correctly, run:

```bash
python -c "import forever; print(forever.__version__)"
```

You should see the version number printed to the console.

## Upgrading

To upgrade to the latest version:

```bash
pip install --upgrade forevercode
```

## Next Steps

- [Your First Program](./first-program.md)
- [Core Concepts](../concepts/)
