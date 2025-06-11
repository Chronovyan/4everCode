---
title: 4ever Language Reference Supplement
description: Documentation for 4ever_language_reference_supplement.md
weight: 100
draft: true
---

# 4ever Language Reference Supplement

> *"The true depth of 4ever lies not in what it allows us to do, but in how it reshapes our understanding of what is possible."*  
> — Archivist's Note, Vault of Temporal Knowledge

## Introduction

This supplement expands upon the 4ever Language Guide series, providing additional technical details, practical examples, and reference information not covered in the main guides. While the Act II Primer and Advanced Mechanics Addendum focus on the fundamental concepts and philosophical underpinnings of 4ever, this reference addresses specific implementation details, edge cases, and best practices for effective temporal programming.

## Table of Contents

1. [Comprehensive Type System](#comprehensive-type-system)
2. [Standard Library Reference](#standard-library-reference)
3. [Error Handling Patterns](#error-handling-patterns)
4. [Best Practices and Anti-Patterns](#best-practices-and-anti-patterns)
5. [Advanced Syntax Constructs](#advanced-syntax-constructs)
6. [Memory and Resource Model](#memory-and-resource-model)
7. [Integration with External Systems](#integration-with-external-systems)

## Comprehensive Type System

4ever features a rich type system that balances static typing for stability with dynamic flexibility for handling temporal flux. This section details the complete type hierarchy and type operations available to Weavers.

### Primitive Types

| Type | Description | Example Literals | Default Value |
|------|-------------|-----------------|--------------|
| `Int` | Signed integer values | `42`, `-7`, `0` | `0` |
| `Float` | Floating-point values | `3.14`, `-0.001`, `6.022e23` | `0.0` |
| `Boolean` | Logical true/false | `TRUE`, `FALSE` | `FALSE` |
| `String` | Text data | `"Hello"`, `"Temporal flux: 0.31"` | `""` (empty string) |
| `Timestamp` | Temporal point | `Timestamp.now()`, `Timestamp.from_absolute(163402)` | `null` |
| `Void` | No value | - | - |
| `Null` | Absence of value | `null` | `null` |

### Composite Types

| Type | Description | Declaration Example | Access Pattern |
|------|-------------|---------------------|----------------|
| `Array<T>` | Ordered collection of elements of type T | `CONF numbers: Array<Int> = [1, 2, 3];` | `numbers[index]` |
| `Map<K, V>` | Key-value pairs mapping K to V | `REB config: Map<String, Float> = {"threshold": 0.5};` | `config[key]` or `config.get(key)` |
| `Tuple<T1, T2, ...>` | Fixed-size heterogeneous collection | `CONF point: Tuple<Float, Float> = (3.5, 2.0);` | `point.first`, `point.second` or `point[index]` |
| `Set<T>` | Unordered collection of unique elements | `CONF allowed_states: Set<String> = {"STABLE", "FLUX"};` | `allowed_states.contains(value)` |
| `Optional<T>` | May contain a value of type T or null | `REB maybe_reading: Optional<Float> = get_reading();` | `maybe_reading.value_or(default)` |

### User-Defined Types

#### Structures

Structures define compound data types with named fields:

```4ever
structure SensorReading {
    CONF timestamp: Timestamp;
    REB value: Float;
    CONF location: String;
    REB is_valid: Boolean = TRUE;
    
    method validate() -> Boolean {
        // Validation logic
        return value >= 0 && value <= MAX_READING;
    }
    
    static method create_from_raw(raw_data: String) -> SensorReading {
        // Parse raw data and create a reading
    }
}

// Usage
REB reading: SensorReading = SensorReading {
    timestamp: Timestamp.now(),
    value: 42.5,
    location: "Sector 7"
};

if (reading.validate()) {
    process_reading(reading);
}
```

#### Enumerations

Enumerations define a set of named constants:

```4ever
enumeration TimelineState {
    STABLE,
    FLUCTUATING,
    DIVERGING,
    CONVERGING,
    COLLAPSING
}

// Usage
REB current_state: TimelineState = TimelineState.STABLE;

if (current_state == TimelineState.FLUCTUATING) {
    apply_stabilization();
}
```

#### Type Aliases

Type aliases create alternative names for existing types:

```4ever
typedef ReadingMap = Map<Timestamp, SensorReading>;
typedef StabilityValue = Float;

// Usage
CONF readings: ReadingMap = collect_readings();
REB current_stability: StabilityValue = calculate_stability(readings);
```

### Type Conversions

#### Implicit Conversions

4ever allows certain implicit conversions that preserve semantic meaning:

- `Int` to `Float`
- `T` to `Optional<T>`
- Array literals to `Array<T>`
- Tuple literals to `Tuple<T1, T2, ...>`

#### Explicit Conversions

For conversions that might lose precision or change semantics, explicit conversion is required:

```4ever
CONF float_value: Float = 3.14;
CONF int_value: Int = Int.from(float_value);  // Explicit conversion

CONF timestamp: Timestamp = Timestamp.now();
CONF string_timestamp: String = String.from(timestamp);  // Conversion to string representation
```

#### Safe Type Checking and Casting

4ever provides safe type checking and casting operations:

```4ever
if (value is SensorReading) {
    // value is known to be a SensorReading in this scope
    process_sensor_reading(value);
}

// Conditional casting with fallback
REB reading = value as? SensorReading ?? default_reading;

// Safe cast with runtime check
ATTEMPT_WEAVE {
    REB reading = value as SensorReading;
    process_reading(reading);
} ON_DISSONANCE(TypeCastDissonance e) {
    log_error("Invalid type cast: " + e.message);
}
```

## Standard Library Reference

The 4ever Standard Library provides essential modules for temporal programming. This section outlines the core modules and their key functions.

### Core Module (`chrono.core`)

The foundational module providing basic temporal utilities:

```4ever
// Time measurement
Timestamp now() -> Timestamp
Duration elapsed_since(Timestamp start) -> Duration
Boolean is_timeline_stable() -> Boolean

// Resource management
Int get_aethel_level() -> Int
Int get_chronons_level() -> Int
void allocate_aethel(Int amount) -> void
void allocate_chronons(Int amount) -> void
void release_aethel(Int amount) -> void
void release_chronons(Int amount) -> void

// System
void print(String message) -> void
String input(String prompt) -> String
void sleep(Duration duration) -> void
```

### Temporal Analysis (`chrono.analysis`)

Functions for analyzing temporal patterns and anomalies:

```4ever
Float measure_timeline_stability() -> Float
Array<Anomaly> detect_anomalies(Timestamp start, Timestamp end) -> Array<Anomaly>
TimelineSnapshot capture_timeline_state() -> TimelineSnapshot
Boolean compare_timeline_states(TimelineSnapshot a, TimelineSnapshot b) -> Boolean
```

### Pattern Management (`chrono.patterns`)

Tools for creating and managing temporal patterns:

```4ever
Pattern create_pattern(String name, Function callback) -> Pattern
void register_pattern(Pattern pattern) -> void
Pattern find_pattern(String name) -> Pattern
void execute_pattern(Pattern pattern, Map<String, Any> parameters) -> void
```

### Error Handling (`chrono.dissonance`)

Functions for working with temporal dissonance (errors):

```4ever
void raise_dissonance(String message) -> void
Dissonance capture_dissonance() -> Dissonance
Boolean is_dissonant_state() -> Boolean
void stabilize_timeline() -> void
```

### Collections (`chrono.collections`)

Extended collection manipulation functions:

```4ever
Array<B> map<A, B>(Array<A> input, Function<A, B> transform) -> Array<B>
Array<A> filter<A>(Array<A> input, Function<A, Boolean> predicate) -> Array<A>
B reduce<A, B>(Array<A> input, B initial, Function<B, A, B> accumulator) -> B
Array<A> sort<A>(Array<A> input, Function<A, A, Int> comparator) -> Array<A>
```

### File Operations (`chrono.io`)

Functions for reading and writing data:

```4ever
String read_file(String path) -> String
Array<String> read_lines(String path) -> Array<String>
void write_file(String path, String content) -> void
Boolean file_exists(String path) -> Boolean
```

## Error Handling Patterns

4ever's error handling system is built around the concept of "temporal dissonance"—disturbances in the timeline caused by exceptional conditions. This section demonstrates common patterns for robust error handling.

### Basic Dissonance Handling

The `ATTEMPT_WEAVE`/`ON_DISSONANCE` construct provides try/catch functionality:

```4ever
ATTEMPT_WEAVE {
    // Code that might cause temporal dissonance
    REB reading: Float = parse_sensor_data(raw_input);
    process_reading(reading);
} ON_DISSONANCE(ParseDissonance e) {
    // Handle specific parsing errors
    log_error("Parse error: " + e.message);
    request_new_data();
} ON_DISSONANCE(Any e) {
    // Catch-all handler for other dissonances
    log_error("Unexpected error: " + e.message);
    initiate_safe_mode();
} TIMELINE_CLEANUP {
    // Always executed, similar to 'finally'
    release_resources();
}
```

### Propagating Dissonance

For functions that should pass dissonance to their callers:

```4ever
define_pattern read_critical_data() -> SensorData +DISSONANT {
    if (!sensor_available()) {
        RAISE_DISSONANCE(SensorDissonance, "Sensor unavailable");
    }
    
    // Continue with normal processing
    return retrieve_sensor_data();
}

// Caller must handle the potential dissonance
ATTEMPT_WEAVE {
    REB data = read_critical_data();
    process_data(data);
} ON_DISSONANCE(SensorDissonance e) {
    activate_backup_sensors();
}
```

### Recovery Strategies

Different approaches for recovering from temporal dissonance:

```4ever
// Retry pattern
define_pattern with_retry(operation: Function, max_attempts: Int) -> Any {
    REB attempts: Int = 0;
    
    WHILE_EVENT (attempts < max_attempts) {
        ATTEMPT_WEAVE {
            return operation();
        } ON_DISSONANCE(Any e) {
            attempts++;
            
            if (attempts >= max_attempts) {
                RAISE_DISSONANCE(MaxRetriesExceededDissonance, 
                                "Failed after " + max_attempts + " attempts: " + e.message);
            }
            
            print("Attempt " + attempts + " failed, retrying...");
            sleep(Duration.seconds(1 * attempts));  // Exponential backoff
        }
    }
}

// Circuit breaker pattern
structure CircuitBreaker {
    CONF name: String;
    REB state: CircuitState = CircuitState.CLOSED;
    REB failure_count: Int = 0;
    CONF failure_threshold: Int;
    CONF reset_timeout: Duration;
    REB last_failure_time: Timestamp = null;
    
    method execute(operation: Function) -> Any {
        if (state == CircuitState.OPEN) {
            // Check if we should try resetting the circuit
            if (Timestamp.now() - last_failure_time > reset_timeout) {
                state = CircuitState.HALF_OPEN;
                print("Circuit " + name + " entering half-open state");
            } else {
                RAISE_DISSONANCE(CircuitOpenDissonance, "Circuit " + name + " is open");
            }
        }
        
        ATTEMPT_WEAVE {
            REB result = operation();
            
            // Success - reset circuit if it was half-open
            if (state == CircuitState.HALF_OPEN) {
                state = CircuitState.CLOSED;
                failure_count = 0;
                print("Circuit " + name + " reset to closed state");
            }
            
            return result;
        } ON_DISSONANCE(Any e) {
            failure_count++;
            last_failure_time = Timestamp.now();
            
            if (state == CircuitState.HALF_OPEN || failure_count >= failure_threshold) {
                state = CircuitState.OPEN;
                print("Circuit " + name + " tripped open");
            }
            
            RAISE_DISSONANCE(e);  // Re-raise the original dissonance
        }
    }
}

enumeration CircuitState {
    CLOSED,    // Normal operation
    OPEN,      // Failing, rejecting requests
    HALF_OPEN  // Testing if system has recovered
}
```

## Best Practices and Anti-Patterns

This section provides guidance on writing effective, maintainable, and efficient 4ever code.

### Resource Management Best Practices

- **Explicit Allocation and Release**: Always explicitly allocate and release temporal resources
- **Resource Budgeting**: Set clear resource limits for each significant operation
- **Monitoring**: Continuously monitor resource levels during long-running operations
- **Efficiency**: Use the minimum resources necessary for stability

```4ever
// Good practice: Explicit resource management
CONF required_aethel: Int = calculate_required_aethel(operation_size);
CONF required_chronons: Int = calculate_required_chronons(operation_complexity);

if (get_aethel_level() < required_aethel || get_chronons_level() < required_chronons) {
    RAISE_DISSONANCE(InsufficientResourcesDissonance, "Insufficient resources for operation");
}

allocate_aethel(required_aethel);
allocate_chronons(required_chronons);

ATTEMPT_WEAVE {
    // Perform operation
} TIMELINE_CLEANUP {
    // Always release resources, even on dissonance
    release_aethel(required_aethel);
    release_chronons(required_chronons);
}
```

### Code Organization

- **Module Structure**: Organize code into logical modules focused on specific responsibilities
- **Pattern Naming**: Use descriptive names for patterns that indicate their purpose
- **Constants**: Define common constants at the program level for consistency
- **Documentation**: Document the purpose, parameters, and dissonance conditions for all patterns

```4ever
// Good practice: Well-organized module with clear responsibilities
temporal_program {
    name: "SensorMonitor";
    // ... configuration ...
    
    // Constants section
    variables {
        DECLARE CONF MAX_SENSOR_VALUE: Float = 100.0 ::STATIC;
        DECLARE CONF CRITICAL_THRESHOLD: Float = 85.0 ::STATIC;
        DECLARE CONF WARNING_THRESHOLD: Float = 70.0 ::STATIC;
    }
    
    // Initialization
    initialization_phase {
        // ... setup code ...
    }
    
    // Main logic
    execution_weave {
        monitor_sensors();
    }
    
    // Helper patterns organized by responsibility
    
    // Data acquisition
    define_pattern read_sensor(sensor_id: String) -> Float {
        // ... implementation ...
    }
    
    // Analysis
    define_pattern analyze_readings(readings: Array<Float>) -> AnalysisResult {
        // ... implementation ...
    }
    
    // Response
    define_pattern respond_to_critical_condition(sensor_id: String, value: Float) {
        // ... implementation ...
    }
}
```

### Anti-Patterns to Avoid

- **Resource Leaks**: Failing to release allocated resources
- **Temporal Deadlocks**: Creating circular dependencies between timeline branches
- **Excessive Paradox**: Generating high levels of temporal paradox through conflicting operations
- **Magic Numbers**: Using unexplained numeric constants throughout code
- **Global State Abuse**: Overreliance on global variables instead of proper parameter passing

```4ever
// Anti-pattern: Resource leak
allocate_aethel(100);
// Code that might raise dissonance without releasing resources
process_data();  // If this raises dissonance, aethel is never released

// Better approach:
ATTEMPT_WEAVE {
    allocate_aethel(100);
    process_data();
} TIMELINE_CLEANUP {
    release_aethel(100);  // Always executed
}
```

## Advanced Syntax Constructs

This section covers specialized syntax constructs for advanced 4ever programming.

### Temporal Assertions

Assertions for timeline integrity validation:

```4ever
TEMPORAL_ASSERT(condition, "Error message");

// Example: Assert that timeline stability is above a threshold
TEMPORAL_ASSERT(measure_timeline_stability() > 0.7, 
               "Timeline stability too low for safe operation");
```

### Lambda Expressions

Anonymous functions for in-place operations:

```4ever
// Filter readings above threshold using a lambda
REB high_readings = readings.filter(reading => reading.value > THRESHOLD);

// Sort readings by timestamp
readings.sort((a, b) => a.timestamp.compare_to(b.timestamp));

// Multi-line lambda with explicit return
REB processed_data = raw_data.map(item => {
    REB processed = preprocess(item);
    if (is_valid(processed)) {
        return transform(processed);
    } else {
        return null;
    }
});
```

### Temporal Annotations

Metadata tags that affect pattern execution:

```4ever
@Stability(high)
@ResourceLimit(aethel = 50, chronons = 200)
@Documentation("Calculates the stability index for a temporal region")
define_pattern calculate_stability_index(region_id: String) -> Float {
    // Implementation...
}

@Deprecated("Use calculate_stability_index instead")
define_pattern old_stability_calculation(region_id: String) -> Float {
    return calculate_stability_index(region_id);
}
```

### Pattern Overloading

Define multiple pattern variants with different parameter signatures:

```4ever
// Overloaded pattern with different parameter types
define_pattern stabilize(region_id: String) -> void {
    // Implementation for region-based stabilization
}

define_pattern stabilize(coordinates: Tuple<Float, Float, Float>) -> void {
    // Implementation for coordinate-based stabilization
}

define_pattern stabilize(reading: SensorReading) -> void {
    // Implementation for reading-based stabilization
}
```

## Memory and Resource Model

This section details the memory management and resource allocation model of 4ever.

### Memory Management

4ever uses a hybrid memory management approach:

- **Automatic Reference Counting**: Most objects are managed through reference counting
- **Manual Resource Management**: Temporal resources (Aethel and Chronons) require explicit allocation and release
- **Temporal Scope**: Variables are bound to their temporal scope and automatically released when the scope ends

```4ever
define_pattern demonstrate_memory_management() {
    // Local variable with automatic lifetime
    {
        REB local_data = create_large_dataset();
        process_data(local_data);
        // local_data is automatically cleaned up when it goes out of scope
    }
    
    // Manual resource management
    allocate_aethel(50);
    
    ATTEMPT_WEAVE {
        // Use aethel for operations
    } TIMELINE_CLEANUP {
        release_aethel(50);
    }
}
```

### Reference Types vs. Value Types

Understanding the difference between reference and value types:

- **Value Types**: Primitive types (Int, Float, Boolean) are copied when assigned
- **Reference Types**: Complex objects (structures, arrays) are passed by reference

```4ever
// Value type behavior
CONF a: Int = 5;
CONF b: Int = a;  // b gets a copy of a's value
b = 10;           // Changing b doesn't affect a

// Reference type behavior
CONF array1: Array<Int> = [1, 2, 3];
CONF array2: Array<Int> = array1;  // array2 references the same array as array1
array2[0] = 99;                   // Changes are visible through both variables
```

### Resource Lifecycle

The complete lifecycle of temporal resources:

1. **Estimation**: Calculate required resources
2. **Verification**: Check availability
3. **Allocation**: Reserve resources for use
4. **Consumption**: Use resources during operations
5. **Release**: Return unused resources to the system

```4ever
// Complete resource lifecycle example
define_pattern perform_temporal_operation(complexity: Int) -> Result {
    // 1. Estimation
    CONF estimated_aethel: Int = 10 * complexity;
    CONF estimated_chronons: Int = 5 * complexity;
    
    // 2. Verification
    if (get_aethel_level() < estimated_aethel || get_chronons_level() < estimated_chronons) {
        RAISE_DISSONANCE(InsufficientResourcesDissonance, 
                        "Insufficient resources for operation with complexity " + complexity);
    }
    
    // 3. Allocation
    allocate_aethel(estimated_aethel);
    allocate_chronons(estimated_chronons);
    
    ATTEMPT_WEAVE {
        // 4. Consumption
        REB result: Result = null;
        
        FOR_CHRONON (i: 0..complexity) {
            // Perform work
            result = process_step(i, result);
            
            // Explicit consumption based on actual work done
            consume_aethel(5 + i);  // Progressive consumption
        }
        
        return result;
    } ON_DISSONANCE(Any e) {
        log_error("Operation failed: " + e.message);
        RAISE_DISSONANCE(e);  // Re-raise
    } TIMELINE_CLEANUP {
        // 5. Release
        CONF remaining_aethel: Int = get_allocated_aethel();
        CONF remaining_chronons: Int = get_allocated_chronons();
        
        release_aethel(remaining_aethel);
        release_chronons(remaining_chronons);
        
        log_info("Released remaining resources: " + remaining_aethel + " aethel, " + 
                remaining_chronons + " chronons");
    }
}
```

## Integration with External Systems

Guidelines for integrating 4ever programs with external systems and data sources.

### Temporal API Design

Principles for designing APIs that interface with 4ever:

```4ever
// External system integration pattern
define_pattern fetch_external_data(endpoint: String) -> ExternalData +DISSONANT {
    // Allocate resources for external communication
    allocate_aethel(20);
    
    ATTEMPT_WEAVE {
        // Set up timeout and connection parameters
        CONF timeout: Duration = Duration.seconds(5);
        CONF retry_count: Int = 3;
        
        // Make external call with retry logic
        REB response = with_retry(() => make_http_request(endpoint), retry_count);
        
        // Parse and validate response
        REB data = parse_response(response);
        validate_external_data(data);
        
        return data;
    } ON_DISSONANCE(ConnectionDissonance e) {
        log_error("Connection error: " + e.message);
        RAISE_DISSONANCE(ExternalSystemDissonance, "Failed to connect to " + endpoint);
    } ON_DISSONANCE(ParseDissonance e) {
        log_error("Parse error: " + e.message);
        RAISE_DISSONANCE(ExternalDataDissonance, "Failed to parse response from " + endpoint);
    } TIMELINE_CLEANUP {
        release_aethel(20);
    }
}
```

### Data Conversion

Converting between 4ever types and external formats:

```4ever
// Convert external JSON to 4ever types
define_pattern json_to_sensor_reading(json: String) -> SensorReading +DISSONANT {
    REB parsed = parse_json(json);
    
    return SensorReading {
        timestamp: Timestamp.from_string(parsed.get("timestamp")),
        value: Float.from(parsed.get("value")),
        location: String.from(parsed.get("location")),
        is_valid: parsed.get("valid") == "true"
    };
}

// Convert 4ever types to external formats
define_pattern sensor_reading_to_json(reading: SensorReading) -> String {
    REB json_map: Map<String, String> = {
        "timestamp": reading.timestamp.to_string(),
        "value": reading.value.to_string(),
        "location": reading.location,
        "valid": reading.is_valid.to_string()
    };
    
    return generate_json(json_map);
}
```

### Configuration Management

Best practices for managing configuration:

```4ever
// Load configuration from external file
define_pattern load_configuration(config_path: String) -> Configuration +DISSONANT {
    if (!file_exists(config_path)) {
        RAISE_DISSONANCE(ConfigurationDissonance, "Configuration file not found: " + config_path);
    }
    
    REB config_text = read_file(config_path);
    REB config = parse_configuration(config_text);
    
    // Validate required configuration values
    TEMPORAL_ASSERT(config.has_key("sensor_endpoint"), "Missing required config: sensor_endpoint");
    TEMPORAL_ASSERT(config.has_key("update_interval"), "Missing required config: update_interval");
    
    return config;
}

// Apply configuration with validation
define_pattern apply_configuration(config: Configuration) {
    // Apply system-wide settings
    set_update_interval(Duration.from_string(config.get("update_interval")));
    set_sensor_endpoint(config.get("sensor_endpoint"));
    
    // Apply optional settings with defaults
    set_retry_count(Int.from(config.get("retry_count", "3")));
    set_timeout(Duration.from_string(config.get("timeout", "5s")));
    
    // Log applied configuration
    log_info("Configuration applied successfully");
}
```