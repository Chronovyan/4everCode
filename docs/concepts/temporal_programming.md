---
title: Temporal Programming
description: Core concepts of temporal programming in Chronovyan
---

# Temporal Programming in Chronovyan

## The Great Duality

Chronovyan is built around two fundamental principles that govern all temporal operations:

### 1. Order (Conformity)

**Stability and Predictability**
- Enforces deterministic behavior
- Ensures consistent execution across runs
- Provides a stable foundation for temporal operations

**Key Concepts:**
- `CONF` variables (Conformity)
- `::STATIC` flag for immutable values
- Deterministic control flow
- Predictable resource consumption

**Example:**
```chronovyan
// A static, unchanging value
DECLARE CONF::STATIC MAX_ITERATIONS : INT = 1000;

// A function with deterministic behavior
FUNC INT calculate_fibonacci(INT n) {
    IF (n <= 1) {
        RETURN n;
    }
    RETURN calculate_fibonacci(n-1) + calculate_fibonacci(n-2);
}
```

### 2. Flux (Rebellion)

**Dynamic Adaptation**
- Enables temporal manipulation
- Allows for adaptive behavior
- Supports runtime modifications

**Key Concepts:**
- `REB` variables (Rebellion)
- `::FLUX` flag for mutable state
- Dynamic resource allocation
- Temporal manipulation operations

**Example:**
```chronovyan
// A mutable value that can change over time
DECLARE REB::FLUX current_time : TIMESTAMP = NOW();

// Function that can modify temporal state
FUNC VOID adjust_timeline(FLOAT factor) {
    // Modify the flow of time
    SET_TIME_DILATION(factor);
}
```

## Temporal Resources

Chronovyan manages two primary temporal resources:

### 1. Chronon
- Basic quantum of time
- Represents discrete moments in the timeline
- Used for scheduling and ordering events

### 2. Aethel
- Temporal energy resource
- Consumed by temporal operations
- Must be managed carefully to prevent paradoxes

**Example:**
```chronovyan
// Allocate Aethel for temporal operations
ALLOCATE_AETHEL(100);

// Perform a temporal operation that consumes Aethel
TIMELINE_SPLIT("branch_1", 0.5);  // Consumes Aethel
```

## Temporal Safety

Chronovyan enforces temporal safety through:

1. **Type System**
   - Temporal types with compile-time checking
   - Resource tracking
   - Side effect analysis

2. **Runtime Checks**
   - Paradox detection
   - Resource exhaustion
   - Timeline consistency

3. **Deterministic Execution**
   - Predictable behavior
   - Reproducible results
   - Controlled non-determinism when explicitly requested

## Next Steps

- [Resource Management](resource_management.md)
- [Concurrency Model](concurrency.md)
- [Language Reference](/reference/syntax.md)
