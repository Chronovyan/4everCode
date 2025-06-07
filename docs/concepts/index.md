# Core Concepts

Welcome to the core concepts of Chronovyan, a statically-typed temporal programming language. This section covers the fundamental principles that make Chronovyan unique and powerful.

## Table of Contents

- [Temporal Programming](temporal_programming.md)
  - [The Great Duality](#the-great-duality)
  - [Temporal Resources](#temporal-resources)
  - [Temporal Safety](#temporal-safety)
- [Resource Management](resource_management.md)
  - [Chronon](#chronon)
  - [Aethel](#aethel)
  - [Allocation Strategies](#allocation-strategies)
- [Concurrency Model](concurrency.md)
  - [Threading](#threading-model)
  - [Synchronization](#synchronization-primitives)
  - [Temporal Consistency](#temporal-consistency)

## The Great Duality

Chronovyan is built around two fundamental principles that govern all temporal operations:

1. **Order (Conformity)**
   - Stability and predictability
   - Enforced through `CONF` variables and `::STATIC` flags
   - Ensures deterministic behavior

2. **Flux (Rebellion)**
   - Dynamic adaptation and change
   - Enabled by `REB` variables and `::FLUX` flags
   - Allows for temporal manipulation

## Temporal Resources

### Chronon
- Basic quantum of time
- Represents discrete moments in the timeline
- Used for scheduling and ordering events

### Aethel
- Temporal energy resource
- Consumed by temporal operations
- Must be managed to prevent paradoxes

## Memory Management

Chronovyan provides several memory management strategies:

1. **Static Allocation**
   - Fixed at compile-time
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
