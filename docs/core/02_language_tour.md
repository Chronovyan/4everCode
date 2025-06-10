---
title: 'Language Tour: A Quick Introduction to 4ever'
description: 'A quick tour of 4ever syntax and features'
weight: 110
draft: false
---

# Language Tour: A Quick Introduction to 4ever

> **Version**: 1.0.0  
> **Last Updated**: 2025-06-09  
> **Status**: Active

## Introduction

Welcome to 4ever! This document provides a quick tour of the language's syntax and core features through practical examples.

## Hello, 4ever!

```4ever
temporal_program {
    name: "Hello World";
    type: standard;
    
    // Entry point
    main: () -> Void {
        print("Hello, 4ever!");
    }
}
```

## Variables and Types

### Conformist Variables
```4ever
CONF var name: String = "Alice";
CONF var age: Int = 30;
CONF var isActive: Bool = true;
```

### Rebel Variables
```4ever
REB var response: Dynamic = get_user_input();
REB var state: SystemState = SystemState::INITIALIZING;
```

## Control Flow

### Temporal If-Else
```4ever
CONF var temperature: Float = 22.5;

branch (temperature > 30.0) {
    print("It's hot!");
} else (temperature > 20.0) {
    print("It's pleasant!");
} else {
    print("It's cold!");
}
```

### Temporal Loops
```4ever
// Standard for loop
for (CONF var i = 0; i < 10; i++) {
    print("Count: " + i);
}

// Temporal while loop
CONF var counter = 0;
while (counter < 5) {
    print("Counter: " + counter);
    counter++;
}
```

## Functions

### Function Definition
```4ever
// A simple function
fn add(a: Int, b: Int): Int {
    return a + b;
}

// Function with temporal context
fn process_with_retry(data: Data, attempts: Int): Result {
    CONF var result: Result;
    
    temporal_loop (CONF var i = 0; i < attempts; i++) {
        result = try_process(data);
        if (result.success) {
            break;
        }
        wait(1000); // Wait 1 second before retry
    }
    
    return result;
}
```

## Types and Structures

### Struct Definition
```4ever
struct User {
    CONF var id: Int;
    CONF var name: String;
    REB var lastActive: Timestamp;
    
    // Constructor
    new(id: Int, name: String) {
        this.id = id;
        this.name = name;
        this.lastActive = now();
    }
    
    // Method
    fn greet(): String {
        return "Hello, " + this.name + "!";
    }
}
```

## Temporal Features

### Timeline Branching
```4ever
CONF var decisionPoint = analyze_situation();

// Create two potential timelines
branch (decisionPoint > 0.7) {
    // High probability path
    execute_optimistic_strategy();
} else {
    // Fallback path
    execute_conservative_strategy();
}
```

### Temporal Variables
```4ever
temporal var userSession: Session;

// This variable's value can differ across timelines
userSession = create_session();

// Access the value in the current timeline
CONF var currentSession = userSession.resolve();
```

## Error Handling

### Temporal Try-Catch
```4ever
temporal_try {
    let result = risky_operation();
    print("Success: " + result);
} catch (error: TemporalError) {
    print("Temporal anomaly detected: " + error.message);    
} catch (error: Error) {
    print("Error: " + error.message);
}
```

## Concurrency

### Parallel Execution
```4ever
// Execute operations in parallel across timelines
parallel {
    // These blocks execute concurrently
    {
        let data = fetch_data();
        process_data(data);
    }
    {
        let config = load_config();
        update_system(config);
    }
}
```

## Next Steps

1. [Variables](../core/03_variables.md) - Deep dive into 4ever's variable system
2. [Control Flow](../core/04_control_flow.md) - Understanding temporal control structures
3. [Standard Library](../stdlib/00_overview.md) - Explore built-in functionality

---
*Document Revision: 1.0.0*  
*Last Updated: 2025-06-09*
