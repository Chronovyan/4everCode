---
title: Reflection Module
description: Runtime type information and reflection
weight: 90
---

# Reflection Module

## Overview

The `std.reflect` module provides facilities for runtime type inspection, dynamic code evaluation, and metaprogramming. This module enables you to examine and manipulate program structure at runtime.

## Type Information

### `type_of<T>() -> Type`
Returns a `Type` object representing the type `T`.

```chronovyan
IMPORT std.reflect as reflect;

DECLARE int_type = reflect.type_of<INT>();
PRINT(int_type.name());  // "INT"
```

### `type_of(value: ANY) -> Type`
Returns the runtime type of a value.

```chronovyan
DECLARE x = 42;
DECLARE type = reflect.type_of(x);
PRINT(type.name());  // "INT"
```

### Type Properties

```chronovyan
DECLARE t = reflect.type_of<FLOAT>();

PRINT(t.name());        // "FLOAT"
PRINT(t.sizeof());      // 8 (bytes)
PRINT(t.is_numeric());  // true
PRINT(t.is_compound()); // false
```

## Inspecting Types

### Checking Type Kinds

```chronovyan
DECLARE t = reflect.type_of([1, 2, 3]);

PRINT(t.is_array());     // true
PRINT(t.is_struct());    // false
PRINT(t.is_primitive()); // false
```

### Getting Type Members

```chronovyan
// For structs and classes
FOR field IN type.fields() {
    PRINT(field.name(), ": ", field.type().name());
}

// For enums
FOR variant IN type.variants() {
    PRINT("Variant: ", variant.name());
}
```

## Dynamic Type Creation

### Creating Struct Types

```chronovyan
// Define a struct type at runtime
DECLARE fields = [
    ("name", reflect.type_of<STRING>()),
    ("age", reflect.type_of<INT>())
];

DECLARE person_type = reflect.define_struct("Person", fields);

// Create an instance
DECLARE person = person_type.construct(["Alice", 30]);
PRINT(person);  // Person { name: "Alice", age: 30 }
```

## Runtime Evaluation

### `eval(code: STRING) -> ANY`
Evaluates a string as Chronovyan code.

```chronovyan
DECLARE result = reflect.eval("2 + 2 * 2");
PRINT(result);  // 6
```

### `compile(expr: STRING) -> FUNCTION`
Compiles an expression into a callable function.

```chronovyan
DECLARE add = reflect.compile("a + b");
PRINT(add(2, 3));  // 5
```

## Function Reflection

### Getting Function Information

```chronovyan
FUNC INT add(INT a, INT b) {
    RETURN a + b;
}

DECLARE func_type = reflect.type_of(add);
PRINT(func_type.return_type().name());  // "INT"

FOR param IN func_type.parameters() {
    PRINT(param.name, ": ", param.type.name());
    // a: INT
    // b: INT
}
```

### Calling Functions Dynamically

```chronovyan
DECLARE func = reflect.get_function("std.math.sqrt");
DECLARE result = func.call([16.0]);
PRINT(result);  // 4.0
```

## Example: JSON Serializer

```chronovyan
IMPORT std.reflect as reflect;
IMPORT std.string as str;

FUNC STRING to_json(value: ANY) -> STRING {
    DECLARE type = reflect.type_of(value);
    
    IF (type.is_primitive()) {
        IF (type.is_string()) {
            RETURN "\"" + str.escape_json(value as STRING) + "\"";
        }
        RETURN str(value);
    }
    ELSE IF (type.is_array()) {
        DECLARE items = value as ARRAY;
        DECLARE parts = [];
        
        FOR item IN items {
            parts.push(to_json(item));
        }
        
        RETURN "[" + str.join(", ", parts) + "]";
    }
    ELSE IF (type.is_map()) {
        DECLARE map = value as MAP<STRING, ANY>;
        DECLARE parts = [];
        
        FOR (key, val) IN map {
            parts.push('"' + str.escape_json(key) + '": ' + to_json(val));
        }
        
        RETURN "{" + str.join(", ", parts) + "}";
    }
    ELSE IF (type.is_struct()) {
        DECLARE fields = [];
        
        FOR field IN type.fields() {
            DECLARE field_value = reflect.get_field(value, field.name);
            fields.push('"' + field.name + '": ' + to_json(field_value));
        }
        
        RETURN "{" + str.join(", ", fields) + "}";
    }
    
    RETURN "null";
}

// Example usage
DECLARE data = {
    "name": "Alice",
    "age": 30,
    "scores": [95, 89, 92]
};

PRINT(to_json(data));
// {"name": "Alice", "age": 30, "scores": [95, 89, 92]}
```

## Performance Considerations

- Reflection operations are slower than direct method calls
- Cache `Type` objects when possible
- Use `type_of<T>()` instead of `type_of(value)` when the type is known at compile time
- Be cautious with `eval()` as it can have security implications

## Security Considerations

1. **Use sparingly** - Prefer static typing when possible
2. **Cache reflection data** for repeated lookups
3. **Validate early** - Check types and constraints as soon as possible
4. **Handle errors gracefully** - Reflection can fail in many ways
5. **Document dynamic behavior** - Make it clear when and how reflection is used
6. **Consider performance** - Reflection can be slow, avoid in hot paths
7. **Be mindful of security** - Dynamic code loading can be a security risk

## See Also

- [Standard Library](../stdlib/index.md) - Overview of all standard library modules
- [Chronovyan Language Specification](reference/language/specification.md) - For language-level reflection features
- [Metaprogramming](../advanced/metaprogramming.md) - For compile-time code generation
