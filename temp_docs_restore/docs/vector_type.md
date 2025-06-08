---
title: VECTOR Type in Chronovyan
description: Documentation for vector_type.md
weight: 100
draft: true
---

# VECTOR Type in Chronovyan

## Overview

The VECTOR type in Chronovyan is a powerful multi-dimensional container with temporal semantics, designed for efficient storage and manipulation of collections of data across time. It provides extensive functionality for numerical computations, matrix operations, and temporal transformations, making it ideal for scientific computing, data analysis, and simulation in temporal environments.

The VECTOR implementation consists of two main classes:
- `ChronovyanVector`: A single-dimensional dynamic array with temporal features
- `MultiDimensionalVector`: A multi-dimensional extension providing matrix and tensor operations

## Key Features

### Basic Features

- **Multi-dimensional storage**: Support for vectors, matrices, and higher-dimensional tensors
- **Dynamic sizing**: Ability to grow and reshape as needed
- **Type flexibility**: Can store any value type supported by Chronovyan
- **Resource tracking**: Automatically tracks chronon consumption for operations

### Matrix and Tensor Operations

- **Element access and manipulation**: Get, set, and iterate over elements
- **Slicing**: Extract subsets of data along any dimension
- **Reshaping**: Change the dimensions while preserving data
- **Matrix operations**: Transpose, matrix multiplication, and element-wise operations
- **Higher-order functions**: Map, reduce, and transform operations on elements
- **String representation**: Human-readable representation of vector contents

### Temporal Features

- **History tracking**: Save and restore states from different temporal cycles
- **Temporal querying**: Access element values from any saved cycle
- **Temporal transformations**: Apply functions across temporal states
- **Gradient calculation**: Compute rates of change between cycles
- **State prediction**: Extrapolate future states based on historical data

## Using the VECTOR Type

### Basic Usage

```cpp
// Create a runtime
auto runtime = std::make_shared<TemporalRuntime>();

// Create a 1D vector with 5 elements
std::vector<size_t> dimensions = {5};
MultiDimensionalVector vector(dimensions, runtime);

// Set values
for (size_t i = 0; i < 5; ++i) {
    vector.set({i}, Value(static_cast<int64_t>(i * i)));
}

// Access values
Value v = vector.at({2});  // Get value at index 2

// Print the vector
std::cout << vector.toString() << std::endl;
```

### Matrix Operations

```cpp
// Create a 3x3 matrix
std::vector<size_t> dimensions = {3, 3};
MultiDimensionalVector matrix(dimensions, runtime);

// Fill with values (identity matrix)
for (size_t i = 0; i < 3; ++i) {
    for (size_t j = 0; j < 3; ++j) {
        matrix.set({i, j}, Value(static_cast<int64_t>(i == j ? 1 : 0)));
    }
}

// Transpose the matrix
auto transposed = matrix.transpose();

// Matrix multiplication
auto product = matrix.matrixMultiply(transposed);

// Reshape a matrix
std::vector<size_t> newDimensions = {1, 9};
matrix.reshape(newDimensions);  // Now it's a 1x9 matrix
```

### Temporal Operations

```cpp
// Create a vector
std::vector<size_t> dimensions = {3};
MultiDimensionalVector vector(dimensions, runtime);

// Set initial values
vector.set({0}, Value(static_cast<int64_t>(10)));
vector.set({1}, Value(static_cast<int64_t>(20)));
vector.set({2}, Value(static_cast<int64_t>(30)));

// Save state at cycle 1
vector.saveHistoryCheckpoint(1);

// Change values
vector.set({0}, Value(static_cast<int64_t>(15)));
vector.set({1}, Value(static_cast<int64_t>(25)));
vector.set({2}, Value(static_cast<int64_t>(35)));

// Save state at cycle 2
vector.saveHistoryCheckpoint(2);

// Access historical values
Value v1 = vector.getAtCycle({0}, 1);  // Value at index 0, cycle 1

// Restore from a previous cycle
vector.restoreFromCycle(1);

// Calculate temporal gradient
auto gradient = vector.temporalGradient(1, 2);

// Predict future state
auto future = vector.predictFutureState(3, {1, 2});
```

### Advanced Operations

```cpp
// Create a 2x3 matrix
std::vector<size_t> dimensions = {2, 3};
MultiDimensionalVector matrix(dimensions, runtime);
// Fill with values...

// Map a function over elements (square each element)
auto squared = matrix.map([](const Value& v) -> Value {
    if (v.isInteger()) {
        int64_t val = v.asInteger();
        return Value(static_cast<int64_t>(val * val));
    }
    return v;
});

// Element-wise operation (add two matrices)
auto matrix2 = /* another 2x3 matrix */;
auto sum = matrix.elementWise(matrix2, [](const Value& a, const Value& b) -> Value {
    if (a.isInteger() && b.isInteger()) {
        return Value(static_cast<int64_t>(a.asInteger() + b.asInteger()));
    }
    return Value();
});

// Reduce along a dimension (sum columns)
auto columnSums = matrix.reduce(0, [](const Value& a, const Value& b) -> Value {
    if (a.isInteger() && b.isInteger()) {
        return Value(static_cast<int64_t>(a.asInteger() + b.asInteger()));
    }
    return Value();
}, Value(static_cast<int64_t>(0)));
```

## Integration with Custom Type System

The VECTOR type is registered with Chronovyan's custom type system, allowing it to be used seamlessly in Chronovyan scripts and programs:

```cpp
// Register the VECTOR type
CustomTypeSystem typeSystem;
registerMultiDimensionalVectorType(typeSystem);

// Use the type in scripts
// VECTOR<INT, 2> matrix;  // 2D matrix of integers
```

## Resource Consumption

Operations on VECTOR objects consume chronons based on the operation complexity and the size of the vector:

- **Basic operations** (get, set): Minimal chronon cost
- **Slicing, reshaping**: Cost scales with log of vector size
- **Matrix multiplication**: Cost scales with O(n³) for n×n matrices
- **Element-wise operations**: Cost scales linearly with vector size
- **Temporal operations**: Higher cost, scaling with vector size

## Performance Considerations

### Memory Efficiency

- The `MultiDimensionalVector` uses a flat internal representation for efficient storage
- Reshaping and slicing operations reuse data when possible
- Temporal history is stored efficiently, only saving changes between states

### Computational Efficiency

- Matrix operations are optimized for performance
- Element-wise operations and mapping use efficient algorithms
- Resource costs are scaled to be reasonable for large vectors

### Thread Safety

- Vector operations are not inherently thread-safe
- For concurrent access, external synchronization is required
- The temporal runtime tracks resource usage safely across threads

## Examples

For comprehensive examples of using the VECTOR type, refer to:
- `examples/vector_demo.cpp`: Demonstrates all major features
- `tests/multi_dimensional_vector_test.cpp`: Shows detailed usage patterns

## API Reference

### MultiDimensionalVector

#### Constructors

- `MultiDimensionalVector(const std::vector<size_t>& dimensions, std::shared_ptr<TemporalRuntime> runtime)`
- `MultiDimensionalVector(std::shared_ptr<ChronovyanVector> flatVector, const std::vector<size_t>& dimensions, std::shared_ptr<TemporalRuntime> runtime)`

#### Basic Methods

- `size_t getDimensionCount() const`
- `const std::vector<size_t>& getDimensions() const`
- `size_t getTotalSize() const`
- `const Value& at(const std::vector<size_t>& indices) const`
- `Value& at(const std::vector<size_t>& indices)`
- `void set(const std::vector<size_t>& indices, const Value& value)`
- `std::string toString() const`

#### Matrix and Tensor Operations

- `std::shared_ptr<MultiDimensionalVector> slice(size_t dimension, size_t index) const`
- `bool reshape(const std::vector<size_t>& newDimensions)`
- `std::shared_ptr<MultiDimensionalVector> transpose() const`
- `std::shared_ptr<MultiDimensionalVector> matrixMultiply(const MultiDimensionalVector& other) const`
- `std::shared_ptr<MultiDimensionalVector> elementWise(const MultiDimensionalVector& other, std::function<Value(const Value&, const Value&)> op) const`
- `std::shared_ptr<MultiDimensionalVector> map(std::function<Value(const Value&)> func) const`
- `std::shared_ptr<MultiDimensionalVector> reduce(size_t dimension, std::function<Value(const Value&, const Value&)> func, const Value& initial) const`

#### Temporal Methods

- `Value getAtCycle(const std::vector<size_t>& indices, int cycle) const`
- `void saveHistoryCheckpoint(int cycle)`
- `bool restoreFromCycle(int cycle)`
- `std::shared_ptr<MultiDimensionalVector> temporalTransform(int startCycle, int endCycle, std::function<Value(const Value&, const Value&, double)> transformFunc) const`
- `std::shared_ptr<MultiDimensionalVector> temporalGradient(int startCycle, int endCycle) const`
- `std::shared_ptr<MultiDimensionalVector> predictFutureState(int futureCycle, const std::vector<int>& historicalCycles) const`

## Future Enhancements

Future versions of the VECTOR type may include:

1. **Advanced numerical operations**: Eigenvalue/eigenvector calculation, SVD, LU decomposition
2. **GPU acceleration**: Offloading computations to GPU for large matrices
3. **Sparse matrix support**: Efficient storage for sparse data
4. **Neural network operations**: Specialized functions for ML applications
5. **Integration with advanced visualization**: Real-time plotting and visualization
6. **Parallel processing**: Multi-threaded operations for large vectors

## See Also

- [ChronovyanVector](./chronovyan_vector.md): Documentation for the underlying 1D vector implementation
- [Temporal Runtime](./temporal_runtime.md): Details on resource tracking and temporal features
- [Custom Type System](./custom_type_system.md): How types are defined and registered in Chronovyan