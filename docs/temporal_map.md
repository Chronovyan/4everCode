---
title: TEMPORAL_MAP Type in Chronovyan
description: Documentation for temporal_map.md
weight: 100
draft: true
---

# TEMPORAL_MAP Type in Chronovyan

## Overview

The TEMPORAL_MAP type in Chronovyan is an advanced key-value container with temporal semantics, designed for efficient storage and retrieval of key-value pairs across time. It provides extensive functionality for tracking changes, detecting patterns, and predicting future states, making it ideal for temporal analytics, state management, and simulation in temporal environments.

The `TemporalMap` class is a comprehensive implementation of a map data structure with full temporal awareness, allowing developers to track, analyze, and manipulate the evolution of key-value associations throughout different temporal cycles.

## Key Features

### Basic Features

- **Key-value storage**: Efficient storage and retrieval of values associated with keys
- **Dynamic size**: Automatically grows and shrinks as needed
- **Flexible types**: Both keys and values can be of any type supported by Chronovyan
- **Resource tracking**: Automatically tracks chronon consumption for operations

### Higher-order Functions

- **ForEach**: Apply a function to each key-value pair
- **Map values**: Transform all values using a custom function
- **Filter**: Create a new map containing only pairs that satisfy a predicate
- **Merge**: Combine two maps with custom conflict resolution

### Temporal Features

- **History tracking**: Save and restore map states from different temporal cycles
- **Temporal querying**: Access key-value pairs from any saved cycle
- **Key history**: Track how a specific key's value changes over time
- **Pattern detection**: Identify temporal patterns in key value changes
- **Future prediction**: Extrapolate future values based on historical data
- **Change frequency**: Identify which keys change most frequently

## Using the TEMPORAL_MAP Type

### Basic Usage

```cpp
// Create a runtime
auto runtime = std::make_shared<TemporalRuntime>();

// Create an empty map
TemporalMap map(runtime);

// Set key-value pairs
map.set(Value("name"), Value("Chronovyan"));
map.set(Value("version"), Value(1.0));

// Access values
Value name = map.at(Value("name"));  // Returns "Chronovyan"
bool hasKey = map.contains(Value("version"));  // Returns true

// Remove a key
map.remove(Value("version"));

// Get all keys or values
std::vector<Value> keys = map.keys();
std::vector<Value> values = map.values();

// Print the map
std::cout << map.toString() << std::endl;  // Outputs: {name: Chronovyan}
```

### Higher-order Functions

```cpp
// Create a map with numeric values
TemporalMap numericMap(runtime);
numericMap.set(Value(1), Value(10));
numericMap.set(Value(2), Value(20));
numericMap.set(Value(3), Value(30));

// ForEach - calculate sum of values
int sum = 0;
numericMap.forEach([&sum](const Value& key, const Value& value) {
    sum += value.asInteger();
});
// sum is now 60 (10 + 20 + 30)

// Map values - double each value
auto doubledMap = numericMap.mapValues([](const Value& key, const Value& value) {
    return Value(static_cast<int64_t>(value.asInteger() * 2));
});
// doubledMap contains {1: 20, 2: 40, 3: 60}

// Filter - keep only odd keys
auto oddKeysMap = numericMap.filter([](const Value& key, const Value& value) {
    return key.asInteger() % 2 != 0;
});
// oddKeysMap contains {1: 10, 3: 30}

// Merge two maps
TemporalMap extraMap(runtime);
extraMap.set(Value(3), Value(300));  // Conflicting key
extraMap.set(Value(4), Value(40));   // New key

// Custom conflict resolution
auto mergedMap = numericMap.merge(extraMap, [](const Value& key, const Value& val1, const Value& val2) {
    return Value(static_cast<int64_t>(val1.asInteger() + val2.asInteger()));
});
// mergedMap contains {1: 10, 2: 20, 3: 330, 4: 40}
```

### Temporal Features

```cpp
// Create a map
TemporalMap temporalMap(runtime);
temporalMap.set(Value("counter"), Value(100));

// Save the current state at cycle 1
temporalMap.saveHistoryCheckpoint(1);

// Modify the map
temporalMap.set(Value("counter"), Value(200));
temporalMap.set(Value("message"), Value("Added at cycle 2"));

// Save the current state at cycle 2
temporalMap.saveHistoryCheckpoint(2);

// Access historical values
Value counterAtCycle1 = temporalMap.getAtCycle(Value("counter"), 1);  // Returns 100
Value counterAtCycle2 = temporalMap.getAtCycle(Value("counter"), 2);  // Returns 200

// Check key existence in history
bool hasMessageAtCycle1 = temporalMap.containedAtCycle(Value("message"), 1);  // Returns false
bool hasMessageAtCycle2 = temporalMap.containedAtCycle(Value("message"), 2);  // Returns true

// Get all keys at a specific cycle
std::vector<Value> keysAtCycle1 = temporalMap.keysAtCycle(1);  // Contains only "counter"
std::vector<Value> keysAtCycle2 = temporalMap.keysAtCycle(2);  // Contains "counter" and "message"

// Restore the map to an earlier state
temporalMap.restoreFromCycle(1);  // Map now contains only {counter: 100}

// Get all historical cycles
std::vector<int> cycles = temporalMap.getHistoricalCycles();  // Contains 1 and 2
```

### Advanced Temporal Features

```cpp
// Track the history of a key across cycles
std::map<int, Value> counterHistory = temporalMap.trackKeyHistory(Value("counter"), 1, 10);
// Returns a map of cycle numbers to values for the "counter" key

// Find keys that change most frequently
std::map<Value, int> frequentlyChanged = temporalMap.getFrequentlyChangedKeys(1, 10, 5);
// Returns the top 5 most frequently changed keys

// Detect temporal patterns in key changes
std::string pattern = temporalMap.detectKeyPattern(Value("counter"), {1, 2, 3, 4, 5});
// Returns a pattern description like "increasing", "decreasing", "cyclic", etc.

// Predict future values based on historical data
Value predictedValue = temporalMap.predictFutureValue(Value("counter"), 10, {1, 2, 3, 4, 5});
// Predicts the value at cycle 10 based on cycles 1-5
```

## Integration with Custom Type System

The TEMPORAL_MAP type is registered with Chronovyan's custom type system, allowing it to be used seamlessly in Chronovyan scripts and programs:

```cpp
// Register the TEMPORAL_MAP type
CustomTypeSystem typeSystem;
registerTemporalMapType(typeSystem);

// Use the type in scripts
// TEMPORAL_MAP<string, int> countMap;
```

## Resource Consumption

Operations on TEMPORAL_MAP objects consume chronons based on the operation complexity and the size of the map:

- **Basic operations** (set, remove, contains): Logarithmic chronon cost based on map size
- **Collection operations** (keys, values): Linear chronon cost based on map size
- **Higher-order functions** (forEach, mapValues, filter): Linear chronon cost based on map size
- **Temporal operations** (saveHistoryCheckpoint, restoreFromCycle): Higher cost, scaling with map size
- **Advanced temporal operations** (detectKeyPattern, predictFutureValue): Highest cost, based on the complexity of the operation

## Performance Considerations

### Memory Efficiency

- Efficient storage of key-value pairs using a balanced tree structure
- Temporal history is stored as complete snapshots at each checkpoint
- Large maps with frequent checkpoints may consume significant memory

### Computational Efficiency

- Key lookup is O(log n) where n is the number of keys
- Higher-order functions are O(n) operations
- Pattern detection and prediction are more expensive operations

### Thread Safety

- Map operations are not inherently thread-safe
- For concurrent access, external synchronization is required
- The temporal runtime tracks resource usage safely across threads

## Examples

For comprehensive examples of using the TEMPORAL_MAP type, refer to:
- `examples/map_demo.cpp`: Demonstrates all major features
- `tests/temporal_map_test.cpp`: Shows detailed usage patterns

## API Reference

### TemporalMap

#### Constructors

- `TemporalMap(std::shared_ptr<TemporalRuntime> runtime = nullptr)`
- `TemporalMap(std::map<Value, Value> elements, std::shared_ptr<TemporalRuntime> runtime = nullptr)`

#### Basic Methods

- `size_t size() const`
- `bool empty() const`
- `bool contains(const Value& key) const`
- `const Value& at(const Value& key) const`
- `Value& at(const Value& key)`
- `void set(const Value& key, const Value& value)`
- `bool remove(const Value& key)`
- `std::vector<Value> keys() const`
- `std::vector<Value> values() const`
- `const std::map<Value, Value>& getElements() const`
- `std::string toString() const`

#### Higher-order Functions

- `void forEach(std::function<void(const Value& key, const Value& value)> func) const`
- `std::shared_ptr<TemporalMap> mapValues(std::function<Value(const Value& key, const Value& value)> func) const`
- `std::shared_ptr<TemporalMap> filter(std::function<bool(const Value& key, const Value& value)> pred) const`
- `std::shared_ptr<TemporalMap> merge(const TemporalMap& other, std::function<Value(const Value& key, const Value& val1, const Value& val2)> conflictResolver = nullptr) const`

#### Temporal Methods

- `Value getAtCycle(const Value& key, int cycle) const`
- `bool containedAtCycle(const Value& key, int cycle) const`
- `std::vector<Value> keysAtCycle(int cycle) const`
- `void saveHistoryCheckpoint(int cycle)`
- `bool restoreFromCycle(int cycle)`
- `std::vector<int> getHistoricalCycles() const`
- `void clearHistory()`

#### Advanced Temporal Methods

- `std::map<int, Value> trackKeyHistory(const Value& key, int startCycle, int endCycle) const`
- `std::map<Value, int> getFrequentlyChangedKeys(int startCycle, int endCycle, size_t limit = 0) const`
- `std::string detectKeyPattern(const Value& key, const std::vector<int>& cycles) const`
- `Value predictFutureValue(const Value& key, int futureCycle, const std::vector<int>& historicalCycles) const`

#### Runtime Management

- `void setTemporalRuntime(std::shared_ptr<TemporalRuntime> runtime)`
- `std::shared_ptr<TemporalRuntime> getTemporalRuntime() const`

## Future Enhancements

Future versions of the TEMPORAL_MAP type may include:

1. **Optimized history storage**: Incremental snapshots instead of full copies
2. **Advanced pattern recognition**: Machine learning-based pattern detection
3. **Key relationship analysis**: Identify correlations between different keys
4. **Partial restoration**: Restore only specific keys from a checkpoint
5. **Optimistic locking**: Better concurrency support for multi-threaded access
6. **Continuous aggregation**: Automated aggregation of temporal data
7. **Custom key comparators**: Support for user-defined key ordering

## See Also

- [ChronovyanVector](./vector_type.md): Documentation for the VECTOR type
- [Temporal Runtime](./temporal_runtime.md): Details on resource tracking and temporal features
- [Custom Type System](./custom_type_system.md): How types are defined and registered in Chronovyan
- [Value](./value.md): Information about the Value class used for keys and values