# Variable Flags API

## Overview
Variable Flags in Chronovyan provide a powerful mechanism to attach metadata and control behavior of variables. This document covers the built-in flags and how to create custom flags.

## Built-in Flags

### 1. Access Control Flags

#### `ReadOnly`
Makes a variable read-only after initialization.

```cpp
Variable<int> x = 42;
x.add_flag(ReadOnly{});
// x = 10;  // Compile-time error
```

#### `Volatile`
Indicates the variable can change asynchronously.

```cpp
VolatileVariable<int> sensor_reading;
// The value might change between accesses
```

### 2. Persistence Flags

#### `Persistent`
Automatically saves variable state to persistent storage.

```cpp
Variable<std::string> config("default");
config.add_flag(Persistent{"app.config.theme"});
// Value persists between application restarts
```

#### `Transient`
Explicitly marks a variable as non-persistent.

```cpp
Variable<int> counter;
counter.add_flag(Transient{});
// Value is not saved to persistent storage
```

### 3. Notification Flags

#### `Notifiable`
Enables change notifications.

```cpp
Variable<int> value;
value.add_flag(Notifiable{});
value.on_change([](const auto& old_val, const auto& new_val) {
    std::cout << "Value changed from " << old_val << " to " << new_val << std::endl;
});
```

#### `Debounced`
Delays change notifications until the value stabilizes.

```cpp
Variable<int> search_query;
search_query.add_flag(Debounced{std::chrono::milliseconds(300)});
search_query.on_change([](auto&&...) {
    // Triggered only after user stops typing for 300ms
});
```

### 4. Validation Flags

#### `Range`
Ensures the variable stays within specified bounds.

```cpp
Variable<int> age;
age.add_flag(Range{0, 150});
// age = 200;  // Throws std::out_of_range
```

#### `Regex`
Validates string variables against a regular expression.

```cpp
Variable<std::string> email;
email.add_flag(Regex{"^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$"});
// email = "invalid";  // Throws std::invalid_argument
```

## Custom Flags

### Creating a Custom Flag

```cpp
class LogChanges : public VariableFlag {
public:
    template <typename T>
    void on_change(
        const Variable<T>& var,
        const T& old_value,
        const T& new_value
    ) const {
        std::cout << var.name() << " changed from " 
                  << old_value << " to " << new_value << std::endl;
    }
};

// Usage
Variable<int> counter;
counter.add_flag(LogChanges{});
counter = 10;  // Logs: "counter changed from 0 to 10"
```

### Flag Composition

#### `AllOf`
Requires all conditions to be true.

```cpp
Variable<int> x;
x.add_flag(AllOf{
    Range{0, 100},
    Validator{[](int v) { return v % 2 == 0; }},  // Must be even
    OnChange{[](auto&&...) { /* ... */ }}
});
```

#### `AnyOf`
Requires at least one condition to be true.

```cpp
Variable<std::string> id;
id.add_flag(AnyOf{
    Regex{"^[A-Z]{3}\\d{3}$"},  // ABC123
    Regex{"^[a-z]{2}-\\d{4}$"}  // ab-1234
});
```

## Flag Metadata

### Attaching Metadata

```cpp
Variable<int> priority;
priority.add_flag("importance", "high");
priority.add_flag("owner", "team-a");

// Later...
if (priority.has_flag("importance") && 
    priority.get_flag_value("importance") == "high") {
    // Handle high priority
}
```

### Built-in Metadata Flags

#### `Description`
Human-readable description of the variable.

```cpp
Variable<float> temperature;
temperature.add_flag(Description{"Ambient temperature in degrees Celsius"});
```

#### `Units`
Physical units of the variable.

```cpp
Variable<double> distance;
distance.add_flag(Units{"meters"});
```

## Flag Inheritance

### Variable Templates

```cpp
template <typename T>
class MonitoredVariable : public Variable<T> {
public:
    using Variable<T>::Variable;
    
    MonitoredVariable() {
        this->add_flag(LogChanges{});
        this->add_flag(Persistent{"app." + this->name()});
    }
};

// Usage
MonitoredVariable<int> counter;  // Automatically has logging and persistence
```

### Flag Composition

```cpp
const auto AppSetting = [](auto&&... args) {
    return AllOf{
        Persistent{std::forward<decltype(args)>(args)...},
        Notifiable{},
        Description{"Application setting"}
    };
};

Variable<std::string> theme("dark");
theme.add_flag(AppSetting{"app.theme"});
```

## Performance Considerations

- **Flag Overhead**: Each flag adds a small overhead to variable operations
- **Virtual Calls**: Some flags use virtual calls for extensibility
- **Memory Usage**: Flags increase the memory footprint of variables
- **Compile-Time Flags**: Some flags can be optimized away at compile time

## Best Practices

1. **Use Flags Judiciously**: Only add flags that provide value
2. **Combine Related Flags**: Use composition to group related flags
3. **Document Custom Flags**: Provide clear documentation for custom flags
4. **Consider Performance**: Be mindful of the performance impact of flags
5. **Test Thoroughly**: Flags can interact in complex ways

## Related Documentation

- [Variable Types](./variable_types.md)
- [Variable System Overview](../variable_system/)
- [Performance Optimization](../../guides/performance.md)
