# Variable Types API

## Overview
The Variable System in Chronovyan provides a flexible and type-safe way to handle variables that can change over time. This document describes the various variable types available and how to use them effectively.

## Core Variable Types

### 1. Basic Types

#### `Variable<T>`
A type-safe variable that can hold any copy-constructible type.

```cpp
template <typename T>
class Variable {
public:
    // Constructors
    Variable();
    explicit Variable(const T& value);
    explicit Variable(T&& value);
    
    // Value access
    T get() const;
    operator T() const;
    
    // Value modification
    void set(const T& value);
    Variable& operator=(const T& value);
    
    // Type information
    static constexpr std::string_view type_name();
};
```

#### `TemporalVariable<T>`
A variable that tracks its value over time.

```cpp
template <typename T>
class TemporalVariable : public Variable<T> {
public:
    // Get value at a specific time
    std::optional<T> get_at(TimePoint when) const;
    
    // Get historical values in a time range
    std::vector<std::pair<TimePoint, T>> get_history(
        TimeRange range
    ) const;
    
    // Get rate of change (derivative)
    double get_rate_of_change() const;
};
```

### 2. Specialized Types

#### `AtomicVariable<T>`
Thread-safe variable with atomic operations.

```cpp
template <typename T>
class AtomicVariable : public Variable<T> {
public:
    // Atomic operations
    T exchange(T desired);
    bool compare_exchange_weak(T& expected, T desired);
    T fetch_add(T arg);
    // ... other atomic operations
};
```

#### `ComputedVariable<Result, Args...>`
A variable that computes its value from other variables.

```cpp
template <typename Result, typename... Args>
class ComputedVariable : public Variable<Result> {
public:
    template <typename Func>
    ComputedVariable(Func&& compute_func, const Variable<Args>&... args);
    
    // Recompute the value
    void recompute();
};
```

## Type Categories

### 1. Numeric Types
- `IntVariable`: Integer values
- `FloatVariable`: Floating-point values
- `DoubleVariable`: Double-precision floating-point values

### 2. Temporal Types
- `DurationVariable`: Time durations
- `TimePointVariable`: Specific points in time
- `TimeRangeVariable`: Ranges of time

### 3. Container Types
- `ArrayVariable<T>`: Fixed-size arrays
- `VectorVariable<T>`: Dynamic arrays
- `MapVariable<K, V>`: Key-value mappings

## Type Conversion

### Implicit Conversion
```cpp
IntVar x = 42;
DoubleVar y = x;  // Implicit conversion from int to double
```

### Explicit Conversion
```cpp
StringVar s = "3.14";
DoubleVar d = s.as<double>();  // Explicit conversion
```

### Custom Type Conversion
```cpp
// Define conversion for a custom type
struct Point { int x, y; };

template<>
struct VariableTraits<Point> {
    static std::string to_string(const Point& p) {
        return fmt::format("({}, {})", p.x, p.y);
    }
    
    static Point from_string(const std::string& s) {
        Point p;
        std::sscanf(s.c_str(), "(%d, %d)", &p.x, &p.y);
        return p;
    }
};

// Now you can use Point with Variable
Variable<Point> point = Point{10, 20};
std::string str = point.to_string();  // "(10, 20)"
```

## Type Safety

### Compile-Time Checking
```cpp
IntVar x = 42;
DoubleVar y = x;  // OK: int to double is safe
StringVar s = x;  // Compile error: no implicit conversion
```

### Runtime Type Checking
```cpp
VariableBase* var = get_variable("some_var");
if (var->is_type<int>()) {
    // Safe to cast
    auto int_var = var->as<IntVar>();
    // ...
}
```

## Advanced Features

### Type Erasure
```cpp
// Store any variable type in a type-erased container
AnyVariable var = IntVariable(42);
var = StringVariable("hello");
```

### Reflection
```cpp
// Get type information at runtime
const auto& type_info = var.get_type_info();
std::cout << "Type: " << type_info.name() << std::endl;
std::cout << "Size: " << type_info.size() << " bytes" << std::endl;
```

### Serialization
```cpp
// Save to JSON
json j;
j["value"] = var.serialize();

// Load from JSON
var.deserialize(j["value"]);
```

## Best Practices

1. **Use Strong Types**: Prefer specific variable types over generic ones
2. **Leverage Type Safety**: Let the compiler catch type errors
3. **Minimize Implicit Conversions**: Be explicit about type conversions
4. **Use `auto` with Variables**: Let the compiler deduce the exact type
5. **Document Custom Types**: Provide clear documentation for custom variable types

## Performance Considerations

- **Memory Layout**: Variables are stored with minimal overhead
- **Type Erasure**: Small buffer optimization for type-erased variables
- **Copy-on-Write**: Large values use copy-on-write semantics
- **Thread Safety**: Atomic operations where appropriate

## Related Documentation

- [Variable System Overview](../variable_system/)
- [Variable Flags](./variable_flags.md)
- [Performance Optimization](../../guides/performance.md)
