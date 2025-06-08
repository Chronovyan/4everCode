---
title: Concurrency
description: Parallel and concurrent programming primitives
weight: 30
---

# Concurrency Module

## Overview

The `std.concurrency` module provides safe and efficient concurrency primitives for Chronovyan, built on top of the language's temporal model.

## Threads

### `Thread`
Lightweight unit of execution.

```chronovyan
FUNC VOID worker() {
    PRINT("Hello from thread");
}

DECLARE handle = THREAD_NEW(worker);
handle.join();
```

### Thread Pools

#### `ThreadPool`
Manages a pool of worker threads.

```chronovyan
DECLARE pool = ThreadPool.new(4);  // 4 worker threads

FUTURE<INT> result = pool.execute(\() -> INT {
    RETURN 42;
});

PRINT(AWAIT result);  // 42
```

## Synchronization

### `Mutex<T>`
Mutual exclusion primitive.

```chronovyan
DECLARE counter = Mutex.new(0);

FUNC VOID increment() {
    LET guard = counter.lock();
    *guard += 1;
}  // lock is automatically released here
```

### `RwLock<T>`
Reader-writer lock.

```chronovyan
DECLARE data = RwLock.new(HashMap<STRING, INT>());

// Multiple readers can access simultaneously
{
    LET reader = data.read();
    IF (reader.contains("key")) {
        PRINT(reader["key"]);
    }
}

// Only one writer at a time
{
    LET mut writer = data.write();
    writer["key"] = 42;
}
```

### `Channel<T>`
Thread-safe message passing.

```chronovyan
DECLARE (tx, rx) = channel();

// Sender
THREAD_NEW(\() {
    tx.send("Hello");
});

// Receiver
PRINT(rx.recv());  // "Hello"
```

## Atomic Operations

### `Atomic<T>`
Atomic operations on primitive types.

```chronovyan
DECLARE counter = Atomic.new(0);
counter.fetch_add(1);
PRINT(counter.load());  // 1
```

## Async/Await

### `FUTURE<T>`
Represents an asynchronous computation.

```chronovyan
FUNC FUTURE<INT> compute() {
    RETURN ASYNC {
        AWAIT sleep(1s);
        RETURN 42;
    };
}

FUNC VOID main() {
    DECLARE result = AWAIT compute();
    PRINT(result);  // 42
}
```

## Example: Producer-Consumer

```chronovyan
IMPORT std.concurrency as conc;

FUNC VOID producer(tx: Sender<INT>) {
    FOR i IN 1..=5 {
        tx.send(i);
    }
}

FUNC VOID consumer(rx: Receiver<INT>) {
    WHILE (LET SOME(msg) = rx.try_recv()) {
        PRINT("Consumed: ", msg);
    }
}

FUNC VOID main() {
    DECLARE (tx, rx) = conc.channel();
    
    DECLARE p = conc.THREAD_NEW(\() { producer(tx); });
    DECLARE c = conc.THREAD_NEW(\() { consumer(rx); });
    
    p.join();
    c.join();
}
```

## Performance Considerations

- Prefer message passing over shared state
- Use `RwLock` when there are many readers and few writers
- `Atomic` operations are lock-free but can still cause contention
- Thread pools reduce thread creation overhead

## Memory Model

Chronovyan's memory model is based on:
1. Sequential consistency for data-race-free programs
2. Happens-before relationships for synchronization
3. Strong memory ordering guarantees for atomic operations

## Common Patterns

### Worker Pool

```chronovyan
IMPORT std.concurrency as conc;

FUNC VOID worker(id: INT, rx: Receiver<STRING>) {
    WHILE (LET SOME(task) = rx.recv()) {
        PRINTF("Worker %d processing: %s\n", id, task);
        // Process task
    }
}

FUNC VOID main() {
    DECLARE (tx, rx) = conc.channel::<STRING>();
    DECLARE workers = VECTOR<Thread>();
    
    // Start worker threads
    FOR i IN 1..=4 {
        workers.push(conc.THREAD_NEW(\() { worker(i, rx.clone()); }));
    }
    
    // Send tasks
    FOR i IN 1..=10 {
        tx.send(format("Task {}", i));
    }
    
    // Close channel and wait for workers
    drop(tx);
    FOR worker IN workers {
        worker.join();
    }
}
```

### Rate Limiting

```chronovyan
IMPORT std.concurrency as conc;
IMPORT std.time as time;

STRUCT RateLimiter {
    semaphore: conc.Semaphore,
    interval: time.Duration,
    last: time.Instant,
    
    FUNC NEW(rate: INT, per: time.Duration) -> Self {
        RETURN Self {
            semaphore: conc.Semaphore::new(rate as UINT),
            interval: per / rate as INT,
            last: time.Instant::now()
        };
    }
    
    FUNC acquire(&mut self) {
        LET now = time.Instant::now();
        LET elapsed = now - self.last;
        
        IF (elapsed < self.interval) {
            time.sleep(self.interval - elapsed);
        }
        
        self.semaphore.acquire();
        self.last = time.Instant::now();
    }
    
    FUNC release(&self) {
        self.semaphore.release();
    }
}
```

## Debugging Concurrent Code

### Deadlock Detection

Chronovyan's runtime includes built-in deadlock detection that can be enabled:

```bash
CHRONOVYAN_DEBUG=deadlock your_program.cvy
```

### Thread Sanitizer

To detect data races:

```bash
CHRONOVYAN_TSAN=1 your_program.cvy
```

## Related

- [Temporal](./temporal.md) - For time-aware concurrency
- [Collections](./collections.md) - For thread-safe data structures
- [Testing](./testing.md) - For concurrent test utilities
- [IO](./io.md) - For asynchronous I/O operations

## Best Practices

1. **Prefer message passing over shared state**
2. **Use higher-level abstractions** when possible (channels, actors)
3. **Minimize lock granularity** to reduce contention
4. **Avoid holding locks** during I/O operations
5. **Use thread-local storage** for thread-specific data
6. **Be mindful of async/await** and potential deadlocks
7. **Profile and measure** concurrent performance

## See Also

- [Standard Library](../stdlib/index.md) - Overview of all standard library modules
- [Chronovyan Language Specification](reference/language/specification.md) - For language-level concurrency features
