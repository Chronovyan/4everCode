# Installation

This guide will help you install Chronovyan on your system.

## Prerequisites

- Python 3.8 or higher
- pip (Python package manager)
- Git (for development)

## Installation Methods

### From Source (Recommended)

Since Chronovyan is in active development, the recommended way to install it is from source:

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
python -c "from chronovyan import Timeline; print('Chronovyan imported successfully')"
```

## Updating Chronovyan

To update to the latest version, pull the latest changes and reinstall:

```bash
git pull origin main
pip install -e .
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
