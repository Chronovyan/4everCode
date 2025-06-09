---
title: TEMPORAL_SEQUENCE Type in Chronovyan
description: Documentation for temporal_sequence.md
weight: 100
draft: true
---

# TEMPORAL_SEQUENCE Type in Chronovyan

## Overview

The TEMPORAL_SEQUENCE type in Chronovyan is a specialized collection designed explicitly for time-variant data. Unlike regular sequences with temporal features added on, the `TemporalSequence` class is built from the ground up to efficiently store, analyze, and manipulate data that varies over time. It provides native support for tracking values across multiple temporal cycles and offers a rich set of operations for temporal analysis and manipulation.

This type is particularly well-suited for time-series data, temporal patterns, and any scenario where the evolution of values over time is important to track and analyze.

## Key Features

### Basic Features

- **Time-indexed values**: Store values associated with specific temporal cycles
- **Efficient organization**: Automatically maintains chronological order of time points
- **Dynamic size**: Automatically grows and shrinks as needed
- **Type flexibility**: Values can be of any type supported by Chronovyan
- **Resource tracking**: Automatically tracks chronon consumption for operations

### Temporal Operations

- **Cycle management**: Easily add, retrieve, or remove values at specific cycles
- **Range queries**: Get slices of the sequence between cycles
- **Interpolation**: Calculate values between existing time points using various methods
- **Resampling**: Generate sequences with regular cycle intervals
- **Pattern detection**: Identify patterns and trends in temporal data
- **Projection**: Extend sequences into the future based on detected patterns

### Functional Operations

- **Map**: Transform each time point using a custom function
- **Filter**: Create a new sequence containing only time points that satisfy a predicate
- **Reduce**: Aggregate the sequence into a single value
- **Merge**: Combine two sequences with various strategies for handling overlaps
- **Statistical analysis**: Calculate temporal statistics on the sequence

## Using the TEMPORAL_SEQUENCE Type

### Basic Usage

```cpp
// Create a runtime
auto runtime = std::make_shared<TemporalRuntime>();

// Create an empty sequence
TemporalSequence sequence(runtime);

// Add time points
sequence.set(1, Value(100));  // Cycle 1, value 100
sequence.set(3, Value(300));  // Cycle 3, value 300
sequence.set(5, Value(500));  // Cycle 5, value 500

// Access values
Value value = sequence.at(3);  // Returns 300
bool exists = sequence.hasCycle(2);  // Returns false

// Get all cycles
std::vector<int> cycles = sequence.getCycles();  // Contains 1, 3, 5

// Update a value
sequence.set(3, Value(350));  // Change value at cycle 3

// Remove a time point
sequence.remove(1);  // Remove the point at cycle 1

// Clear the sequence
sequence.clear();
```

### Time Range Operations

```cpp
// Create a sequence with time points
TemporalSequence sequence(runtime);
for (int i = 0; i < 10; i++) {
    sequence.set(i, Value(i * 10));
}

// Get the range of cycles
int earliest = sequence.getEarliestCycle();  // Returns 0
int latest = sequence.getLatestCycle();      // Returns 9

// Create a slice of the sequence
auto sliced = sequence.slice(3, 7);  // Contains cycles 3, 4, 5, 6, 7

// Interpolate to get a value between existing time points
Value interpolated = sequence.interpolate(2.5, "linear");  // Linear interpolation
Value stepValue = sequence.interpolate(2.5, "step");      // Step interpolation

// Resample to regular intervals
auto resampled = sequence.resample(2, "linear");  // Points at cycles 0, 2, 4, 6, 8
```

### Functional Operations

```cpp
// Create a sequence with time points
TemporalSequence sequence(runtime);
for (int i = 1; i <= 5; i++) {
    sequence.set(i, Value(i * 10));
}

// Map - Double each value
auto doubled = sequence.map([](int cycle, const Value& value) {
    return Value(static_cast<int64_t>(value.asInteger() * 2));
});
// doubled contains: (1:20), (2:40), (3:60), (4:80), (5:100)

// Filter - Keep only even cycles
auto filtered = sequence.filter([](int cycle, const Value& value) {
    return cycle % 2 == 0;
});
// filtered contains: (2:20), (4:40)

// Reduce - Sum all values
Value sum = sequence.reduce(
    [](const Value& acc, int cycle, const Value& value) {
        return Value(static_cast<int64_t>(acc.asInteger() + value.asInteger()));
    },
    Value(static_cast<int64_t>(0))
);
// sum is 150 (10 + 20 + 30 + 40 + 50)
```

### Temporal Analysis

```cpp
// Create a sequence with time points
TemporalSequence sequence(runtime);
for (int i = 1; i <= 10; i++) {
    // Linear trend with some noise
    double value = i * 5.0 + (std::rand() % 10 - 5);
    sequence.set(i, Value(value));
}

// Calculate statistics
auto stats = sequence.statistics();
// Contains min_cycle, max_cycle, cycle_range, point_count,
// min_value, max_value, mean, variance, std_dev

// Detect patterns
auto patterns = sequence.detectPatterns();
// Returns identified patterns as sub-sequences

// Project into the future
auto projected = sequence.project(3);
// Extends the sequence 3 cycles into the future
```

### Merging Sequences

```cpp
// Create two sequences
TemporalSequence seq1(runtime);
seq1.set(1, Value(10));
seq1.set(3, Value(30));
seq1.set(5, Value(50));

TemporalSequence seq2(runtime);
seq2.set(2, Value(20));
seq2.set(3, Value(35));  // Overlapping cycle
seq2.set(6, Value(60));

// Merge with "replace" strategy (seq2 values take precedence for overlaps)
auto merged1 = seq1.merge(seq2, "replace");
// merged1 contains: (1:10), (2:20), (3:35), (5:50), (6:60)

// Merge with "keep" strategy (seq1 values preserved for overlaps)
auto merged2 = seq1.merge(seq2, "keep");
// merged2 contains: (1:10), (2:20), (3:30), (5:50), (6:60)

// Merge with "combine" strategy (values combined for overlaps)
auto merged3 = seq1.merge(seq2, "combine");
// merged3 contains: (1:10), (2:20), (3:32.5), (5:50), (6:60)
```

## Advanced Features

### Interpolation Methods

The `TemporalSequence` class supports different interpolation methods when querying values between existing time points:

- **Linear Interpolation** (`"linear"`): Calculates a proportional value between two points. This is useful for smooth transitions and is the default method.
- **Step Interpolation** (`"step"`): Uses the value of the previous point. This is useful for discrete values that should not be averaged.
- **Spline Interpolation** (`"spline"`): Currently implemented as linear interpolation, but could be extended to support more sophisticated curve fitting in the future.

```cpp
// Create a sequence
TemporalSequence sequence(runtime);
sequence.set(1, Value(100.0));
sequence.set(5, Value(500.0));

// Linear interpolation (default)
Value linear = sequence.interpolate(3, "linear");  // Returns 300.0

// Step interpolation
Value step = sequence.interpolate(3, "step");  // Returns 100.0
```

### Pattern Detection and Projection

The `TemporalSequence` class includes basic capabilities for detecting patterns in temporal data and projecting future values based on those patterns:

```cpp
// Create a sequence with a repeating pattern
TemporalSequence sequence(runtime);
sequence.set(1, Value(10));
sequence.set(2, Value(20));
sequence.set(3, Value(30));
sequence.set(4, Value(20));
sequence.set(5, Value(10));

// Detect patterns
auto patterns = sequence.detectPatterns();

// Project future values
auto projected = sequence.project(2);
// Extends the sequence by repeating the pattern 2 more times
```

### Statistical Analysis

The `statistics()` method provides a comprehensive set of metrics about the sequence:

```cpp
TemporalSequence sequence(runtime);
for (int i = 1; i <= 10; i++) {
    sequence.set(i, Value(i * 10.0));
}

auto stats = sequence.statistics();
// Returns a map with the following keys:
// - min_cycle: 1
// - max_cycle: 10
// - cycle_range: 9
// - point_count: 10
// - min_value: 10.0
// - max_value: 100.0
// - mean: 55.0
// - variance: 825.0
// - std_dev: 28.7...
```

## Integration with Custom Type System

The TEMPORAL_SEQUENCE type is registered with Chronovyan's custom type system, allowing it to be used seamlessly in Chronovyan scripts and programs:

```cpp
// Register the TEMPORAL_SEQUENCE type
CustomTypeSystem typeSystem;
registerTemporalSequenceType(typeSystem);

// Use the type in scripts
// TEMPORAL_SEQUENCE<int> timeSeriesData;
```

## Resource Consumption

Operations on TEMPORAL_SEQUENCE objects consume chronons based on the operation complexity and the size of the sequence:

- **Basic operations** (set, remove): Logarithmic chronon cost based on sequence size
- **Range operations** (slice, interpolate): Higher chronon cost based on range size
- **Higher-order functions** (map, filter, reduce): Linear chronon cost based on sequence size
- **Statistical operations** (statistics, detectPatterns): Higher cost, scaling with sequence size
- **Projection operations** (project): Highest cost, based on projection distance and sequence size

## Performance Considerations

### Memory Efficiency

- Time points are stored in a sorted array for efficient access and range operations
- Binary search is used for point lookup by cycle
- The sequence maintains only one value per cycle, minimizing redundancy

### Computational Efficiency

- Time point lookup is O(log n) where n is the number of points
- Slicing and interpolation operations are optimized for performance
- Pattern detection and projection are more complex operations and may be more resource-intensive

### Thread Safety

- Sequence operations are not inherently thread-safe
- For concurrent access, external synchronization is required
- The temporal runtime tracks resource usage safely across threads

## Examples

For comprehensive examples of using the TEMPORAL_SEQUENCE type, refer to:
- `examples/sequence_demo.cpp`: Demonstrates all major features
- `tests/temporal_sequence_test.cpp`: Shows detailed usage patterns

## API Reference

### TemporalSequence

#### TimePoint Struct

- `struct TimePoint`
  - `int cycle`: The temporal cycle
  - `Value value`: The value at this cycle
  - `TimePoint(int c, const Value& v)`: Constructor
  - `bool operator<(const TimePoint& other) const`: Comparison operator
  - `bool operator==(const TimePoint& other) const`: Equality operator

#### Constructors

- `TemporalSequence(std::shared_ptr<TemporalRuntime> runtime = nullptr)`
- `TemporalSequence(const std::vector<TimePoint>& points, std::shared_ptr<TemporalRuntime> runtime = nullptr)`

#### Basic Methods

- `size_t size() const`
- `bool empty() const`
- `std::vector<int> getCycles() const`
- `bool hasCycle(int cycle) const`
- `const Value& at(int cycle) const`
- `Value& at(int cycle)`
- `void set(int cycle, const Value& value)`
- `bool remove(int cycle)`
- `void clear()`
- `std::string toString() const`

#### Range Methods

- `std::shared_ptr<TemporalSequence> slice(int startCycle, int endCycle) const`
- `int getEarliestCycle() const`
- `int getLatestCycle() const`
- `Value interpolate(int cycle, const std::string& method = "linear") const`
- `std::shared_ptr<TemporalSequence> resample(int interval, const std::string& method = "linear") const`

#### Analysis Methods

- `std::vector<std::shared_ptr<TemporalSequence>> detectPatterns() const`
- `std::shared_ptr<TemporalSequence> project(int cycles) const`
- `std::map<std::string, Value> statistics() const`

#### Functional Methods

- `std::shared_ptr<TemporalSequence> map(std::function<Value(int, const Value&)> func) const`
- `std::shared_ptr<TemporalSequence> filter(std::function<bool(int, const Value&)> pred) const`
- `Value reduce(std::function<Value(const Value&, int, const Value&)> func, const Value& initial) const`
- `std::shared_ptr<TemporalSequence> merge(const TemporalSequence& other, const std::string& strategy = "replace") const`

#### Runtime Management

- `void setTemporalRuntime(std::shared_ptr<TemporalRuntime> runtime)`
- `std::shared_ptr<TemporalRuntime> getTemporalRuntime() const`
- `static int64_t getChrononsForOperation(const std::string& operation, size_t sequenceSize)`

## Future Enhancements

Future versions of the TEMPORAL_SEQUENCE type may include:

1. **Advanced pattern detection**: More sophisticated algorithms for identifying temporal patterns
2. **Machine learning integration**: Leverage ML techniques for prediction and anomaly detection
3. **Streaming capabilities**: Support for real-time temporal data streams
4. **Differential analysis**: Compare sequences and identify significant differences
5. **Event detection**: Identify notable events or transitions in temporal data
6. **Windowing operations**: Support for rolling windows and sliding analysis
7. **Compression**: Efficient storage for very large temporal sequences

## See Also

- [TemporalMap](./temporal_map.md): Documentation for the TEMPORAL_MAP type
- [ChronovyanVector](./vector_type.md): Documentation for the VECTOR type
- [Temporal Runtime](./temporal_runtime.md): Details on resource tracking and temporal features
- [Custom Type System](./custom_type_system.md): How types are defined and registered in Chronovyan
- [Value](./value.md): Information about the Value class used for sequence data