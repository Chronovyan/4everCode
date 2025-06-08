---
title: Testing Module
description: Unit testing framework
weight: 100
---

# Testing Module

## Overview

The `std.testing` module provides a comprehensive framework for writing and running unit tests in Chronovyan. It includes assertions, test fixtures, and test runners to help you ensure your code works as expected.

## Writing Tests

### Basic Test Case

```chronovyan
IMPORT std.testing as test;

TEST "addition works" {
    test.assert_eq(2 + 2, 4, "Basic arithmetic should work");
}
```

### Test Suites

```chronovyan
TEST_SUITE "Math Operations" {
    TEST "addition" {
        test.assert_eq(1 + 1, 2);
    }
    
    TEST "subtraction" {
        test.assert_eq(3 - 1, 2);
    }
}
```

## Assertions

### Basic Assertions

```chronovyan
// Assert that a condition is true
test.assert(1 + 1 == 2, "Math is broken!");

// Assert equality
test.assert_eq(1 + 1, 2);

// Assert inequality
test.assert_ne(1 + 1, 3);

// Assert approximate equality (for floating point)
test.assert_approx_eq(0.1 + 0.2, 0.3, 0.0001);
```

### Collection Assertions

```chronovyan
DECLARE numbers = [1, 2, 3];

test.assert_contains(numbers, 2);
test.assert_not_contains(numbers, 42);
test.assert_empty([]);
```

### Exception Assertions

```chronovyan
// Assert that a function throws an exception
test.assert_throws<DivisionByZeroError>(\() {
    RETURN 1 / 0;
});

// Assert specific exception message
test.assert_throws_with_message<ValueError>("Invalid value", \() {
    THROW ValueError("Invalid value");
});
```

## Test Fixtures

### Setup and Teardown

```chronovyan
TEST_SUITE "Database Tests" {
    DECLARE db = NULL;
    
    SETUP {
        // Run before each test
        db = Database.connect(":memory:");
    }
    
    TEARDOWN {
        // Run after each test
        IF (db != NULL) {
            db.close();
        }
    }
    
    TEST "can insert record" {
        db.execute("INSERT INTO users (name) VALUES ('Alice')");
        test.assert_eq(db.row_count(), 1);
    }
}
```

## Running Tests

### Command Line

```bash
chronovyan test path/to/test_file.cvy
```

### Test Discovery

Tests are discovered based on naming conventions:
- Files matching `*_test.cvy`
- Functions starting with `test_`
- Classes inheriting from `TestCase`
- Methods starting with `test_` in test classes

## Test Output

### Verbose Output

```bash
chronovyan test --verbose
```

Example output:
```
Running 3 tests from test_math.cvy
[PASS] test_addition (0.0002s)
[PASS] test_subtraction (0.0001s)
[FAIL] test_division (0.0001s)
  Assertion failed: expected 0.33333, got 0.30000
  at test_math.cvy:15

Test Results: 2 passed, 1 failed, 0 skipped
```

## Code Coverage

Generate coverage reports:

```bash
chronovyan test --coverage
```

This generates a coverage report in the `coverage/` directory.

## Mocking

### Creating Mocks

```chronovyan
TEST "mock database" {
    // Create a mock object
    DECLARE mock_db = test.mock<Database>();
    
    // Setup expectations
    test.expect(mock_db.query, "SELECT * FROM users")
        .returns([{"id": 1, "name": "Alice"}]);
    
    // Run test
    DECLARE result = get_user_name(mock_db, 1);
    
    // Verify
    test.assert_eq(result, "Alice");
    test.verify(mock_db);  // Check all expectations were met
}
```

## Property-Based Testing

### Defining Properties

```chronovyan
PROPERTY "addition is commutative" (a: INT, b: INT) -> BOOL {
    RETURN a + b == b + a;
}

PROPERTY "sorting is idempotent" (list: VECTOR<INT>) -> BOOL {
    DECLARE sorted1 = list.sorted();
    DECLARE sorted2 = sorted1.sorted();
    RETURN sorted1 == sorted2;
}
```

## Example: Testing a Stack

```chronovyan
IMPORT std.testing as test;

FUNC TEST test_stack_operations() {
    DECLARE stack = Stack<INT>();
    
    // Test push and size
    stack.push(1);
    test.assert_eq(stack.size(), 1);
    
    // Test peek
    test.assert_eq(stack.peek(), 1);
    
    // Test pop
    test.assert_eq(stack.pop(), 1);
    test.assert(stack.is_empty());
    
    // Test underflow
    test.assert_throws<EmptyStackError>(\() { stack.pop(); });
}

// Run all tests in this file
TEST_MAIN();
```

## Performance Testing

### Benchmarking

```chronovyan
BENCHMARK "fibonacci(30)" {
    // This block will be timed
    RETURN fibonacci(30);
}

BENCHMARK "fibonacci_memoized(30)" {
    RETURN fibonacci_memoized(30);
}
```

Run with:
```bash
chronovyan test --benchmark
```

## Best Practices

1. **Test Naming**: Use descriptive names that explain what's being tested
2. **One Assertion Per Test**: Each test should verify one behavior
3. **Arrange-Act-Assert**: Structure tests in three clear sections
4. **Test Edge Cases**: Include boundary conditions and error cases
5. **Keep Tests Fast**: Tests should run quickly to encourage frequent testing
6. **Isolate Tests**: Tests should be independent and not rely on shared state
7. **Use Mocks**: For external dependencies, use mocks to isolate the code under test

## Common Patterns

### Test Fixtures

```chronovyan
IMPORT std.testing as test;

// Shared test data
STRUCT TestDB {
    conn: DatabaseConnection,
}

IMPL TestDB {
    FUNC new() -> Self {
        LET conn = Database::connect(":memory:")
            .expect("Failed to create test database");
        Self { conn }
    }
    
    FUNC create_test_data(&self) {
        self.conn.execute("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT)");
        self.conn.execute("INSERT INTO users (name) VALUES ('test_user')");
    }
}

IMPL Drop for TestDB {
    FUNC drop(&mut self) {
        // Clean up resources
        self.conn.execute("DROP TABLE IF EXISTS users");
    }
}

// Test using the fixture
TEST_FN user_test() {
    // Setup
    LET db = TestDB::new();
    db.create_test_data();
    
    // Test
    LET user = db.conn.query("SELECT name FROM users WHERE id = 1");
    test::assert_eq!(user, "test_user");
    
    // Teardown happens automatically when db goes out of scope
}
```

### Property-Based Testing

```chronovyan
IMPORT std.testing as test;

// A property-based test for list reversal
PROPTEST! {
    // This will be run with many random inputs
    TEST_PROP list_reverse_twice_is_identity(mut list: VEC<i32>) {
        // Make a copy
        LET original = list.clone();
        
        // Reverse twice
        list.reverse();
        list.reverse();
        
        // Should be back to original
        test::assert_eq!(list, original);
    }
    
    // Another property: the length stays the same after reverse
    TEST_PROP reverse_preserves_length(list: VEC<i32>) {
        LET original_len = list.len();
        list.reverse();
        test::assert_eq!(list.len(), original_len);
    }
}
```

## Advanced Testing Techniques

### Testing Async Code

```chronovyan
IMPORT std.testing as test;
IMPORT std.async as async;

TEST_FN async_http_test() {
    // Create a test runtime
    LET rt = async::Runtime::new();
    
    // Run the async test
    rt.block_on(async {
        LET client = HttpClient::new();
        LET response = client.get("http://example.com").await?;
        test::assert_eq!(response.status(), 200);
        OK(())
    }).expect("Test failed");
}
```

### Mocking Dependencies

```chronovyan
IMPORT std.testing as test;

// Define a trait for the dependency
TRAIT Database {
    FUNC get_user(&self, id: i32) -> RESULT<STRING, String>;
}

// Real implementation
STRUCT RealDB;
IMPL Database for RealDB {
    FUNC get_user(&self, id: i32) -> RESULT<STRING, String> {
        // Actual database query
        // ...
    }
}

// Test implementation
STRUCT MockDB {
    users: MAP<i32, STRING>,
}

IMPL MockDB {
    FUNC new() -> Self {
        LET mut users = MAP::new();
        users.insert(1, "test_user".to_string());
        Self { users }
    }
}

IMPL Database for MockDB {
    FUNC get_user(&self, id: i32) -> RESULT<STRING, String> {
        self.users.get(&id).cloned()
            .ok_or_else(|| "User not found".to_string())
    }
}

// Test using the mock
TEST_FN test_user_retrieval() {
    LET db = MockDB::new();
    LET user = db.get_user(1).expect("Failed to get user");
    test::assert_eq!(user, "test_user");
    
    // Test error case
    test::assert!(db.get_user(999).is_err());
}
```

## Performance Testing

### Benchmarking

```chronovyan
IMPORT std.testing.bench as bench;

// A simple benchmark
BENCHMARK_FN sort_benchmark(b: &mut bench::Bencher) {
    LET mut rng = rand::thread_rng();
    
    // Create a vector of random numbers
    b.iter(|| {
        LET mut data: VEC<i32> = (0..1000)
            .map(|_| rng.gen_range(0..1000))
            .collect();
            
        // Time just the sorting
        bench::black_box(data.sort());
    });
}

// Benchmark with setup
BENCHMARK_FN search_benchmark(b: &mut bench::Bencher) {
    // Setup (not measured)
    LET haystack: VEC<_> = (0..1_000_000).collect();
    LET needle = 999_999;
    
    // The actual benchmark
    b.iter(|| {
        test::black_box(haystack.contains(&needle)));
    });
}
```

## Related

- [Standard Library](../stdlib/index.md) - Overview of all standard library modules
- [Concurrency](./concurrency.md) - For testing concurrent code
- [Reflection](./reflection.md) - For runtime type checking and mocking
- [Random](./random.md) - For generating test data

## Best Practices

1. **Write clear test names** that describe the behavior being tested
2. **Test edge cases** and error conditions
3. **Keep tests independent** and isolated
4. **Use meaningful assertions** with descriptive failure messages
5. **Avoid testing implementation details** - focus on behavior
6. **Run tests in parallel** when possible
7. **Measure test coverage** to identify untested code
8. **Keep tests maintainable** - they're production code too

## See Also

- [Chronovyan Language Specification](reference/language/specification.md) - For built-in testing support
- [Test-Driven Development Guide](../guides/tdd.md) - For testing methodology
- [Continuous Integration](../guides/ci.md) - For automated testing in CI/CD pipelines
- [Performance Optimization](../performance.md) - For performance testing and optimization
