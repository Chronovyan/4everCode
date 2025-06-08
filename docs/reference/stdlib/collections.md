---
title: Collections
description: Data structures and algorithms
weight: 20
---

# Collections Module

## Overview

The `std.collections` module provides efficient implementations of common data structures and algorithms.

## Core Types

### `Vector<T>`
A dynamic array that can grow and shrink in size.

```chronovyan
DECLARE vec = Vector<INT>();
vec.push(1);
vec.push(2);
PRINT(vec[0]);  // 1
```

### `HashMap<K, V>`
A hash table implementation with O(1) average case operations.

```chronovyan
DECLARE map = HashMap<STRING, INT>();
map["one"] = 1;
PRINT(map["one"]);  // 1
```

### `HashSet<T>`
A set implementation based on hashing.

```chronovyan
DECLARE set = HashSet<STRING>();
set.insert("apple");
PRINT(set.contains("apple"));  // true
```

### `LinkedList<T>`
A doubly-linked list.

```chronovyan
DECLARE list = LinkedList<INT>();
list.push_back(1);
list.push_front(0);
```

## Algorithms

### Sorting

#### `SORT(collection, compare: FUNCTION = (a, b) -> a <=> b)`
Sorts a collection in-place.

```chronovyan
DECLARE nums = [3, 1, 4, 1, 5, 9];
SORT(nums);
// nums is now [1, 1, 3, 4, 5, 9]
```

### Searching

#### `BINARY_SEARCH(collection, value) -> OPTION<INT>`
Performs binary search on a sorted collection.

```chronovyan
DECLARE nums = [1, 2, 3, 4, 5];
DECLARE index = BINARY_SEARCH(nums, 3);  // SOME(2)
```

## Iterators

### Basic Usage

```chronovyan
DECLARE vec = Vector<INT>([1, 2, 3]);

// Using for-in loop
FOR item IN vec {
    PRINT(item);
}

// Using iterator directly
DECLARE iter = vec.iter();
WHILE (LET SOME(item) = iter.next()) {
    PRINT(item);
}
```

### Iterator Methods

- `map(f: FUNCTION) -> Iterator`
- `filter(predicate: FUNCTION) -> Iterator`
- `take(n: INT) -> Iterator`
- `skip(n: INT) -> Iterator`
- `collect() -> Collection`

## Example: Word Frequency

```chronovyan
IMPORT std.collections as coll;

FUNC MAP<STRING, INT> word_frequency(STRING text) {
    DECLARE freq = coll.HashMap<STRING, INT>();
    DECLARE words = text.split("\\s+");
    
    FOR word IN words {
        LET count = freq.get(word).unwrap_or(0);
        freq[word] = count + 1;
    }
    
    RETURN freq;
}
```

## Performance Characteristics

| Operation       | Vector | HashMap | HashSet | LinkedList |
|----------------|--------|---------|---------|------------|
| Indexing       | O(1)   | N/A     | N/A     | O(n)       |
| Search by key  | O(n)   | O(1)    | O(1)    | O(n)       |
| Insertion      | O(1)*  | O(1)    | O(1)    | O(1)       |
| Deletion       | O(n)   | O(1)    | O(1)    | O(1)       |

*Amortized constant time for push_back

## Memory Usage

- `Vector<T>`: Contiguous memory allocation
- `HashMap<K,V>`: Open addressing with Robin Hood hashing
- `HashSet<T>`: Wrapper around `HashMap<T, ()>`
- `LinkedList<T>`: Node-based allocation

## Thread Safety

All collections are thread-safe for concurrent reads, but require external synchronization for concurrent modifications.

## Common Patterns

### Caching with LRU

```chronovyan
IMPORT std.collections as coll;

STRUCT LRUCache<K, V> {
    capacity: INT,
    cache: coll.LinkedHashMap<K, V>,
    
    FUNC NEW(capacity: INT) -> Self {
        RETURN Self {
            capacity: capacity,
            cache: coll.LinkedHashMap::new()
        };
    }
    
    FUNC get(&self, key: &K) -> OPTION<&V> {
        // Move to front if exists
        self.cache.get_refresh(key)
    }
    
    FUNC put(&mut self, key: K, value: V) {
        IF (self.cache.len() >= self.capacity) {
            // Remove least recently used
            self.cache.pop_front();
        }
        self.cache.insert(key, value);
    }
}
```

### Graph Traversal

```chronovyan
IMPORT std.collections as coll;

FUNC MAP<INT, VECTOR<INT>> build_graph(edges: VECTOR<(INT, INT)>) -> MAP<INT, VECTOR<INT>> {
    DECLARE graph = coll.HashMap<INT, VECTOR<INT>>::new();
    
    FOR (u, v) IN edges {
        graph.entry(u).or_insert(VECTOR::new()).push(v);
        // For undirected graph
        graph.entry(v).or_insert(VECTOR::new()).push(u);
    }
    
    RETURN graph;
}

FUNC VECTOR<INT> bfs(graph: &MAP<INT, VECTOR<INT>>, start: INT) -> VECTOR<INT> {
    DECLARE visited = coll.HashSet::new();
    DECLARE queue = coll.VecDeque::new();
    DECLARE result = VECTOR::new();
    
    queue.push_back(start);
    visited.insert(start);
    
    WHILE (LET SOME(node) = queue.pop_front()) {
        result.push_back(node);
        
        IF (LET SOME(neighbors) = graph.get(&node)) {
            FOR &neighbor IN neighbors {
                IF (!visited.contains(&neighbor)) {
                    visited.insert(neighbor);
                    queue.push_back(neighbor);
                }
            }
        }
    }
    
    RETURN result;
}
```

## Performance Optimization

### Pre-allocation

```chronovyan
// Pre-allocate with known capacity
DECLARE vec = VECTOR<INT>::with_capacity(1000);

// For HashMaps with known size
DECLARE map = coll.HashMap::with_capacity(100);
```

### Choosing the Right Collection

| Use Case | Recommended Collection | Notes |
|----------|----------------------|-------|
| Fast lookups by key | `HashMap` | O(1) average case |
| Sorted iteration | `BTreeMap` | O(log n) operations |
| FIFO queue | `VecDeque` | Efficient push/pop both ends |
| Unique elements | `HashSet` | Based on `HashMap` |
| Priority queue | `BinaryHeap` | Max-heap by default |

## Related

- [Algorithms](./algorithms.md) - For more advanced algorithms
- [Concurrency](./concurrency.md) - For thread-safe collections
- [String](./string.md) - For string-specific collections

## Best Practices

1. **Choose the right data structure** for your access patterns
2. **Pre-allocate** when size is known in advance
3. **Use entry API** for efficient updates
4. **Consider memory layout** for cache efficiency
5. **Benchmark** different approaches

## See Also

- [Standard Library](../stdlib/index.md) - Overview of all standard library modules
- [Chronovyan Language Specification](reference/language/specification.md) - For built-in collection types
