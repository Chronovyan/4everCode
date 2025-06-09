---
title: API Reference
description: Comprehensive reference for Chronovyan's standard library and built-in functions
weight: 20
---

# Chronovyan API Reference

## Table of Contents

- [Standard Library](./stdlib/index.md) - Comprehensive reference for all standard library modules
- [Language Specification](reference/language/specification.md) - Detailed language syntax and semantics
- [Temporal Programming](./stdlib/temporal.md) - Time and timeline manipulation
- [Concurrency](./stdlib/concurrency.md) - Parallel and concurrent programming
- [Collections](./stdlib/collections.md) - Data structures and algorithms

## Quick Links

- [Getting Started Guide](guides/getting-started/index.md)
- [Examples](../examples/index.md)
- [Contributing Guide](https://github.com/Chronovyan/Chronovyan.github.io/CONTRIBUTING.md)

## Overview

This section provides a high-level overview of the Chronovyan API. For detailed documentation, please refer to the specific module documentation linked above.

## Core Concepts

### Temporal Programming

Chronovyan's unique approach to handling time and concurrency is documented in the [Temporal Programming](./stdlib/temporal.md) guide.

### Concurrency Model

Learn about Chronovyan's concurrency primitives and best practices in the [Concurrency](./stdlib/concurrency.md) documentation.

### Standard Library

Explore the comprehensive [Standard Library](./stdlib/index.md) documentation for all available modules and functions.

## Migration Guides

- [From v0.1 to v0.2](../migration/0.1_to_0.2.md)
- [From v0.2 to v0.3](../migration/0.2_to_0.3.md)

## API Stability

Chronovyan follows [Semantic Versioning](https://semver.org/). The following stability levels are used:

- **Stable**: Fully supported and backward compatible
- **Beta**: Feature complete but may have minor issues
- **Experimental**: Under active development, API may change
- **Deprecated**: Scheduled for removal in a future version

## Getting Help

- [GitHub Issues](https://github.com/Chronovyan/Chronovyan.github.io/issues)
- [Discord Community](https://discord.gg/chronovyan)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/chronovyan)

// REB (Rebellion) variable
DECLARE REB::FLUX current_state: STRING = "initial";

// Temporal variable with quantum properties
DECLARE QUANTUM::ENTANGLED particle_spin: INT = 0;
```

### 2.3 Quantum Operations

```chronovyan
// Create quantum entanglement
QUANTUM_ENTANGLE(particle_a, particle_b);

// Superposition
DECLARE qbit = QUANTUM_SUPERPOSITION(0, 1);

// Quantum measurement
DECLARE result = MEASURE(qbit);
```

## 3. Standard Library

### 3.1 Core Types

| Type | Description |
|------|-------------|
| INT | 64-bit signed integer |
| FLOAT | 64-bit floating point |
| BOOL | Boolean value |
| STRING | UTF-8 encoded string |
| ARRAY<T> | Dynamic array of type T |
| MAP<K,V> | Hash map with keys K and values V |
| OPTIONAL<T> | May contain a value of type T or be empty |
| RESULT<T,E> | Represents success (T) or failure (E) |

### 3.2 Built-in Functions

#### Temporal Functions

```chronovyan
// Get current timestamp
DECLARE now = NOW();

// Create a duration
DECLARE duration = DURATION(hours: 1, minutes: 30);

// Sleep for a duration
SLEEP(duration);

// Create a timer
DECLARE timer = TIMER(START);
// ...
DECLARE elapsed = TIMER(STOP, timer);
```

#### Mathematical Functions

```chronovyan
// Basic math
DECLARE sum = ADD(5, 3);
DECLARE product = MULTIPLY(4, 7);

// Advanced math
DECLARE root = SQRT(16);
DECLARE power = POW(2, 10);

// Random numbers
DECLARE rand = RANDOM(1, 100);
```

#### I/O Functions

```chronovyan
// Print to console
PRINT("Hello, Chronovyan!");

// Read input
DECLARE name = INPUT("Enter your name: ");

// File operations
DECLARE file = FILE_OPEN("data.txt", "r");
DECLARE content = FILE_READ(file);
FILE_CLOSE(file);
```

## Best Practices

1. **Resource Management**
   - Always check resource availability before operations
   - Use TRACKED_OPERATION for automatic resource tracking
   - Implement cleanup in FINALLY blocks

2. **Error Handling**
   - Use RESULT type for fallible operations
   - Handle temporal paradoxes explicitly
   - Provide meaningful error messages

3. **Performance**
   - Minimize temporal jumps
   - Use appropriate data structures
   - Profile resource-intensive operations

## Version Information

- **Current Version**: 1.0.0
- **Last Updated**: 2023-10-21
- **API Stability**: Experimental

## See Also

- [Language Specification](reference/language/specification.md)
- [Standard Library](./stdlib/index.md)
- [Contributing Guide](https://github.com/Chronovyan/Chronovyan.github.io/CONTRIBUTING.md)
