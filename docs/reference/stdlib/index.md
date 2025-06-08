# Chronovyan Standard Library

Welcome to the Chronovyan Standard Library documentation! This comprehensive collection of modules provides the foundation for building robust and efficient Chronovyan applications.

## 📚 Table of Contents

- [Quick Start](#-quick-start)
- [Core Modules](#-core-modules)
- [Common Patterns](#-common-patterns)
- [Performance Guidelines](#-performance-guidelines)
- [Contributing](#-contributing)
- [Documentation Conventions](#-documentation-conventions)
- [Versioning](#-versioning)
- [License](#-license)
- [Getting Help](#-getting-help)

## 🚀 Quick Start

1. **New to Chronovyan?** Start with the [Language Specification](reference/language/specification.md)
2. **First time using the standard library?** Try these examples:
   ```chronovyan
   // Basic I/O
   IMPORT std.io as io;
   io.println("Hello, Chronovyan!");
   
   // Collections
   IMPORT std.collections as coll;
   LET numbers = VEC![1, 2, 3, 4, 5];
   LET sum = numbers.iter().sum();
   ```
3. **Explore examples** in the [Examples Directory](../../examples/)

## 📦 Core Modules

| Module | Description | Key Features |
|--------|-------------|--------------|
| [Temporal](temporal.md) | Time and timeline manipulation | Timeline management, temporal variables |
| [Concurrency](concurrency.md) | Parallel programming | Threads, async/await, channels |
| [Collections](collections.md) | Data structures | Vectors, maps, sets, algorithms |
| [IO](io.md) | Input/Output | File I/O, streams, serialization |
| [Math](math.md) | Mathematics | Trigonometry, statistics, linear algebra |
| [String](string.md) | Text processing | Unicode, formatting, regex |
| [Time](time.md) | Date and time | Time zones, formatting, arithmetic |
| [Random](random.md) | Random generation | Distributions, sampling |
| [Reflection](reflection.md) | Runtime type info | Dynamic typing, inspection |
| [Testing](testing.md) | Testing framework | Unit tests, benchmarks, mocks |

## 🧩 Common Patterns

### Data Processing Pipeline

```chronovyan
IMPORT std.io as io;
IMPORT std.string as str;

// Read and process lines from a file
WITH file = io.File.open("data.txt", "r") {
    file.lines()
        .map(|line| str.trim(line))
        .filter(|line| !line.is_empty())
        .for_each(|line| {
            // Process each line
        });
}
```

### Concurrent Task Processing

```chronovyan
IMPORT std.concurrency as conc;

// Process items in parallel
FUNC process_items(items: VEC<Item>) -> VEC<Result> {
    LET (tx, rx) = conc.channel();
    
    FOR item IN items {
        LET tx = tx.clone();
        conc.spawn(move || {
            LET result = process_item(item);
            tx.send(result).unwrap();
        });
    }
    
    drop(tx);  // Close the channel
    rx.iter().collect()
}
```

## ⚡ Performance Guidelines

1. **Choose the Right Data Structure**
   - Use `Vec` for indexed access
   - Prefer `HashMap` for key-value lookups
   - Consider `HashSet` for membership tests

2. **Memory Efficiency**
   ```chronovyan
   // Pre-allocate when size is known
   LET mut vec = VEC::with_capacity(1000);
   
   // Use smaller integer types when possible
   LET small: u8 = 255;
   ```

3. **Concurrency Best Practices**
   - Prefer message passing over shared state
   - Use `Arc` for shared ownership
   - Consider thread-local storage

4. **I/O Performance**
   - Use buffered I/O for small, frequent operations
   - Consider memory-mapped files for large files
   - Process data in chunks when possible

## 🤝 Contributing

We welcome contributions to the Chronovyan Standard Library! Here's how you can help:

1. **Report Issues**: Found a bug? [Open an issue](https://github.com/Chronovyan/Chronovyan.github.io/issues)
2. **Improve Documentation**: Fix typos or clarify explanations
3. **Add Examples**: Contribute practical examples
4. **Implement Features**: Check our [roadmap](roadmap/index.md) for planned features

Please read our [Contributing Guide](https://github.com/Chronovyan/Chronovyan.github.io/CONTRIBUTING.md) for details.

## 📝 Documentation Conventions

### Code Examples

```chronovyan
// Use clear, concise examples
FUNC add(a: i32, b: i32) -> i32 {
    RETURN a + b;
}
```

### Type Signatures

- `Type` - Types are in PascalCase
- `variable` - Variables are in snake_case
- `CONSTANT` - Constants are in UPPER_SNAKE_CASE
- `T` - Generic type parameters are in CamelCase

### Parameter Annotations

- `[optional]` - Optional parameters
- `args...` - Variadic parameters
- `// SAFETY:` - Safety requirements for unsafe code

## 🔖 Versioning

The standard library follows [Semantic Versioning](https://semver.org/):

- **MAJOR** version for incompatible API changes
- **MINOR** version for added functionality (backward-compatible)
- **PATCH** version for backward-compatible bug fixes

Check each module's documentation for version-specific information.

## 📜 License

Chronovyan and its documentation are licensed under the [MIT License](https://opensource.org/licenses/MIT).

## ❓ Getting Help

### Documentation
- [Language Reference](reference/language/specification.md)
- [API Documentation](./index.md)
- [Examples](../../examples/)

### Community Support
- [GitHub Discussions](https://github.com/Chronovyan/Chronovyan.github.io/discussions)
- [Discord Community](https://discord.gg/chronovyan)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/chronovyan)

### Report Issues
- [Bug Reports](https://github.com/Chronovyan/Chronovyan.github.io/issues/new?template=bug_report.md)
- [Feature Requests](https://github.com/Chronovyan/Chronovyan.github.io/issues/new?template=feature_request.md)

---

© 2023 Chronovyan Project. All rights reserved.
