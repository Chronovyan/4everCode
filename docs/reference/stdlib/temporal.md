---
title: Temporal Module
description: Time and timeline manipulation
weight: 10
---

# Temporal Module

## Overview

The `std.temporal` module provides primitives for time manipulation, timeline management, and temporal programming in Chronovyan.

## Timeline Management

### Types

#### `Timeline<T>`
Represents a sequence of temporal events.

```chronovyan
// Create a new timeline
DECLARE timeline = TIMELINE_NEW("main");
```

### Functions

#### `TIMELINE_NEW(name: STRING) -> Timeline`
Creates a new timeline with the given name.

#### `TIMELINE_ADD_EVENT(timeline: Timeline, when: MOMENT, callback: FUNCTION)`
Schedules an event to occur at a specific time.

```chronovyan
TIMELINE_ADD_EVENT(timeline, NOW() + 1s, \() {
    PRINT("One second has passed");
});
```

#### `TIMELINE_BRANCH(timeline: Timeline, name: STRING) -> Timeline`
Creates a new branch from an existing timeline.

#### `TIMELINE_MERGE(source: Timeline, target: Timeline)`
Merges the source timeline into the target timeline.

## Temporal Variables

### `CONF` (Conformity)
Immutable variables that maintain temporal consistency.

```chronovyan
DECLARE CONF::STATIC VERSION = "1.0.0";
```

### `REB` (Rebellion)
Mutable variables that can change over time.

```chronovyan
DECLARE REB::FLUX counter = 0;
```

## Quantum Operations

### `QUANTUM<T>`
Represents a quantum state that can be in superposition.

```chronovyan
DECLARE q = QUBIT(0);
H(q);  // Put into superposition
```

### Common Quantum Gates

- `H(qubit)`: Hadamard gate
- `X(qubit)`: Pauli-X gate
- `CNOT(control, target)`: Controlled-NOT gate
- `MEASURE(qubit) -> BOOL`: Measures a qubit

## Temporal Resources

### `AETHEL`
Temporal energy resource used for temporal operations.

```chronovyan
DECLARE energy: AETHEL = 100.0;
```

### `CHRONON`
Basic quantum of time.

```chronovyan
DECLARE start = NOW();
// ...
DECLARE elapsed = NOW() - start;
```

## Example: Simple Timeline

```chronovyan
IMPORT std.temporal as time;

FUNC VOID main() {
    DECLARE tl = time.TIMELINE_NEW("example");
    
    // Schedule events
    FOR i IN 1..5 {
        time.TIMELINE_ADD_EVENT(tl, time.NOW() + i * 1s, \() {
            PRINT("Event ", i, " at ", time.NOW());
        });
    }
    
    // Run the timeline
    time.TIMELINE_RUN(tl);
}
```

## Performance Considerations

- Timeline operations are O(1) for insertion and O(log n) for event dispatch
- Quantum operations require significant computational resources
- Use `CONF` variables for values that don't change to enable optimizations

## Related

- [Concurrency](./concurrency.md) - For concurrent programming with temporal types
- [Time](./time.md) - For date and time handling
- [Random](./random.md) - For generating random temporal values
- [Testing](./testing.md) - For testing temporal code

## Examples

### Timeline Visualization

```mermaid
timeline
    title Project Timeline
    section Phase 1
        Task 1 :a1, 2023-01-01, 30d
        Task 2 :after a1, 20d
    section Phase 2
        Task 3 :2023-03-01, 12d
        Task 4 :5d after Task 3
```

### Quantum Circuit

```mermaid
quantumcircuit
    qreg q[2];
    creg c[2];
    
    h q[0];
    cx q[0], q[1];
    measure q[0] -> c[0];
    measure q[1] -> c[1];
```

## See Also

- [Standard Library](../stdlib/index.md) - Overview of all standard library modules
- [Chronovyan Language Specification](reference/language/specification.md) - For language-level temporal features
