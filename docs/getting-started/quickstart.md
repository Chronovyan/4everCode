# Quick Start Guide

Welcome to Chronovyan! This guide will help you get up and running quickly.

## Basic Usage

1. First, make sure you've completed the [installation](installation.md) steps.

2. Run the Chronovyan compiler on a source file:
   ```bash
   ./chronovyan path/to/your/file.cv
   ```

3. For more options, use the `--help` flag:
   ```bash
   ./chronovyan --help
   ```

## Example

Here's a simple example to get you started:

```chronovyan
// hello.cv
fn main() {
    print("Hello, Chronovyan!");
}
```

Compile and run it with:
```bash
./chronovyan hello.cv
```

## Next Steps

- Check out the [API Reference](../api/) for detailed documentation
- Learn how to [contribute](../CONTRIBUTING.md) to the project
