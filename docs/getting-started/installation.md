# Installation

This guide will help you install Chronovyan on your system.

## Prerequisites

- Python 3.8 or higher
- pip (Python package manager)
- Git (for development)

## Installation Methods

### Using pip (Recommended)

The easiest way to install Chronovyan is using pip:

```bash
pip install chronovyan
```

### From Source

If you want to install the latest development version:

```bash
# Clone the repository
git clone https://github.com/Chronovyan/Chronovyan.github.io.git
cd Chronovyan

# Install in development mode
pip install -e .
```

### Verifying the Installation

After installation, you can verify that Chronovyan is installed correctly by running:

```bash
python -c "import chronovyan; print('Chronovyan version:', chronovyan.__version__)"
```

## Updating Chronovyan

To update Chronovyan to the latest version:

```bash
pip install --upgrade chronovyan
```

## Troubleshooting

### Common Issues

1. **Permission Errors**: If you encounter permission errors, try installing with the `--user` flag:
   ```bash
   pip install --user chronovyan
   ```

2. **Python Version**: Ensure you're using Python 3.8 or higher. Check your Python version with:
   ```bash
   python --version
   ```

3. **Dependencies**: If you encounter dependency issues, try:
   ```bash
   pip install --upgrade pip setuptools wheel
   ```

## Next Steps

Now that you have Chronovyan installed, check out the [First Steps](first-steps.md) guide to start using it!
