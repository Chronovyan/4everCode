# Array Collection

## Overview
Arrays in Chronovyan are fixed-size, contiguous memory containers that provide efficient random access to elements. They are the backbone of many temporal operations and data processing tasks.

## Basic Usage

### Declaration and Initialization
```cpp
// Fixed-size array
chronovyan::Array<int, 5> numbers = {1, 2, 3, 4, 5};

// Array with default initialization
chronovyan::Array<double, 10> values;  // All elements are default-initialized

// Array from C-style array
int raw_array[] = {10, 20, 30};
chronovyan::Array<int, 3> arr(raw_array);
```

### Accessing Elements
```cpp
// Using operator[]
int first = numbers[0];
numbers[1] = 42;

// Bounds-checked access
int value = numbers.at(2);  // Throws std::out_of_range if invalid

// First and last elements
int front = numbers.front();
int back = numbers.back();

// Raw data access
const int* data = numbers.data();
```

## Operations

### Iteration
```cpp
// Range-based for loop
for (const auto& num : numbers) {
    process(num);
}

// Using iterators
for (auto it = numbers.begin(); it != numbers.end(); ++it) {
    process(*it);
}
```

### Algorithms
```cpp
#include <algorithm>

// Sort the array
std::sort(numbers.begin(), numbers.end());

// Find an element
auto it = std::find(numbers.begin(), numbers.end(), 42);
if (it != numbers.end()) {
    // Found the element
}
```

## Performance Characteristics

| Operation | Time Complexity |
|-----------|----------------|
| Access    | O(1)           |
| Search    | O(n)           |
| Iteration | O(n)           |
| Sort      | O(n log n)     |


## Advanced Features

### Multi-dimensional Arrays
```cpp
// 2D array (3x4)
chronovyan::Array<chronovyan::Array<int, 4>, 3> matrix;


// Access elements
matrix[0][0] = 1;
int value = matrix[1][2];
```

### Array Views
```cpp
// Create a view of part of the array
auto sub_view = numbers.view(1, 3);  // Elements at indices 1 and 2

// Modify through view
sub_view[0] = 100;  // Modifies the original array
```

## Best Practices

1. **Prefer Fixed-Size Arrays** when the size is known at compile-time
2. **Use `auto`** with array declarations for better maintainability
3. **Consider `std::array`** for standard C++ compatibility
4. **Use `fill()`** for setting all elements to the same value
   ```cpp
   numbers.fill(0);  // Set all elements to 0
   ```

## Related Collections
- [Vector](Vector.md): Dynamic array with automatic resizing
- [Set](Set.md): Collection of unique elements
- [Map](Map.md): Key-value pair collection

## See Also
- [Chronovyan Collections Overview](../collections/)
- [Performance Optimization](../guides/performance.md)
