---
title: Language Specification
description: Documentation for language specification
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Chronovyan Language Specification

## Introduction

Welcome, Weaver, to **Chronovyan** (also known as **Chronoscript**), a temporal programming language born on Chronos Prime, where time is both canvas and crucible. Crafted for Seekers, Anchors, and Rebel Weavers, Chronovyan lets you weave timelines, harvest `Aethel`, and defy paradoxes. This specification outlines the syntax, semantics, and runtime mechanics, blending code with the lore of a universe where every tick risks a Great Unraveling.

**Purpose**: Chronovyan enables developers to manipulate temporal states, manage resources (`Aethel`, `Chronon`), and compose complex workflows (`WEAVE_PATTERN`) with narrative-driven mechanics inspired by RPGs. It's ideal for crafting simulations, games, or systems where time is a core mechanic.

**Lore Tidbit**: Legend speaks of the First Weaver, who wove the Prime Thread from raw `Chronon`. Her code still echoes in the `ANTECEDENCE` block, a sacred ritual for all Weavers.

## File Format and Structure

### .cvy File Format

The `.cvy` file format is the standard file extension for Chronovyan language source code.

#### File Characteristics

| Characteristic | Description |
|----------------|-------------|
| Extension      | `.cvy` |
| Format         | Plain text (UTF-8 encoded) |
| Line Endings   | Platform-independent (LF or CRLF) |
| Comment Style  | `// Single line comment` or `/* Multi-line comment */` |

#### Basic File Structure

A typical `.cvy` file follows this general structure:

```chronovyan
// File header comment describing purpose
// Author information, version, etc.

// Import statements (if applicable)
IMPORT "standard_library.cvy";

// Constants and global variables
CONF INT MAX_ITERATIONS = 100;
REB FLOAT INSTABILITY_FACTOR = 0.5;

// Function declarations
FUNC INT calculate_temporal_variance(INT base_time, FLOAT modifier) {
    // Function body
    RETURN base_time * modifier;
}

// Main program logic
FUNC VOID main() {
    // Program statements
    PRINT("Executing temporal sequence");
    
    // Temporal operations
    FOR_CHRONON(CONF INT i = 0; i < MAX_ITERATIONS; i++) {
        // Loop body
    }
}

// Call main function
main();
```

### Program Structure

Every Chronovyan program follows a structured pattern that establishes its temporal context and resource requirements:

```chronovyan
// Basic program structure
temporal_program {
    name: "Program Name";
    type: standard;  // standard, foundational, advanced, quantum
    resources: {
        aethel: 10;  // Initial Aethel allocation
        chronon: 5;  // Initial Chronon allocation
    }
    timeline: {
        stability: high;  // high, medium, low
        flow: forward;    // forward, bidirectional, quantum
    }
    
    // Program body follows...
}
```

Alternatively, using the three-block structure:

```chronovyan
ANTECEDENCE {
    // Setup, declarations, and pattern definitions
}

CONCURRENCY {
    // Execution, streams, and pattern application
}

CONSEQUENCE {
    // Error handling, results, and cleanup
}
```

## Syntax (EBNF)

Chronovyan's syntax is precise, reflecting the discipline of Anchors. Below is a simplified EBNF grammar for core constructs:

```
program = "ANTECEDENCE", "{", { antecedent_stmt }, "}",
          "CONCURRENCY", "{", { concurrency_stmt }, "}",
          "CONSEQUENCE", "{", { consequence_stmt }, "}";

antecedent_stmt = declare_stmt | define_pattern_stmt | compose_pattern_stmt;
declare_stmt = "DECLARE", ("CONF" | "REB"), "::", flag, identifier, ":", data_type, ["=", expr], ";";
define_pattern_stmt = "DECLARE", "CONF", "::STATIC", identifier, ":", "WEAVE_PATTERN", "=", "define_pattern", "(", string, ",", "[", stmt, { ",", stmt }, "]", ")", ";";
compose_pattern_stmt = "DECLARE", "CONF", "::STATIC", identifier, ":", "WEAVE_PATTERN", "=", "compose_pattern", "(", string, ",", "[", identifier, { ",", identifier }, "]", ",", strategy, [",", "[", condition, { ",", condition }, "]"], ")", ";";

concurrency_stmt = stream_stmt | apply_pattern_stmt | validate_stmt | paradox_check_stmt;
stream_stmt = "||", identifier, ":", stmt;
apply_pattern_stmt = identifier, "=", "apply_pattern", "(", identifier, ",", identifier, ")", ";";
validate_stmt = identifier, "=", "validate_pattern", "(", identifier, ")", ";";
paradox_check_stmt = "?!", "paradox_check", "(", "[", identifier, { ",", identifier }, "]", ")", ";";

consequence_stmt = catch_temporal_stmt | dejavu_stmt | loot_table_stmt | if_stmt;
catch_temporal_stmt = "CATCH_TEMPORAL", error_type, "{", { stmt }, "}";
dejavu_stmt = "DEJAVU", "(", identifier, ",", identifier, ",", string, ")", ";";
loot_table_stmt = "LOOT_TABLE", "{", number, ":", "LOOT", "(", string, ")", ";", "}";

data_type = "TIMESTAMP" | "CHRONON_STREAM" | "AETHEL_RESERVE" | "TEMPORAL_SIGNATURE" | "WEAVE_PATTERN" | "CONF_ARRAY" | "REB_STREAM";
flag = "STATIC" | "FLUX" | "ANCHOR" | "SOURCE_INFUSED";
strategy = "SEQUENTIAL" | "PARALLEL" | "CONDITIONAL";
condition = temporal_metric | resource_threshold | pattern_outcome | boolean_expr;
error_type = "ParadoxOverflowError" | "AethelDepletionError" | "TimelineCorruptionAlert" | "NoConditionMet";
```

**Lore Tidbit**: The `?!` operator for `paradox_check` is etched in Weaver glyphs, a warning to tread lightly lest the timeline frays.

## Variable Declarations

### Conformist Variables (CONF)

Conformist variables provide stability and predictability in temporal programs:

```chronovyan
// CONF variable declaration
CONF var identifier: Type = initial_value;

// Examples
CONF INT stability_factor = 42;
CONF STRING message = "Temporal anchor established";
CONF var timestamp: Temporal = current_chronon();
CONF var anchor_point: Vector4D = establish_coordinates();
```

### Rebel Variables (REB)

Rebel variables allow for flexibility and adaptation across timelines:

```chronovyan
// REB variable declaration
REB var identifier: Type = initial_value;

// Examples
REB INT flux_factor = 17;
REB STRING message = "Temporal flux detected";
REB var state: Quantum = initialize_superposition();
REB var flux_point: Vector4D = identify_weak_point();
```

### Variable Flags

Variable flags modify the behavior of variables in relation to the temporal fabric:

```chronovyan
// Variables with flags
CONF::STATIC INT anchor = 42;
REB::VOLATILE STRING shifter = "unstable";
CONF::ANCHOR FLOAT anchor_point = 3.14159;
REB::WEAVER BOOL is_divergent = true;

// Multiple flags
CONF::STATIC::ANCHOR Timeline foundation = establish_baseline();
REB::VOLATILE::WEAVER Quantum manipulator = initialize_quantum_state();
```

## Core Constructs

Chronovyan revolves around `WEAVE_PATTERN`, a structured sequence of temporal operations, and composition strategies to craft complex time-threads.

### WEAVE_PATTERN

A `WEAVE_PATTERN` is an immutable (`CONF::STATIC`) blueprint of statements (e.g., `INFUSE`, `REWIND_TO`, `GUARD`), akin to a spell or function. It encapsulates:

- **Header**: Metadata (`Aethel_Cost`, `Chronon_Usage`, `Instability_Index`, `Temporal_Scope`).
- **Body**: Ordered statements with dependencies and temporal offsets.
- **Signature**: A `TEMPORAL_SIGNATURE` for integrity.

**Example**:
```chronovyan
DECLARE CONF::STATIC Exploration : WEAVE_PATTERN = define_pattern("Explore", [
    DECLARE REB::FLUX Scout : TEMPORAL_SIGNATURE = deepen_exploration("Temporal_Rift")
]);
```

### Composition Strategies

`WEAVE_PATTERN`s are combined via `COMPOSE_PATTERN` with three strategies:

- **SEQUENTIAL**: Executes patterns in order, summing `Instability_Index`.  
- **PARALLEL**: Runs patterns concurrently, using `max(Instability_Index)`.  
- **CONDITIONAL**: Selects patterns based on conditions (e.g., `PARADOX_LEVEL < 0.3`), using weighted `Instability_Index`.

**Example**:
```chronovyan
DECLARE CONF::STATIC Master_Weave : WEAVE_PATTERN = compose_pattern("Master", 
                                                  [Stabilization, Exploration], 
                                                  "PARALLEL");
```

**Lore Tidbit**: `PARALLEL` weaves are called "Threadstorms" by Seekers, a daring act that tempts the Void's wrath.

## Control Structures

### Standard Control Flow

```chronovyan
// Conditional execution
IF (condition) {
    // Execute if condition is true
} ELSE IF (another_condition) {
    // Execute if another_condition is true
} ELSE {
    // Execute if no conditions are true
}

// Switch statement
SWITCH (variable) {
    CASE value1:
        // Code for value1
        BREAK;
    CASE value2:
        // Code for value2
        BREAK;
    DEFAULT:
        // Default code
        BREAK;
}
```

### Loops

```chronovyan
// Standard loop with chronon counter
FOR_CHRONON (i = 0; i < 10; i++) {
    // Execute 10 times
}

// Chronon-based loop
FOR_CHRONON (chronon IN timeline) {
    // Process each chronon in the timeline
}

// Event-based loop
WHILE_EVENT (condition) {
    // Execute while condition is true
}

// Rebel loops
REWIND_FLOW (i = 10; i > 0; i--) {
    // Execute in reverse temporal order
}

// Quantum loop
QUANTUM_LOOP (states) {
    // Execute across all quantum states simultaneously
}
```

## Temporal Operations

### Timeline Manipulation

```chronovyan
// Create a new timeline branch
BRANCH_TIMELINE ("branch_id") {
    // Operations in new branch
}

// Merge timelines
MERGE_TIMELINES (source_id, target_id);

// Creating time points
TIME_POINT save_point = CREATE_TIME_POINT("checkpoint");

// Rewinding flow
REWIND_FLOW(save_point);
```

## Functions

```chronovyan
// Simple function
FUNC VOID greet(STRING name) {
    PRINT("Hello, " + name + "!");
}

// Function with return value
FUNC INT calculate_paradox(INT actions, FLOAT intensity) {
    RETURN actions * intensity;
}
```

## Operators

### Arithmetic Operators

```chronovyan
// Standard arithmetic
x = a + b;  // Addition
x = a - b;  // Subtraction
x = a * b;  // Multiplication
x = a / b;  // Division
x = a % b;  // Modulo

// Temporal arithmetic
t = t + 5.chronons;  // Advance 5 chronons
t = t - 3.aeons;     // Rewind 3 aeons
```

### Comparison Operators

```chronovyan
x == y;  // Equal
x != y;  // Not equal
x < y;   // Less than
x > y;   // Greater than
x <= y;  // Less than or equal
x >= y;  // Greater than or equal

// Temporal comparisons
timeline_a ~= timeline_b;  // Timeline similarity
point_a >< point_b;        // Temporal intersection
```

### Logical Operators

```chronovyan
a && b;  // Logical AND
a || b;  // Logical OR
!a;      // Logical NOT

// Temporal logic
timeline_a && timeline_b;  // Timeline conjunction
timeline_a || timeline_b;  // Timeline disjunction
!timeline_a;               // Timeline negation
```

## Runtime Mechanics

The Chronovyan runtime ensures temporal integrity through paradox detection, `PARADOX_LEVEL` management, and sandboxing.

### PARADOX_LEVEL

A global metric (0.0–1.0) tracking timeline instability:

- **Initialization**: Starts at 0.1, adjusted by streams (+0.05 each) or low `AETHEL_RESERVE` (+0.1).
- **Accumulation**: Adds `Instability_Index` (strategy-specific) plus conflict penalties:
  - Resource Contention: 0.03–0.12.
  - Temporal Interference: 0.07–0.15.
  - State Divergence: 0.1–0.2.
- **Decay**: -0.01 per 5 stable ticks; `INFUSE` reduces by `Aethel_Spent * 0.001`.
- **Thresholds**: 0.8+ triggers `ParadoxOverflowError`.

### Paradox Detection

Detects conflicts via `paradox_check`:

- **Sequential**: Resource/anchor inconsistencies.
- **Parallel**: Concurrent state divergence.
- **Conditional**: Condition-driven conflicts.
- Cross-strategy: Nested conflicts (e.g., `PARALLEL` within `SEQUENTIAL`).

### Sandboxing

Nested patterns run in sandboxes to limit paradox "blast radius":

- **Low-Risk** (`Instability_Index < 0.3`): Full resources, contained failures.
- **Medium-Risk** (0.3–0.6): 50% resources, escalates if unhandled.
- **High-Risk** (> 0.6): 25% resources, always escalates.

**Example**:
```chronovyan
CATCH_TEMPORAL ParadoxOverflowError {
    REWIND_TO @Origin;
    INFUSE(Stability_Field, LOOT("Chronovyan_Tapestry"), "Restabilize");
}
```

**Lore Tidbit**: Sandboxing is an Anchor's "Temporal Ward," a shimmering barrier etched in `Aethel` to shield reality.

## Standard Library Overview

Key functions include:

- `define_pattern(name: STRING, operations: STMT[]): WEAVE_PATTERN`: Creates a pattern.
- `apply_pattern(pattern: WEAVE_PATTERN, context: WEAVE_PATTERN): BOOLEAN`: Executes a pattern.
- `validate_pattern(pattern: WEAVE_PATTERN): PARADOX_LEVEL`: Checks for risks.
- `infuse_pattern(target: WEAVE_PATTERN, source: CHRONON_STREAM, intent: STRING): WEAVE_PATTERN`: Enhances a pattern.
- `stabilize_timeline(reserve: AETHEL_RESERVE, amount: INT): FLOAT`: Reduces `PARADOX_LEVEL`.

## Best Practices

1. **File Naming** - Use lowercase names with underscores for clarity (`temporal_simulation.cvy`)
2. **Comments** - Include a header comment with description and author information
3. **Structure** - Organize code in a logical order: imports, constants, functions, main logic
4. **Indentation** - Use consistent indentation (typically 4 spaces or 1 tab)
5. **Resource Usage** - Include resource consumption indicators for computationally expensive operations

## Complete Example Program

A full weave combining strategies, paradox handling, and loot:

```chronovyan
ANTECEDENCE {
    DECLARE CONF::STATIC Stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
        INFUSE(Stability_Field, Time_Crystals, "Restabilize"),
        GUARD(Stability_Field > 0.6, "Low stability")
    ]);
    DECLARE CONF::STATIC Exploration : WEAVE_PATTERN = define_pattern("Explore", [
        DECLARE REB::FLUX Scout : TEMPORAL_SIGNATURE = deepen_exploration("Temporal_Rift")
    ]);
    DECLARE CONF::STATIC Master_Weave : WEAVE_PATTERN = compose_pattern("Master", 
                                                      [Stabilization, Exploration], 
                                                      "PARALLEL");
}

CONCURRENCY {
    || Stream_1: DECLARE REB::FLUX Energy_Harvest : AETHEL_RESERVE = initiate_harvest(Time_Crystals);
    Risk = validate_pattern(Master_Weave);
    Success = apply_pattern(Master_Weave, System_Stabilize);
}

CONSEQUENCE {
    ?! paradox_check([Stability_Field, Scout, Energy_Harvest]);
    CATCH_TEMPORAL ParadoxOverflowError {
        REWIND_TO @Origin;
        INFUSE(Stability_Field, LOOT("Chronovyan_Tapestry"), "Restabilize");
    }
    DEJAVU(Master_Weave, PARADOX_LEVEL, "Master Audit");
    IF Success {
        LOOT_TABLE { 0.95: LOOT("Legendary_Thread"); }
    }
}
```

**Lore Tidbit**: The `DEJAVU` command channels the Weaver's Sight, replaying a timeline's echoes to uncover hidden flaws. 