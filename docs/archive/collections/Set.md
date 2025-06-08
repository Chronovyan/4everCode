---
title: Set Collection
description: Documentation for collections\Set.md
weight: 120
draft: true
---

# Set Collection

The `Set` collection represents an unordered collection of unique elements. It provides methods for performing mathematical set operations like union, intersection, and difference. Sets are particularly useful when you need to store unique values and perform membership tests efficiently.

## Creating a Set

```chronovyan
// Create an empty set
let emptySet = Set();

// Create a set with initial values
let numbers = Set([1, 2, 3, 4, 5]);
let names = Set(["Alice", "Bob", "Charlie"]);

// Sets only store unique values
let uniqueNumbers = Set([1, 2, 2, 3, 3, 3]); // Results in {1, 2, 3}
```

## Type Safety

Sets in Chronovyan are type-flexible but will enforce type consistency during operations:

```chronovyan
let mixedSet = Set([1, "two", 3.0]); // Allowed
let numberSet = Set([1, 2, 3]);
// numberSet.add("four"); // This would cause a type error in strict mode
```

## Basic Operations

### Adding Elements
```chronovyan
let set = Set();
set.add(1);           // Add a single element
set.addAll([2, 3]);  // Add multiple elements
set.add(1);          // Duplicate elements are ignored

// Adding returns the set for method chaining
set.add(4).add(5).add(6);
```

### Removing Elements
```chronovyan
let set = Set([1, 2, 3, 4]);
set.remove(1);        // Remove an element
let wasRemoved = set.delete(2); // Returns true if element was present and removed
set.clear();          // Remove all elements

// Safe removal that won't throw if element doesn't exist
set.removeIfExists(99);
```

### Checking Contents
```chronovyan
let set = Set([1, 2, 3]);

// Check for element existence
if (set.has(1)) {
    // Element exists
}

// Get set size
let size = set.size();  // Returns 3

// Check if set is empty
let isEmpty = set.isEmpty(); // Returns false

// Count elements matching a condition
let count = set.count(fn(x) { return x > 1; }); // Returns 2
```

## Set Operations

### Union
Combines elements from both sets:
```chronovyan
let set1 = Set([1, 2, 3]);
let set2 = Set([3, 4, 5]);
let union1 = set1.unionWith(set2); // {1, 2, 3, 4, 5}

// Alternative syntax
let union2 = set1 | set2; // Same as unionWith

// Union with multiple sets
let set3 = Set([5, 6, 7]);
let union3 = set1.unionWith(set2, set3); // {1, 2, 3, 4, 5, 6, 7}
```

### Intersection
Gets common elements:
```chronovyan
let common = set1.intersect(set2); // {3}

// Alternative syntax
let common2 = set1 & set2; // Same as intersect

// Intersection with multiple sets
let common3 = set1.intersect(set2, Set([3, 4])); // {3}
```

### Difference
Gets elements in first set but not in second:
```chronovyan
let diff1 = set1.difference(set2); // {1, 2}

// Alternative syntax
let diff2 = set1 - set2; // Same as difference

// Symmetric difference (elements in either set but not both)
let symDiff = set1.symmetricDifference(set2); // {1, 2, 4, 5}
```

### Subset/Superset Checks
```chronovyan
let smallSet = Set([1, 2]);
let largeSet = Set([1, 2, 3, 4]);

// Subset checks
let isSubset = smallSet.isSubsetOf(largeSet); // true
let isStrictSubset = smallSet.isStrictSubsetOf(largeSet); // true

// Superset checks
let isSuperset = largeSet.isSupersetOf(smallSet); // true
let isStrictSuperset = largeSet.isStrictSupersetOf(smallSet); // true

// Disjoint check (no common elements)
let noOverlap = Set([1, 2]).isDisjointWith(Set([3, 4])); // true
```

## Conversion and Transformation

### To Array
```chronovyan
let set = Set([1, 2, 3]);
let array = set.toArray(); // [1, 2, 3] (order not guaranteed)

// To get sorted array
let sortedArray = set.sorted().toArray();
```

### Mapping and Transforming
```chronovyan
// Map elements to new values
let doubled = set.map(fn(x) { return x * 2; });

// FlatMap for nested structures
let nested = Set([[1, 2], [3, 4]]);
let flat = nested.flatMap(fn(x) { return x; }); // Set([1, 2, 3, 4])

// Convert to object with values as keys
let obj = set.toObject(fn(x) { return ["key" + x, x]; });
```

## Iteration and Querying

### forEach
```chronovyan
let set = Set([1, 2, 3]);

// Basic iteration
set.forEach(fn(element) {
    print(element);
});

// With index (position in iteration order)
set.forEach(fn(element, index) {
    print(`Element at ${index}: ${element}`);
});
```

### Filtering and Finding
```chronovyan
let numbers = Set([1, 2, 3, 4, 5]);

// Basic filter
let evens = numbers.filter(fn(x) { return x % 2 == 0; });

// Find first matching element
let firstEven = numbers.first(fn(x) { return x % 2 == 0; }); // 2 or 4 (order not guaranteed)


// Check if any/all elements match condition
let hasEven = numbers.any(fn(x) { return x % 2 == 0; }); // true
let allEven = numbers.all(fn(x) { return x % 2 == 0; }); // false
```

### Reducing
```chronovyan
// Sum all elements
let sum = numbers.reduce(0, fn(acc, x) { return acc + x; });

// Join elements with separator
let joined = numbers.map(String).reduce("", fn(acc, x) { 
    return acc ? "${acc}, ${x}" : x; 
});
```

## Performance Characteristics

### Time Complexity
- **Add/Remove/Contains**: O(1) average case, O(n) worst case (due to potential hash collisions)
- **Union/Intersection/Difference**: O(n + m) where n and m are the sizes of the sets
- **Iteration**: O(n)
- **Size**: O(1)


### Memory Usage
- Approximately O(n) where n is the number of elements
- Overhead is higher than Array due to hash table implementation

### Optimization Tips
1. Pre-allocate with expected size if known:
   ```chronovyan
   // More efficient for large sets
   let largeSet = Set.withCapacity(1000);
   ```
2. Use `reserve()` to pre-allocate space for known growth
3. Consider using `Array` if:
   - You need ordered elements
   - You need to access elements by index
   - Memory usage is a concern and you don't need set operations

## Common Use Cases

### 1. Removing Duplicates
```chronovyan
let array = [1, 2, 2, 3, 3, 3];
let unique = Set(array).toArray(); // [1, 2, 3] (order not preserved)
```

### 2. Membership Testing
```chronovyan
let validStatuses = Set(["active", "pending", "completed"]);
if (validStatuses.has(userStatus)) {
    // Valid status
}
```

### 3. Tagging System
```chronovyan
let userTags = Set(["programming", "javascript"]);
let postTags = Set(["javascript", "web"]);

// Find common tags
let commonTags = userTags.intersect(postTags);

// Find posts with any matching tag
let hasMatchingTag = !userTags.intersect(postTags).isEmpty();
```

### 4. Graph Algorithms
```chronovyan
// Track visited nodes in graph traversal
let visited = Set();

function dfs(node) {
    if (visited.has(node)) return;
    
    visited.add(node);
    // Process node
    node.neighbors.forEach(dfs);
}
```

### 5. Event Handlers
```chronovyan
let pressedKeys = Set();

window.onKeyDown(fn(event) {
    pressedKeys.add(event.key);
});

window.onKeyUp(fn(event) {
    pressedKeys.delete(event.key);
});

// Check for key combinations
if (pressedKeys.has("Control") && pressedKeys.has("s")) {
    // Save document
}
```

## Best Practices

1. **Immutability**: Consider creating new sets instead of modifying existing ones:
   ```chronovyan
   // Instead of:
   set1.addAll(set2);
   
   // Prefer:
   let combined = set1.unionWith(set2);
   ```

2. **Type Safety**: Be mindful of type coercion in sets:
   ```chronovyan
   let set = Set([1, "1"]); // Contains two different elements
   ```

3. **Large Datasets**: For very large sets, consider using specialized data structures:
   ```chronovyan
   // Use Bloom filters for approximate membership testing
   // Use BitSets for dense integer sets
   ```

4. **Performance Testing**: Profile set operations in performance-critical code, as actual performance can vary based on:
   - Hash distribution of elements
   - Load factor of the underlying hash table
   - Memory access patterns

## See Also

- [Array Documentation](./Array.md) - For ordered collections with index-based access
- [Map Documentation](./Map.md) - For key-value pair collections
- [Performance Guide](../guides/performance.md) - For optimizing collection usage
- [Standard Library Reference](../reference/) - For complete API documentation

## Changelog

### v1.0.0
- Initial implementation of Set collection
- Added basic set operations (union, intersection, difference)
- Added iteration and transformation methods

### v1.1.0
- Added method chaining support
- Added symmetric difference operation
- Improved performance for large sets

## Contributing

Found an issue or have a suggestion? Please open an issue or submit a pull request on our [GitHub repository](https://github.com/yourorg/chronovyan).