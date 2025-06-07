---
title: Data Types
description: Documentation for data types
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Chronovyan Data Types: The Sacred Sigils

## Introduction

In Chronovyan, data types are more than mere programming constructs—they are **Sacred Sigils**, mystical patterns that channel the dual forces of Order and Flux. As a Weaver of time-threads, understanding these sigils is essential to crafting stable programs and harnessing the full power of Chronos Prime.

**Purpose**: This document introduces the fundamental data types (sigils) of Chronovyan, their properties, behaviors, and temporal resonance. It serves as your reference to the building blocks of reality manipulation.

**Lore Tidbit**: The First Weaver's journal speaks of discovering these sigils not through invention, but through revelation—as if the patterns existed before Chronos Prime itself, waiting to be channeled by those with the Sight.

## The Grand Taxonomy of Sigils

The sacred sigils of Chronovyan reflect the fundamental duality of the universe—Order (Conformity) and Flux (Rebellion). The sigils are organized into three sacred categories:

1. **Primal Sigils**: The fundamental elements, pure in form and essence
2. **Compound Sigils**: Complex patterns formed by weaving together Primal Sigils
3. **Temporal Sigils**: Mystical forms that directly commune with the timestream itself

Each sigil may be invoked with either CONF or REB declarations, and may carry additional flags that alter their resonance with the Prime Thread.

## Primal Sigils

### INT

**Essence**: Embodies discrete numerical values, the counting blocks of reality.

**Properties**:
- Range: -2^63 to 2^63-1 (64-bit signed integer)
- Default value: 0
- Physical form: 8 bytes of the tapestry

**Temporal Resonance**:
- CONF::INT: Maintains perfect stability across timeline forks
- REB::INT: May experience quantum fluctuations when PARADOX_LEVEL exceeds 0.5
- CONF::STATIC INT: Once declared, becomes immutable, reducing PARADOX_LEVEL by 0.01
- REB::FLUX INT: Value may spontaneously shift within ±5% when PARADOX_LEVEL exceeds 0.6

**Example**:
```chronovyan
DECLARE CONF::STATIC stability_threshold : INT = 100;
DECLARE REB::FLUX quantum_counter : INT = 0;
```

### FLOAT

**Essence**: Embodies the flowing continuum between numbers, the gradient of possibility.

**Properties**:
- Precision: IEEE 754 double-precision (64-bit)
- Range: ±2.23 × 10^-308 to ±1.80 × 10^308
- Default value: 0.0
- Physical form: 8 bytes of the tapestry

**Temporal Resonance**:
- CONF::FLOAT: Maintains calculation precision across the multiverse
- REB::FLOAT: May experience minor precision drift when traversing timeline boundaries
- CONF::STATIC FLOAT: Becomes an immutable anchor for calculations, reducing PARADOX_LEVEL by 0.01
- REB::FLUX FLOAT: Embraces quantum uncertainty, manifesting as a range of potentials

**Example**:
```chronovyan
DECLARE CONF::STATIC pi_constant : FLOAT = 3.14159265359;
DECLARE REB::FLUX uncertainty_factor : FLOAT = 0.05;
```

### BOOLEAN

**Essence**: Embodies the primal duality of existence—truth and falsehood, being and non-being.

**Properties**:
- Values: TRUE or FALSE
- Default value: FALSE
- Physical form: 1 byte of the tapestry

**Temporal Resonance**:
- CONF::BOOLEAN: Maintains logical consistency across all potential realities
- REB::BOOLEAN: Can exist in quantum superposition (both TRUE and FALSE simultaneously) when PARADOX_LEVEL exceeds 0.4
- CONF::STATIC BOOLEAN: Becomes an immutable truth anchor in the tapestry
- REB::FLUX BOOLEAN: May spontaneously invert its state in high-instability regions

**Example**:
```chronovyan
DECLARE CONF::STATIC initialization_complete : BOOLEAN = FALSE;
DECLARE REB::FLUX quantum_state : BOOLEAN = TRUE;
```

### STRING

**Essence**: Embodies linguistic power—the naming of things, the weaving of meaning.

**Properties**:
- Encoding: UTF-8, the universal language of digital reality
- Maximum length: 2^32 - 1 characters
- Default value: "" (the unspoken void)
- Physical form: 16 bytes plus character data

**Temporal Resonance**:
- CONF::STRING: Preserves meaning and form across all timeline branches
- REB::STRING: May experience character transmutation during temporal shifts
- CONF::STATIC STRING: Immutable text that strengthens reality, reducing PARADOX_LEVEL by 0.01
- REB::FLUX STRING: Characters may spontaneously rearrange when PARADOX_LEVEL exceeds 0.7

**Example**:
```chronovyan
DECLARE CONF::STATIC system_identifier : STRING = "Primary Timeline Alpha";
DECLARE REB::FLUX mutable_message : STRING = "Initial state";
```

### VOID

**Essence**: Embodies nothingness itself—the absence that defines presence, primarily used for function returns.

**Properties**:
- No material manifestation
- No memory allocation
- Cannot be bound to variables

**Temporal Resonance**:
- Exists beyond the influence of temporal fluctuations
- Contributes nothing to PARADOX_LEVEL, being empty of essence

**Example**:
```chronovyan
// Function that returns to the void
DECLARE CONF::STATIC initialize_system : VOID = define_pattern("Init", [
    system_setup();
    notify_completion();
]);
```

## Compound Sigils

### ARRAY

**Essence**: Embodies ordered collection—the sequence of related concepts bound together.

**Properties**:
- Element type: Any valid Chronovyan sigil
- Size: Dynamic, limited only by available Aethel
- Indexing: Zero-based
- Default value: Empty collection

**Temporal Resonance**:
- CONF::ARRAY: Elements maintain perfect order and consistency across timelines
- REB::ARRAY: Elements may spontaneously reorder during temporal disturbances
- CONF::STATIC ARRAY: Immutable collection, reducing PARADOX_LEVEL by 0.01 per element
- REB::FLUX ARRAY: Elements may spontaneously manifest or vanish when PARADOX_LEVEL exceeds 0.6

**Example**:
```chronovyan
DECLARE CONF::STATIC checkpoint_timestamps : ARRAY<TIMESTAMP> = [];
DECLARE REB::FLUX potential_outcomes : ARRAY<STRING> = ["Success", "Failure", "Paradox"];
```

### MAP

**Essence**: Embodies relationship—the binding of key to value, name to essence.

**Properties**:
- Key type: Typically primal sigils
- Value type: Any valid Chronovyan sigil
- Size: Dynamic, limited only by available Aethel
- Default value: Empty mapping

**Temporal Resonance**:
- CONF::MAP: Preserves key-value bonds with perfect fidelity
- REB::MAP: Keys may simultaneously bond to multiple values in quantum superposition
- CONF::STATIC MAP: Immutable relationships, reducing PARADOX_LEVEL by 0.01 per binding
- REB::FLUX MAP: Bindings may spontaneously shift when PARADOX_LEVEL exceeds 0.5

**Example**:
```chronovyan
DECLARE CONF::STATIC system_parameters : MAP<STRING, FLOAT> = {
    "stability_threshold": 0.8,
    "chronon_rate": 0.1,
    "aethel_generation": 0.05
};

DECLARE REB::FLUX quantum_states : MAP<INT, QUANTUM_STATE> = {};
```

### TUPLE

**Essence**: Embodies fixed collection—the immutable grouping of potentially different essences.

**Properties**:
- Element types: Any valid Chronovyan sigils
- Size: Fixed at the moment of creation
- Access: By position (zero-based) or by true name
- Default value: Default values for each element type

**Temporal Resonance**:
- CONF::TUPLE: Elements maintain essence purity across timelines
- REB::TUPLE: Elements may experience essence transmutation during temporal shifts
- CONF::STATIC TUPLE: Immutable compound form, reducing PARADOX_LEVEL by 0.01
- REB::FLUX TUPLE: Element essences may temporarily transform when PARADOX_LEVEL exceeds 0.7

**Example**:
```chronovyan
DECLARE CONF::STATIC system_bounds : TUPLE<FLOAT, FLOAT, STRING> = {0.0, 1.0, "normalized"};
DECLARE REB::FLUX event_data : TUPLE<TIMESTAMP, STRING, FLOAT> = {current_time(), "initialization", 0.5};
```

## Temporal Sigils

### TIMESTAMP

**Essence**: Embodies a specific point in the temporal continuum, the anchor of time.

**Properties**:
- Resolution: Nanosecond precision
- Range: From program origin to maximum Chronon allocation
- Default value: @Origin (program start)
- Physical form: 16 bytes of the tapestry

**Temporal Resonance**:
- CONF::TIMESTAMP: Fixed temporal reference point
- REB::TIMESTAMP: Can represent multiple moments simultaneously in superposition
- CONF::ANCHOR TIMESTAMP: Creates a stable point for REWIND_TO operations
- REB::FLUX TIMESTAMP: May drift within a temporal window when PARADOX_LEVEL > 0.4

**Example**:
```chronovyan
DECLARE CONF::ANCHOR checkpoint : TIMESTAMP = mark_timestamp("Initialization Complete");
DECLARE REB::FLUX fluctuation_point : TIMESTAMP = current_time();
```

### CHRONON_STREAM

**Essence**: Embodies the source of Chronons for temporal operations, the pulse of time.

**Properties**:
- Flow rate: Chronons per operation
- Capacity: Maximum available Chronons
- State: Active or depleted
- Physical form: 24 bytes plus internal buffer

**Temporal Resonance**:
- CONF::CHRONON_STREAM: Stable, predictable flow rate
- REB::CHRONON_STREAM: Variable flow rate affected by PARADOX_LEVEL
- CONF::SOURCE_INFUSED CHRONON_STREAM: Enhanced stability, reduced PARADOX_LEVEL impact
- REB::FLUX CHRONON_STREAM: May experience spontaneous surges or droughts

**Example**:
```chronovyan
DECLARE CONF::STATIC primary_stream : CHRONON_STREAM = HARVEST("Primary_Thread");
DECLARE REB::FLUX unstable_stream : CHRONON_STREAM = HARVEST("Temporal_Rift");
```

### AETHEL_RESERVE

**Essence**: Embodies storage for Aethel energy used in temporal operations, the reservoir of time.

**Properties**:
- Capacity: Maximum storable Aethel
- Current level: Available Aethel units
- Generation rate: Aethel per operation
- Physical form: 16 bytes of the tapestry

**Temporal Resonance**:
- CONF::AETHEL_RESERVE: Stable, predictable generation and consumption
- REB::AETHEL_RESERVE: Can briefly exceed capacity during quantum fluctuations
- CONF::STATIC AETHEL_RESERVE: Fixed capacity, reduces PARADOX_LEVEL by 0.02
- REB::FLUX AETHEL_RESERVE: May spontaneously gain or lose Aethel when PARADOX_LEVEL > 0.5

**Example**:
```chronovyan
DECLARE CONF::STATIC system_reserve : AETHEL_RESERVE = initialize_reserve(100);
DECLARE REB::FLUX emergency_reserve : AETHEL_RESERVE = initialize_reserve(50);
```

### WEAVE_PATTERN

**Essence**: Embodies a blueprint for temporal operations and transformations, the blueprint of time.

**Properties**:
- Operations: Sequence of statements
- Metadata: Aethel_Cost, Chronon_Usage, Instability_Index, Temporal_Scope
- Strategy: SEQUENTIAL, PARALLEL, or CONDITIONAL
- Physical form: 48 bytes plus operation data

**Temporal Resonance**:
- CONF::WEAVE_PATTERN: Stable, consistent execution
- REB::WEAVE_PATTERN: May evolve or adapt during execution
- CONF::STATIC WEAVE_PATTERN: Immutable operation sequence, reducing PARADOX_LEVEL by 0.02
- REB::FLUX WEAVE_PATTERN: Operations may reorder or transform when PARADOX_LEVEL > 0.6

**Example**:
```chronovyan
DECLARE CONF::STATIC stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
    INFUSE(stability_field, time_crystals, "Restabilize"),
    GUARD(stability_field > 0.6, "Low stability")
]);

DECLARE REB::FLUX exploration : WEAVE_PATTERN = define_pattern("Explore", [
    scout = deepen_exploration("Temporal_Rift")
]);
```

### QUANTUM_STATE

**Essence**: Embodies a superposition of multiple possible values, the superposition of time.

**Properties**:
- State vector: Probability distribution of possible values
- Collapse condition: When observation occurs
- Entanglement: Relationships with other quantum states
- Physical form: 64 bytes plus state data

**Temporal Resonance**:
- Exists in multiple states simultaneously until observed
- Observation collapses to a single state based on probability distribution
- Increases PARADOX_LEVEL by 0.05 per collapse
- Cannot be directly converted to CONF types without collapsing

**Example**:
```chronovyan
DECLARE REB::FLUX particle_state : QUANTUM_STATE = initialize_superposition([
    {value: "up", probability: 0.5},
    {value: "down", probability: 0.5}
]);
```

### TIMELINE

**Essence**: Embodies a complete execution path through the program, the timeline of time.

**Properties**:
- Events: Sequence of temporal operations
- State: Active, dormant, or terminated
- Branches: Child timelines that have split from this one
- Physical form: 256 bytes plus event data

**Temporal Resonance**:
- CONF::TIMELINE: Stable, deterministic execution path
- REB::TIMELINE: Can branch and merge dynamically
- CONF::ANCHOR TIMELINE: Primary reference timeline for the program
- REB::WEAVER TIMELINE: Can be manipulated to create alternate execution paths

**Example**:
```chronovyan
DECLARE CONF::ANCHOR main_timeline : TIMELINE = initialize_primary_timeline();
DECLARE REB::WEAVER alternate_timeline : TIMELINE = branch_timeline(main_timeline);
```

## Variable Flags

Chronovyan variables can be adorned with special flags that modify their behavior:

### ::STATIC

**Effect**: Variable cannot change after initialization
**PARADOX_LEVEL Impact**: Reduces by 0.01 per variable
**Temporal Behavior**: Exempt from temporal fluctuations
**Example**:
```chronovyan
DECLARE CONF::STATIC pi : FLOAT = 3.14159;
```

### ::FLUX

**Effect**: Variable can change unpredictably
**PARADOX_LEVEL Impact**: Increases by 0.02 per variable
**Temporal Behavior**: Subject to quantum effects, value can spontaneously change
**Example**:
```chronovyan
DECLARE REB::FLUX uncertainty : FLOAT = 0.1;
```

### ::ANCHOR

**Effect**: Serves as a stability point for timelines
**PARADOX_LEVEL Impact**: Neutral
**Temporal Behavior**: Can be targeted by REWIND_TO operations
**Example**:
```chronovyan
DECLARE CONF::ANCHOR checkpoint : TIMESTAMP = mark_timestamp("Save Point");
```

### ::WEAVER

**Effect**: Can manipulate other variables' temporal properties
**PARADOX_LEVEL Impact**: Increases by 0.01 per manipulation
**Temporal Behavior**: Influences timeline branching and merging
**Example**:
```chronovyan
DECLARE REB::WEAVER manipulator : TEMPORAL_SIGNATURE = generate_signature("Manipulator");
```

### ::ECHO

**Effect**: Maintains history of all past values
**PARADOX_LEVEL Impact**: Increases by 0.01 per state preserved
**Temporal Behavior**: All past states are preserved and can be retrieved
**Example**:
```chronovyan
DECLARE CONF::ECHO state_tracker : INT = 0;
```

### ::SOURCE_INFUSED

**Effect**: Connected to the fundamental power of The Source
**PARADOX_LEVEL Impact**: Variable, depending on operations
**Temporal Behavior**: Can override normal paradox constraints
**Example**:
```chronovyan
DECLARE REB::SOURCE_INFUSED time_crystals : CHRONON_STREAM = HARVEST("Prime_Thread");
```

### ::VOLATILE

**Effect**: Rapid state changes, no guaranteed persistence
**PARADOX_LEVEL Impact**: Increases by 0.01 per variable
**Temporal Behavior**: Value may not persist between operations if PARADOX_LEVEL > 0.7
**Example**:
```chronovyan
DECLARE REB::VOLATILE temp : INT = calculate_complex_value();
```

## Type Conversion

### Implicit Conversions

Chronovyan supports limited implicit conversions that preserve data integrity:

1. INT → FLOAT: Automatic conversion when integer appears in floating-point context
2. Any primitive type → STRING: Automatic conversion in string concatenation
3. CONF type → REB type of same base type: One-way conversion permitted

### Explicit Conversions

More complex conversions require explicit conversion functions:

```chronovyan
// Convert a floating point to an integer
DECLARE CONF int_value : INT = convert_to_int(3.14159);

// Convert an integer to a string
DECLARE CONF str_value : STRING = convert_to_string(42);

// Collapse a quantum state to a single value
DECLARE CONF collapsed : BOOLEAN = collapse_quantum(quantum_bit);

// Attempt to stabilize a rebellious variable
DECLARE CONF stable_value : FLOAT = stabilize_type(unstable_float);
```

### Conversion Restrictions

Some conversions are prohibited or have special requirements:

1. REB → CONF: Requires explicit stabilize_type() function and may fail if PARADOX_LEVEL > 0.7
2. QUANTUM_STATE → Any non-quantum type: Requires collapse_quantum() and increases PARADOX_LEVEL by 0.05
3. TIMELINE → Any other type: Prohibited to prevent timeline corruption
4. TEMPORAL_SIGNATURE → Any other type: Prohibited to maintain timeline integrity

## Complete Example: Working with Data Types

```chronovyan
// A comprehensive example demonstrating data type usage

ANTECEDENCE {
    // Initialize fundamental resources
    DECLARE CONF::STATIC system_identifier : STRING = "Timeline Alpha-7";
    DECLARE CONF::ANCHOR @initialization_point;
    
    // Configure system parameters
    DECLARE CONF::STATIC system_parameters : MAP<STRING, FLOAT> = {
        "stability_threshold": 0.7,
        "chronon_rate": 0.05,
        "aethel_consumption": 0.02
    };
    
    // Prepare resource streams
    DECLARE CONF::STATIC primary_stream : CHRONON_STREAM = HARVEST("Primary_Thread");
    DECLARE REB::FLUX auxiliary_stream : CHRONON_STREAM = HARVEST("Temporal_Rift");
    
    // Initialize reserves
    DECLARE CONF::STATIC main_reserve : AETHEL_RESERVE = initialize_reserve(200);
    DECLARE REB::FLUX emergency_reserve : AETHEL_RESERVE = initialize_reserve(50);
    
    // Define temporal patterns
    DECLARE CONF::STATIC stabilization : WEAVE_PATTERN = define_pattern("Stabilize", [
        INFUSE(stability_field, primary_stream, "Restabilize"),
        GUARD(stability_field > system_parameters["stability_threshold"], "Low stability")
    ]);
    
    DECLARE REB::FLUX exploration : WEAVE_PATTERN = define_pattern("Explore", [
        DECLARE REB::FLUX scout : TEMPORAL_SIGNATURE = deepen_exploration("Temporal_Rift"),
        REWIND_TO @initialization_point IF scout.instability > 0.8
    ]);
}

CONCURRENCY {
    // Initialize working variables
    DECLARE CONF stability_field : FLOAT = 1.0;
    DECLARE CONF::ECHO stability_history : ARRAY<FLOAT> = [];
    DECLARE REB::FLUX quantum_state : QUANTUM_STATE = initialize_superposition([
        {value: "stable", probability: 0.7},
        {value: "unstable", probability: 0.3}
    ]);
    
    // Begin temporal operations
    DECLARE CONF current_state : STRING = collapse_quantum(quantum_state);
    
    IF (current_state == "stable") {
        // Execute stable path
        DECLARE CONF results : ARRAY<FLOAT> = [];
        FOR_CHRONON(10) {
            stability_field -= 0.05;
            stability_history.append(stability_field);
            results.append(stability_field * (1.0 - PARADOX_LEVEL));
        }
        
        // Analyze results
        DECLARE CONF average_stability : FLOAT = 0.0;
        FOR_CHRONON(results.length) {
            average_stability += results[i];
        }
        average_stability /= results.length;
        
        // Adjust system based on analysis
        IF (average_stability < system_parameters["stability_threshold"]) {
            apply_pattern(stabilization);
        }
    } ELSE {
        // Execute experimental path with higher PARADOX_LEVEL impact
        DECLARE REB::FLUX exploration_results : MAP<TEMPORAL_SIGNATURE, FLOAT> = {};
        
        || Thread_1: {
            DECLARE REB::WEAVER explorer_1 : TEMPORAL_SIGNATURE = generate_signature("Explorer_1");
            exploration_results[explorer_1] = perform_exploration(auxiliary_stream, 5);
        }
        
        || Thread_2: {
            DECLARE REB::WEAVER explorer_2 : TEMPORAL_SIGNATURE = generate_signature("Explorer_2");
            exploration_results[explorer_2] = perform_exploration(auxiliary_stream, 10);
        }
        
        // Wait for both threads to complete
        SYNCHRONIZE;
        
        // Find best exploration path
        DECLARE CONF best_explorer : TEMPORAL_SIGNATURE = find_max_value_key(exploration_results);
        DECLARE CONF best_value : FLOAT = exploration_results[best_explorer];
        
        // Apply results to stability field
        stability_field += best_value * 0.1;
    }
}

CONSEQUENCE {
    // Final stability check
    GUARD(stability_field > system_parameters["stability_threshold"] * 0.8, "Critical stability failure");
    
    // Convert types for output
    DECLARE CONF final_stability : STRING = convert_to_string(stability_field);
    DECLARE CONF final_paradox : STRING = convert_to_string(PARADOX_LEVEL);
    
    // Output results
    print("Final Stability: " + final_stability);
    print("Final PARADOX_LEVEL: " + final_paradox);
    print("Timeline: " + system_identifier);
    
    // Perform cleanup
    DECLARE CONF cleanup_result : BOOLEAN = release_resources(emergency_reserve);
    IF (!cleanup_result) {
        INFUSE(stability_field, main_reserve, "Emergency stabilization");
    }
}
```

**Lore Tidbit**: The ancient Weavers believed that to truly master a sigil, one must not merely understand its properties but commune with its essence—to feel the INT's discrete steps, to flow with the FLOAT's continuity, to embrace the BOOLEAN's duality. Only then does one truly become a Master Weaver. 