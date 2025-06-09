---
title: Chronovyan Language Specification
description: Official specification for the Chronovyan programming language
---

# Chronovyan Language Specification

## 1. Introduction

Chronovyan is a statically-typed, temporal programming language designed for building time-aware applications. It introduces first-class concepts of time, temporal resources, and timeline management.

### 1.1 Design Philosophy

- **Temporal First**: Time is a first-class citizen
- **Type Safety**: Strong, static typing with type inference
- **Determinism**: Predictable execution model
- **Performance**: Efficient resource management

### 1.2 Language Version

- **Version**: 1.0.0
- **Status**: Stable
- **Release Date**: 2023-10-21

## 2. Lexical Structure

### 2.1 Character Set

- **Encoding**: UTF-8
- **Case Sensitivity**: Yes
- **Whitespace**: Spaces, tabs, newlines, and comments

### 2.2 Comments

```chronovyan
// Single-line comment

/* Multi-line
   comment */

/// Documentation comment
/// @param name Parameter description
```

## 3. Basic Syntax

### 3.1 Program Structure

```chronovyan
// Module declaration
MODULE example;

// Import statements
IMPORT std.io;
IMPORT temporal.core;

// Constant declarations
CONF INT MAX_ITERATIONS = 100;

// Type declarations
TYPE Point {
    x: FLOAT;
    y: FLOAT;
    z: FLOAT;
}

// Function definition
FUNC INT add(INT a, INT b) {
    RETURN a + b;
}

// Main function
FUNC VOID main() {
    // Program entry point
    io::println("Hello, Chronovyan!");
}
```

### 3.2 Variables and Mutability

```chronovyan
// CONF (Conformity) - immutable by default
CONF INT immutable = 42;

// REB (Rebellion) - mutable
REB INT mutable = 0;

// Type inference
DECLARE inferred = "Type inferred as string";

// Constants
CONST FLOAT PI = 3.14159;
```

### 3.3 Temporal Variables

```chronovyan
// Static temporal variable
DECLARE CONF::STATIC start_time = NOW();

// Flux (mutable) temporal variable
DECLARE REB::FLUX current_state = "initial";

// Quantum variable
DECLARE QUANTUM::ENTANGLED qbit = 0;
```

## 4. Types

### 4.1 Primitive Types

| Type     | Description                     | Default |
|----------|---------------------------------|---------|
| BOOL     | Boolean (true/false)            | false   |
| INT      | 64-bit signed integer           | 0       |
| FLOAT    | 64-bit floating point           | 0.0     |
| CHAR     | Unicode code point              | '\0'    |
| STRING   | UTF-8 encoded string            | ""      |
| DURATION | Time duration                   | 0s      |
| MOMENT   | Point in time                   | EPOCH   |
| VOID     | No value                        | N/A     |


### 4.2 Composite Types

#### 4.2.1 Arrays

```chronovyan
// Fixed-size array
DECLARE numbers: INT[5] = [1, 2, 3, 4, 5];

// Dynamic array
DECLARE dynamic: ARRAY<STRING> = ["one", "two"];
dynamic.push("three");
```

#### 4.2.2 Tuples

```chronovyan
DECLARE pair: (INT, STRING) = (42, "answer");
DECLARE (num, str) = pair;  // Destructuring
```

#### 4.2.3 Structs

```chronovyan
TYPE Person {
    name: STRING;
    age: INT;
    
    FUNC STRING greet() {
        RETURN "Hello, " + name;
    }
}

DECLARE alice = Person { name: "Alice", age: 30 };
```

### 4.3 Temporal Types

| Type         | Description                     |
|--------------|---------------------------------|
| TIMELINE     | Sequence of temporal events     |
| TEMPORAL<T>  | Value that changes over time    |
| QUANTUM<T>   | Quantum state of type T         |
| AETHEL       | Temporal energy resource        |
| CHRONON      | Basic quantum of time           |


## 5. Control Flow

### 5.1 Conditionals

```chronovyan
// If-else
IF (x > 0) {
    // Positive
} ELSE IF (x < 0) {
    // Negative
} ELSE {
    // Zero
}

// When expression (like switch)
MATCH value {
    1 => io::println("One"),
    2 | 3 | 5 | 7 => io::println("Prime"),
    _ => io::println("Other"),
}
```

### 5.2 Loops

```chronovyan
// For loop
FOR (DECLARE i = 0; i < 10; i++) {
    io::println(i);
}

// For-each
FOR (item IN collection) {
    io::println(item);
}

// While loop
WHILE (condition) {
    // ...
}

// Loop with break/continue
FOR (;;) {
    IF (should_break) BREAK;
    IF (should_skip) CONTINUE;
    // ...
}
```

### 5.3 Temporal Control

```chronovyan
// Delay execution
DELAY 1s;


// Create a branch in time
DECLARE branch = TIMELINE_BRANCH("alternate");

// Merge timelines
TIMELINE_MERGE(branch);

// Quantum conditional
QUANTUM_IF (qbit) {
    // This block exists in superposition
} ELSE {
    // As does this one
}
```

## 6. Functions

### 6.1 Function Declaration

```chronovyan
// Basic function
FUNC INT add(INT a, INT b) {
    RETURN a + b;
}

// Default parameters
FUNC STRING greet(STRING name = "World") {
    RETURN "Hello, " + name;
}

// Variadic function
FUNC INT sum(INT... numbers) {
    DECLARE total = 0;
    FOR (n IN numbers) {
        total += n;
    }
    RETURN total;
}
```

### 6.2 Higher-Order Functions

```chronovyan
// Function as parameter
FUNC INT apply(INT a, INT b, FUNC(INT,INT) op) {
    RETURN op(a, b);
}

// Lambda expression
DECLARE add = \\(INT a, INT b) -> INT { RETURN a + b; };
DECLARE result = apply(5, 3, add);
```

## 7. Error Handling

### 7.1 Result Type

```chronovyan
FUNC RESULT<INT, STRING> divide(INT a, INT b) {
    IF (b == 0) {
        RETURN ERR("Division by zero");
    }
    RETURN OK(a / b);
}

// Usage
MATCH divide(10, 2) {
    OK(result) => io::println("Result: " + result),
    ERR(msg) => io::println("Error: " + msg),
}
```

### 7.2 Panics

```chronovyan
FUNC INT divide(INT a, INT b) {
    IF (b == 0) {
        PANIC("Division by zero");
    }
    RETURN a / b;
}

// Recover from panic
TRY {
    DECLARE result = divide(10, 0);
} CATCH (e) {
    io::println("Caught: " + e.message());
}
```

## 8. Concurrency

### 8.1 Threads

```chronovyan
// Spawn a new thread
DECLARE handle = SPAWN {
    // This runs in parallel
    io::println("Hello from another thread");
};

// Wait for thread to complete
AWAIT handle;
```

### 8.2 Channels

```chronovyan
// Create a channel
DECLARE (tx, rx) = CHANNEL<INT>();

// Send data in one thread
SPAWN {
    tx.send(42);
};

// Receive in another
DECLARE value = rx.recv();
```

## 9. Memory Management

### 9.1 Ownership

```chronovyan
// Transfer ownership
DECLARE s1 = "Hello".to_string();
DECLARE s2 = s1;  // s1 is moved, not copied
// s1 is no longer valid here

// Borrowing
FUNC INT string_length(STRING &s) {
    RETURN s.length();
}

// Mutable borrow
FUNC VOID append_world(STRING &mut s) {
    s += " World";
}
```

### 9.2 Lifetimes

```chronovyan
// Explicit lifetime parameter
FUNC &STR longest<'a>(&'a STR a, &'a STR b) -> &'a STR {
    IF (a.length() > b.length()) {
        RETURN a;
    } ELSE {
        RETURN b;
    }
}
```

## 10. Standard Library

### 10.1 Core Modules

- `std.io`: Input/output operations
- `std.collections`: Data structures
- `std.time`: Time-related functions
- `std.math`: Mathematical functions
- `std.thread`: Concurrency primitives

### 10.2 Common Types

- `Option<T>`: Represents an optional value
- `Result<T,E>`: For fallible operations
- `Vec<T>`: Dynamic array
- `HashMap<K,V>`: Hash map
- `String`: UTF-8 string

## 11. Temporal Programming

Chronovyan's temporal programming model is built around first-class support for time-aware computations, quantum operations, and timeline management.

### 11.1 Core Temporal Types

| Type         | Description                                      | Example                     |
|--------------|--------------------------------------------------|-----------------------------|
| `MOMENT`     | A point in time                                  | `NOW()`                     |
| `DURATION`   | A length of time                                 | `5s`, `2h30m`               |
| `TIMELINE`   | A sequence of temporal events                   | `TIMELINE_NEW("events")`   |
| `TEMPORAL<T>`| A value that changes over time                  | `TEMPORAL<INT> counter`     |
| `QUANTUM<T>` | A quantum state that can be in superposition    | `QUANTUM<BOOL> q`          |
| `AETHEL`     | Temporal energy resource                        | `AETHEL energy = 100.0`     |
| `CHRONON`    | Basic quantum of time                           | `CHRONON t = NOW()`         |


### 11.2 Timeline Operations

Timelines are the foundation of Chronovyan's temporal model, allowing you to sequence and manage events over time.

```chronovyan
// Create a new named timeline
DECLARE timeline = TIMELINE_NEW("simulation");


// Schedule events with temporal precision
TIMELINE_ADD_EVENT(timeline, NOW() + 100ms, \() {
    io::println("100ms elapsed");
});

// Add a periodic event
TIMELINE_ADD_RECURRING(timeline, 1s, \() {
    static INT counter = 0;
    io::println("Tick ", counter++);
});

// Run the timeline asynchronously
DECLARE timeline_handle = TIMELINE_RUN_ASYNC(timeline);

// Later, pause or stop the timeline
TIMELINE_PAUSE(timeline);
// ...
TIMELINE_RESUME(timeline);
// ...
TIMELINE_STOP(timeline);
AWAIT timeline_handle;  // Wait for completion
```

### 11.3 Quantum Programming

Chronovyan provides first-class support for quantum computing operations.

```chronovyan
// Quantum teleportation example
FUNC QUANTUM<BOOL> quantum_teleport(QUANTUM<BOOL> q) {
    // Create entangled pair (Bell pair)
    DECLARE q1 = QUBIT(0);
    DECLARE q2 = QUBIT(1);
    H(q1);
    CNOT(q1, q2);
    
    // Entangle with input qubit
    CNOT(q, q1);
    H(q);
    
    // Measure and correct
    IF (MEASURE(q)) { Z(q2); }
    IF (MEASURE(q1)) { X(q2); }
    
    RETURN q2;
}

// Using quantum operations
FUNC VOID main() {
    // Create a qubit in superposition
    DECLARE q = QUBIT(0);
    H(q);
    
    // Teleport the quantum state
    DECLARE teleported = quantum_teleport(q);
    
    // Measure the result
    IF (MEASURE(teleported)) {
        io::println("Measured |1>");
    } ELSE {
        io::println("Measured |0>");
    }
}
```

### 11.4 Temporal Variables and State

Chronovyan provides special variable types for handling time-dependent state.

```chronovyan
// A static temporal variable (fixed point in time)
DECLARE CONF::STATIC program_start = NOW();

// A flux variable that changes over time
DECLARE REB::FLUX<FLOAT> temperature = 20.0;

// Monitor temperature changes
TIMELINE_ADD_RECURRING(main_timeline, 1s, \() {
    io::println("Current temperature: ", temperature, "°C");
    
    // Update temperature (simulated)
    temperature = 20.0 + 5.0 * SIN((NOW() - program_start).as_seconds());
});

// Quantum state that can be in superposition
DECLARE QUANTUM::ENTANGLED coin = QUBIT(0);
H(coin);  // Put in superposition

// Measure collapses the superposition
IF (MEASURE(coin)) {
    io::println("Heads!");
} ELSE {
    io::println("Tails!");
}
```

### 11.5 Temporal Resource Management

Chronovyan includes special mechanisms for managing temporal resources.

```chronovyan
// AETHEL represents temporal energy
DECLARE AETHEL energy = 100.0;

// Temporal functions consume AETHEL
FUNC TEMPORAL<INT> count_down(INT start) {
    // Consume temporal energy
    TEMPORAL_EXPEND(10.0);
    
    DECLARE current = start;
    RETURN TEMPORAL<INT> {
        next: \() -> OPTION<INT> {
            IF (current > 0) {
                RETURN SOME(current--);
            } ELSE {
                RETURN NONE;
            }
        }
    };
}

// Using temporal resources
FUNC VOID main() {
    DECLARE counter = count_down(5);
    WHILE (TRUE) {
        MATCH counter.next() {
            SOME(n) => io::println(n),
            NONE => BREAK,
        }
        DELAY 1s;
    }
}
```

## 12. Implementation Notes

### 12.1 Memory Model

- Stack allocation by default
- Move semantics for complex types
- Automatic reference counting for shared ownership
- No garbage collection pauses

### 12.2 Runtime

- Lightweight runtime
- Zero-cost abstractions
- Minimal runtime overhead

## 13. Appendix

### 13.1 Keywords

```
as          break       const       continue    else        enum
extern      false       fn          for         if          impl
in          let         loop        match       mod         move
mut         pub         ref         return      self       Self
static      struct      super       trait       true       type
unsafe      use         where       while       async      await
dyn         become      box         do          final      macro
override    priv        try         typeof      unsized    virtual
yield       abstract    const       macro_rules
```

### 13.2 Operators

```
+    -    *    /    %    ^    !    &    |    &&   ||   <<   >>
+=   -=   *=   /=   %=   ^=   &=   |=   <<=  >>=  =    ==   !=
>    <    >=   <=   @    .    ..   ...  ..=   ,    ;    :    ::
->   =>   #    $
```

## 14. References

1. [Chronovyan GitHub Repository](https://github.com/chronovyan/chronovyan)
2. [Temporal Programming Patterns](https://chronovyan.dev/patterns)
3. [Quantum Computing Basics](https://quantum.chronovyan.dev)

## 15. License

Chronovyan is licensed under the MIT License. See [LICENSE](https://github.com/chronovyan/chronovyan/blob/main/LICENSE) for details.
