---
title: Concurrency Model
description: Understanding concurrency and parallelism in Chronovyan
weight: 20
draft: false
---

# Concurrency in Chronovyan

## Overview

Chronovyan's concurrency model is built on three core principles:

1. **Temporal Safety**: Ensures that concurrent operations maintain temporal consistency
2. **Structured Concurrency**: Child operations complete before their parents
3. **Composition**: Concurrent operations can be combined predictably

## Core Concepts

### 1. Temporal Threads

Temporal threads are lightweight, cooperative tasks that enable concurrent execution while maintaining temporal consistency.

#### Key Features:
- **Cooperative Scheduling**: Threads yield control explicitly with `YIELD`
- **Structured Lifetime**: Child threads complete before their parent
- **Temporal Isolation**: Each thread maintains its own timeline

**Example: Basic Threading**
```chronovyan
// Create a new temporal thread
DECLARE worker = SPAWN {
    // This code runs concurrently
    FOR i IN 1..5 {
        PRINT("Worker: Processing item ", i);
        YIELD;  // Explicitly yield control
    }
    RETURN "Done";
};

// Main thread continues execution
FOR i IN 1..3 {
    PRINT("Main: Working on task ", i);
    YIELD;
}

// Wait for worker to complete and get result
DECLARE result = AWAIT worker;
PRINT("Worker completed with: ", result);
```

### 2. Worker Pools

For CPU-bound work, Chronovyan provides managed worker pools.

**Example: Parallel Processing**
```chronovyan
// Process items in parallel
DECLARE results = PARALLEL_MAP(1..10, \i -> {
    // Simulate CPU-intensive work
    DECLARE sum = 0;
    FOR j IN 1..1_000_000 {
        sum = (sum + i * j) % 1000;
    }
    RETURN sum;
}, max_workers: 4);  // Limit concurrent workers

PRINT("Results: ", results);
```

## Synchronization Primitives

### 1. Temporal Locks

Time-aware mutexes that prevent deadlocks and support timeouts.

**Example: Bank Transfer with Locking**
```chronovyan
// Shared account data
TYPE Account = STRUCT {
    balance: MUTEX<FLOAT>,
    id: INT
};

FUNC BOOL transfer(Account &from, Account &to, FLOAT amount) {
    // Try to acquire both locks with timeout
    LOCK (from.balance, to.balance) WITH_TIMEOUT(1s) {
        IF (from.balance < amount) {
            RETURN FALSE;  // Insufficient funds
        }
        from.balance -= amount;
        to.balance += amount;
        RETURN TRUE;
    } ELSE {
        // Handle timeout
        PRINT("Transfer timed out");
        RETURN FALSE;
    }
}
```

### 2. Channels

Thread-safe communication primitives for message passing.

**Example: Producer-Consumer Pattern**
```chronovyan
// Create a buffered channel
DECLARE ch = CHANNEL<INT>(capacity: 5);

// Producer
DECLARE producer = SPAWN {
    FOR i IN 1..10 {
        // Simulate work
        DELAY 100ms;
        // Send data
        SEND(ch, i);
    }
    CLOSE(ch);  // Signal completion
};

// Consumer
SPAWN {
    FOR VALUE item IN ch {
        PRINT("Processed: ", item);
        DELAY 200ms;  // Simulate processing time
    }
    PRINT("All items processed");
};

AWAIT producer;
```

## Temporal Consistency

### 1. Happens-Before Relationship

Chronovyan enforces a strict happens-before relationship for all operations:

1. **Thread Start**: Operations in the parent thread before `SPAWN` happen before the new thread starts
2. **Thread Join**: All operations in a thread happen before the `AWAIT` completes
3. **Message Passing**: A message send happens before the corresponding receive

### 2. Snapshot Isolation

Transactions see a consistent snapshot of shared state.

**Example: Account Balance Transfer**
```chronovyan
// Start a transaction
TRANSACTION {
    // These operations see a consistent snapshot
    DECLARE from_balance = GET_BALANCE(from_account);
    DECLARE to_balance = GET_BALANCE(to_account);
    
    // Validate
    IF (from_balance < amount) {
        ROLLBACK "Insufficient funds";
    }
    
    // Make changes
    SET_BALANCE(from_account, from_balance - amount);
    SET_BALANCE(to_account, to_balance + amount);
    
    // Commit if all operations succeed
    COMMIT;
} ON_CONFLICT (error) {
    // Handle conflicts
    PRINT("Transaction failed: ", error);
}
```

## Best Practices

### 1. Minimize Shared State

```chronovyan
// Prefer message passing
DECLARE results = CHANNEL<RESULT>();

SPAWN {
    DECLARE result = expensive_computation();
    SEND(results, result);
};

// Instead of shared variables
// DECLARE shared_result;
// SPAWN { shared_result = expensive_computation(); };
```

### 2. Use Structured Concurrency

```chronovyan
WITH_TIMEOUT(5s) {
    // All spawned tasks must complete within 5 seconds
    DECLARE task1 = SPAWN { process_data(data1); };
    DECLARE task2 = SPAWN { process_data(data2); };
    
    AWAIT task1;
    AWAIT task2;
} ON_TIMEOUT {
    // Clean up if timeout occurs
    PRINT("Operation timed out");
}
```

### 3. Error Handling

```chronovyan
TRY {
    DECLARE result = AWAIT some_async_operation();
    PROCESS(result);
} CATCH (e) {
    IF (e IS TimeoutError) {
        HANDLE_TIMEOUT();
    } ELSE {
        LOG_ERROR(e);
        THROW e;  // Re-throw if unhandled
    }
}
```

## Performance Considerations

### 1. Task Granularity

```chronovyan
// Too fine-grained
FOR i IN 1..1000 {
    SPAWN { process_item(i); };  // Overhead of task creation
}

// Better: Process chunks
FOR chunk IN CHUNK(1..1000, size: 100) {
    SPAWN {
        FOR item IN chunk {
            process_item(item);
        }
    };
}
```

### 2. Memory Locality

```chronovyan
// Process data in cache-friendly chunks
FOR chunk IN CHUNK(large_array, size: 1024) {
    PROCESS_CHUNK(chunk);
}
```

## Advanced Topics

### 1. Lock-Free Algorithms

```chronovyan
// Atomic operations
DECLARE counter = ATOMIC<INT>(0);

SPAWN {
    counter.fetch_add(1, ORDERING::RELAXED);
};
```

### 2. Software Transactional Memory (STM)

```chronovyan
// Atomic block
ATOMIC {
    // These operations execute atomically
    DECLARE a = shared_value_a;
    DECLARE b = shared_value_b;
    shared_value_a = a + b;
    shared_value_b = a - b;
};
```

## Next Steps

- [Temporal Programming](temporal_programming.md) - Advanced time-based concurrency
- [Resource Management](../concepts/resource_management.md) - Managing system resources
- [Performance Tuning](../concepts/performance.md) - Optimizing concurrent applications

## See Also

- [Language Specification](reference/language/specification.md) - Complete language reference
- [Standard Library](../../reference/stdlib/index.md) - Built-in concurrency utilities
- [Recipes](../../examples/concurrency) - Practical concurrency patterns
