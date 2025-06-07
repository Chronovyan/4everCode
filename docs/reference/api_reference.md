---
title: API Reference
description: Comprehensive reference for Chronovyan's standard library and built-in functions
---

# Chronovyan API Reference

## Table of Contents

- [1. Resource Management](#1-resource-management)
  - [1.1 ResourceTracker](#11-resourcetracker)
  - [1.2 ResourceProcessor](#12-resourceprocessor)
  - [1.3 Resource Types](#13-resource-types)
- [2. Temporal Operations](#2-temporal-operations)
  - [2.1 Timeline Management](#21-timeline-management)
  - [2.2 Temporal Variables](#22-temporal-variables)
  - [2.3 Quantum Operations](#23-quantum-operations)
- [3. Standard Library](#3-standard-library)
  - [3.1 Core Types](#31-core-types)
  - [3.2 Built-in Functions](#32-built-in-functions)

## 1. Resource Management

### 1.1 ResourceTracker

Tracks and manages temporal resources (Aethel and Chronon).

```chronovyan
// Create a new resource tracker
LET tracker = NEW ResourceTracker(
    initial_aethel: 100,
    initial_chronon: 50
);

// Check current resource levels
LET aethel = tracker.get_aethel();
LET chronon = tracker.get_chronon();

// Consume resources
TRACKED_OPERATION(tracker, 10, 5) {
    // Operation that consumes 10 Aethel and 5 Chronon
}
```

### 1.2 ResourceProcessor

Processes and optimizes resource allocations.

```chronovyan
// Create a processor with a tracker
LET processor = NEW ResourceProcessor(tracker);

// Optimize resource allocation
processor.optimize_allocation(
    required_aethel: 50,
    required_chronon: 25
);

// Process a temporal operation
processor.process_operation(
    operation_type: "TIMELINE_SPLIT",
    parameters: {"branches": 2}
);
```

### 1.3 Resource Types

| Type | Description | Default Value |
|------|-------------|---------------|
| Aethel | Temporal energy resource | 0 |
| Chronon | Basic quantum of time | 0 |
| Timeline | Sequence of temporal events | null |
| QuantumState | Quantum state representation | null |

## 2. Temporal Operations

### 2.1 Timeline Management

```chronovyan
// Create a new timeline
LET timeline = NEW Timeline("main_timeline");

// Add an event
TIMELINE_ADD_EVENT(
    timeline: timeline,
    timestamp: NOW() + 1h,
    callback: \() { PRINT("Event triggered"); }
);

// Branch the timeline
LET branch = TIMELINE_BRANCH("alternate_reality");

// Merge timelines
TIMELINE_MERGE(source: branch, target: timeline);
```

### 2.2 Temporal Variables

```chronovyan
// CONF (Conformity) variable
DECLARE CONF::STATIC MAX_ITERATIONS: INT = 100;

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
LET qbit = QUANTUM_SUPERPOSITION(0, 1);

// Quantum measurement
LET result = MEASURE(qbit);
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
LET now = NOW();

// Create a duration
LET duration = DURATION(hours: 1, minutes: 30);

// Sleep for a duration
SLEEP(duration);

// Create a timer
LET timer = TIMER(START);
// ...
LET elapsed = TIMER(STOP, timer);
```

#### Mathematical Functions

```chronovyan
// Basic math
LET sum = ADD(5, 3);
LET product = MULTIPLY(4, 7);

// Advanced math
LET root = SQRT(16);
LET power = POW(2, 10);

// Random numbers
LET rand = RANDOM(1, 100);
```

#### I/O Functions

```chronovyan
// Print to console
PRINT("Hello, Chronovyan!");

// Read input
LET name = INPUT("Enter your name: ");

// File operations
LET file = FILE_OPEN("data.txt", "r");
LET content = FILE_READ(file);
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

- [Language Specification](/reference/language_specification.md)
- [Examples](/examples/temporal_examples.md)
- [Performance Guide](/guides/performance.md)
