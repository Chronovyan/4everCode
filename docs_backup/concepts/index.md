# Core Concepts

Welcome to the core concepts of Chronovyan, a statically-typed temporal programming language. This section covers the fundamental principles that make Chronovyan unique and powerful.

## Table of Contents

- [Temporal Programming](temporal_programming.md)
  - [The Great Duality](#the-great-duality)
  - [Temporal Resources](#temporal-resources)
- [Resource Management](resource_management.md)
  - [Chronon](#chronon)
  - [Aethel](#aethel)
- [Concurrency](concurrency.md)
- [Advanced Features](advanced_features.md)

## The Great Duality {#the-great-duality}

Chronovyan is built around two fundamental principles that govern all temporal operations:

1. **Order (Conformity)**
   - Stability and predictability
   - Enforced through `CONF` variables and `::STATIC` flags
   - Ensures deterministic behavior
   - Prevents temporal paradoxes
   - Enables compile-time optimizations

2. **Flux (Rebellion)**
   - Dynamic adaptation and change
   - Enabled by `REB` variables and `::FLUX` flags
   - Allows for temporal manipulation
   - Supports runtime flexibility
   - Enables adaptive algorithms

## Temporal Resources {#temporal-resources}

### Chronon {#chronon}
- Basic quantum of time in Chronovyan
- Represents discrete moments in the timeline
- Used for scheduling and ordering events
- Immutable and strictly increasing
- Fundamental unit of temporal measurement

### Aethel {#aethel}
- Temporal energy resource
- Consumed by temporal operations
- Must be managed to prevent paradoxes
- Automatically tracked by the runtime
- Can be optimized by the compiler

## Memory Management

Chronovyan provides several memory management strategies:

1. **Static Allocation**
   - Fixed at compile-time
   - Zero runtime overhead
   - Used for constant data

2. **Arena Allocation**
   - Efficient for short-lived objects
   - Fast allocation/deallocation
   - Reduces memory fragmentation

3. **Reference Counting**
   - Automatic memory management
   - Predictable performance
   - No garbage collection pauses

## Performance Considerations

When working with Chronovyan, keep these performance considerations in mind:

1. **Temporal Locality**
   - Group related temporal operations
   - Minimize context switches
   - Optimize for cache coherence

2. **Resource Management**
   - Monitor `Aethel` consumption
   - Use appropriate allocation strategies
   - Profile and optimize hot paths

3. **Concurrency Model**
   - Leverage temporal isolation
   - Minimize shared state
   - Use message passing when appropriate
   - Most efficient for known sizes

2. **Dynamic Allocation**
   - Flexible runtime allocation
   - Requires explicit management

3. **Pooled Allocation**
   - Pre-allocated resource pools
   - Reduces allocation overhead

## Concurrency Model

Chronovyan's concurrency model is designed to handle temporal operations safely and efficiently:

- **Temporal Threads**: Lightweight, cooperative threads
- **Worker Pools**: For CPU-bound operations
- **Channels**: Thread-safe communication
- **Synchronization Primitives**: Locks, barriers, and conditions

## Getting Started

Ready to dive deeper? Explore the following sections:

- [Temporal Programming](temporal_programming.md): Learn about time as a first-class citizen
- [Resource Management](resource_management.md): Master Chronon and Aethel
- [Concurrency](concurrency.md): Understand parallel execution in Chronovyan
- **Locks**: Protect shared resources

## Time Management

Chronovyan provides flexible time management:

- **Wall Time**: Real-world time
- **Simulation Time**: Virtual time for testing
- **Time Scaling**: Speed up or slow down time

## Next Steps

- [Getting Started](../getting-started/installation.md) - Set up Chronovyan
- [API Reference](../reference/index.md) - Detailed API documentation
- [Examples](../examples/index.md) - Practical examples of Chronovyan in action
