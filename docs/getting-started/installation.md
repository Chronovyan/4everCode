# Installation

## Prerequisites

- Python 3.8 or higher
- pip (Python package manager)

## Installation Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/Chronovyan/Chronovyan.git
   cd Chronovyan
   ```

2. Install the required dependencies:
   ```bash
   pip install -r requirements.txt
   ```

3. Build the project:
   ```bash
   mkdir build
   cd build
   cmake ..
   cmake --build .
   ```

## Verifying the Installation

Run the tests to verify everything is working correctly:

```bash
ctest -C Debug
```

## Next Steps

- [Quick Start Guide](quickstart.md)
- [API Reference](../api/)
- [Contributing Guide](../CONTRIBUTING.md)
