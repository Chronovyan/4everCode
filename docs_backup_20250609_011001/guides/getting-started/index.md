# Getting Started with Chronovyan

Welcome to Chronovyan! This guide will help you get up and running with the language quickly.

## Table of Contents

1. [Installation](#installation)
2. [Your First Program](#your-first-program)
3. [Language Tour](#language-tour)
4. [Next Steps](#next-steps)
5. [Getting Help](#getting-help)

## Installation

### Prerequisites

- **Operating System**: Windows, macOS, or Linux
- **Memory**: 4GB RAM minimum, 8GB recommended
- **Disk Space**: 2GB free space

### Installation Steps

#### Windows

1. Download the installer from [our releases page](https://github.com/Chronovyan/Chronovyan/releases)
2. Run the installer and follow the on-screen instructions
3. Open a new command prompt and verify the installation:
   ```powershell
   chronovyan --version
   ```

#### macOS

Using Homebrew:

```bash
brew tap chronovyan/chronovyan
brew install chronovyan
```

Or download the package directly from our [releases page](https://github.com/Chronovyan/Chronovyan/releases).

#### Linux

For Debian/Ubuntu:

```bash
curl -sSL https://chronovyan.org/install.sh | sh
```

For other distributions, download the appropriate package from our [releases page](https://github.com/Chronovyan/Chronovyan/releases).

### Verify Installation

```bash
chronovyan --version
```

You should see output similar to:
```
chronovyan 1.0.0 (a1b2c3d4)
```

## Your First Program

1. Create a new file called `hello.chrono` with the following content:

```chronovyan
// This is a comment
FUNC main() {
    // Print to the console
    println("Hello, Chronovyan!");
    
    // Variables and types
    LET name = "World";
    LET count = 42;
    LET is_awesome = true;
    
    // String interpolation
    println("Hello, {name}! The answer is {count}.");
    
    // Conditional logic
    IF is_awesome {
        println("Chronovyan is awesome!");
    } ELSE {
        println("This should never happen!");
    }
    
    // Loops
    FOR i IN 1..=3 {
        println("Counting: {i}");
    }
    
    // Functions
    FUNC greet(name: STRING) -> STRING {
        RETURN "Hello, {name}!";
    }
    
    println(greet("Developer"));
}
```

2. Run the program:

```bash
chronovyan run hello.chrono
```

You should see the following output:

```
Hello, Chronovyan!
Hello, World! The answer is 42.
Chronovyan is awesome!
Counting: 1
Counting: 2
Counting: 3
Hello, Developer!
```

## Language Tour

### Variables and Constants

```chronovyan
// Mutable variable
LET mut counter = 0;
counter += 1;

// Constant
CONST MAX_USERS = 100;

// Type annotations (usually optional)
LET name: STRING = "Alice";
LET age: INT = 30;
```

### Control Flow

```chronovyan
// If-else
LET number = 7;

IF number < 5 {
    println("Less than five");
} ELSE IF number < 10 {
    println("Less than ten");
} ELSE {
    println("Ten or more");
}

// Loops
LET mut count = 0;
WHILE count < 3 {
    println("Count: {count}");
    count += 1;
}

// For loop with range
FOR i IN 0..5 {
    println("Number: {i}");
}

// Pattern matching
MATCH number {
    1 => println("One"),
    2 | 3 | 5 | 7 => println("Prime"),
    4 | 6 | 8 | 9 => println("Not prime"),
    _ => println("Out of range"),
}
```

### Functions

```chronovyan
// Function with parameters and return type
FUNC add(a: INT, b: INT) -> INT {
    RETURN a + b;
}

// Expression-based function (no return keyword needed)
FUNC multiply(a: INT, b: INT) -> INT { a * b }

// Function with default parameters
FUNC greet(name: STRING, greeting: STRING = "Hello") -> STRING {
    RETURN "{greeting}, {name}!";
}

// Higher-order function
FUNC apply_twice<T>(f: FUNC(T) -> T, x: T) -> T {
    f(f(x))
}

// Using the function
LET result = apply_twice(|x| x * 2, 5);  // Returns 20
```

### Data Structures

```chronovyan
// Structs
STRUCT Point {
    x: FLOAT,
    y: FLOAT,
}

// Methods
IMPL Point {
    FUNC new(x: FLOAT, y: FLOAT) -> Self {
        Self { x, y }
    }
    
    FUNC distance(&self, other: &Point) -> FLOAT {
        LET dx = self.x - other.x;
        LET dy = self.y - other.y;
        (dx * dx + dy * dy).sqrt()
    }
}

// Enums
ENUM WebEvent {
    PageLoad,
    KeyPress(CHAR),
    Click { x: INT, y: INT },
}

// Using the enum
LET event = WebEvent::Click { x: 10, y: 20 };

MATCH event {
    WebEvent::PageLoad => println("page loaded"),
    WebEvent::KeyPress(c) => println!("pressed '{c}'"),
    WebEvent::Click { x, y } => println!("clicked at x={x}, y={y}"),
}
```

### Collections

```chronovyan
// Vector (dynamic array)
LET mut numbers = VEC![1, 2, 3];
numbers.push(4);

// Iteration
FOR num IN &numbers {
    println!("Number: {num}");
}

// Functional style
LET sum = numbers.iter().sum();
LET doubled: VEC<_> = numbers.iter().map(|x| x * 2).collect();

// Hash map
USE std.collections as coll;
LET mut scores = coll::HashMap::new();
scores.insert("Blue", 10);
scores.insert("Red", 20);

// String
LET mut s = String::from("Hello");
s.push_str(", world!");
println!("{s}");  // Prints: Hello, world!
```

### Error Handling

```chronovyan
// Result type for recoverable errors
FUNC divide(a: INT, b: INT) -> RESULT<FLOAT, STRING> {
    IF b == 0 {
        RETURN ERR("Cannot divide by zero");
    }
    OK(a as FLOAT / b as FLOAT)
}

// Using the function
MATCH divide(10, 2) {
    OK(result) => println!("Result: {result}"),
    ERR(e) => println!("Error: {e}"),
}

// Using the ? operator for error propagation
FUNC calculate() -> RESULT<FLOAT, STRING> {
    LET result = divide(10, 0)?;
    OK(result * 2.0)
}
```

## Next Steps

### Learning Path

1. **Basic Concepts**
   - Variables and types
   - Control flow
   - Functions and modules

2. **Intermediate Topics**
   - Structs and enums
   - Error handling
   - Collections
   - Testing

3. **Advanced Topics**
   - Concurrency
   - Metaprogramming
   - FFI (Foreign Function Interface)
   - Performance optimization

### Recommended Resources

- [Language Specification](reference/language/specification.md)
- [Standard Library Reference](../reference/stdlib/README.md)
- [Standard Library Guide](../guides/standard_library_guide.md)
- [Examples Directory](../examples/)

## Getting Help

### Documentation

- [Official Documentation](https://chronovyan.org/docs)
- [API Reference](https://chronovyan.org/api)
- [Examples](https://github.com/Chronovyan/examples)

### Community

- [GitHub Discussions](https://github.com/Chronovyan/Chronovyan/discussions)
- [Discord Server](https://discord.gg/chronovyan)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/chronovyan)

### Reporting Issues

Found a bug or have a feature request? Please file an issue on our [GitHub repository](https://github.com/Chronovyan/Chronovyan/issues).

---

© 2023 Chronovyan Project. All rights reserved.
