---
title: String Module
description: String manipulation and formatting
weight: 60
---

# String Module

## Overview

The `std.string` module provides comprehensive string manipulation and text processing capabilities for Chronovyan.

## String Creation

### String Literals
```chronovyan
DECLARE s1 = "Hello";           // UTF-8 string
DECLARE s2 = 'single quotes';    // Also valid
DECLARE raw = r"C:\\path\\to\\file";  // Raw string
DECLARE multi = """
    Multi-line
    string
""";
```

### String Constructors
```chronovyan
DECLARE s = String(42);          // "42"
DECLARE ch = String.from_char(65); // "A"
DECLARE rep = String.repeat("-", 10); // "----------"
```

## String Properties

### `length`
Number of Unicode characters (graphemes).
```chronovyan
"hello".length  // 5
"こんにちは".length  // 5
```

### `is_empty`
Checks if the string is empty.

### `is_whitespace`
Checks if all characters are whitespace.

## String Operations

### Concatenation
```chronovyan
DECLARE name = "Alice";
DECLARE greeting = "Hello, " + name + "!";  // "Hello, Alice!"
```

### Repetition
```chronovyan
"na" * 3  // "nanana"
```

### Indexing
```chronovyan
DECLARE s = "hello";
s[0]    // 'h'
s[-1]   // 'o' (negative index from end)
```

### Slicing
```chronovyan
DECLARE s = "hello";
s[1:4]    // "ell"
s[::2]     // "hlo" (every second character)
s[::-1]    // "olleh" (reverse)
```

## String Methods

### Case Conversion
```chronovyan
"Hello".to_upper()    // "HELLO"
"Hello".to_lower()    // "hello"
"hello world".to_title()  // "Hello World"
```

### Trimming
```chronovyan
"  hello  ".trim()        // "hello"
"  hello  ".trim_start()  // "hello  "
"  hello  ".trim_end()    // "  hello"
```

### Searching
```chronovyan
"hello".contains("ell")   // true
"hello".st_with("he")     // true
"hello".ends_with("lo")   // true
"hello".find("ll")        // 2
"hello".rfind("l")        // 3
```

### Splitting and Joining
```chronovyan
"a,b,c".split(",")       // ["a", "b", "c"]
"a b c".split_whitespace() // ["a", "b", "c"]
" ".join(["a", "b", "c"]) // "a b c"
```

### Replacing
```chronovyan
"hello".replace("l", "L")      // "heLLo"
"hello".replace("l", "L", 1)   // "heLlo" (replace first occurrence)
```

## String Formatting

### Format Strings
```chronovyan
IMPORT std.string as str;

// Positional arguments
str.format("{} {}", "Hello", "World")  // "Hello World"

// Named arguments
str.format("{name} is {age} years old", name: "Alice", age: 30)

// Format specifiers
str.format("π = {:.2f}", 3.14159)  // "π = 3.14"
```

### Format Specifiers

| Specifier | Meaning | Example |
|-----------|---------|---------|
| `s` | String (default) | `"hello"` |
| `d` | Decimal integer | `42` |
| `x` | Lowercase hex | `2a` |
| `X` | Uppercase hex | `2A` |
| `f` | Fixed-point | `3.14` |
| `e` | Scientific | `3.14e0` |
| `%` | Percentage | `0.5` → `50.00%` |

## Regular Expressions

### Basic Matching
```chronovyan
IMPORT std.regex;

DECLARE re = regex.compile(r"\d+");
DECLARE text = "123 abc 456";

// Check if pattern matches
IF (re.matches(text)) {
    PRINT("Found a number!");
}

// Find all matches
FOR match IN re.find_all(text) {
    PRINT(match.group(0));  // "123", then "456"
}
```

### Capturing Groups
```chronovyan
DECLARE re = regex.compile(r"(\d{4})-(\d{2})-(\d{2})");
DECLARE result = re.captures("2023-04-01").unwrap();

PRINT(result[0]);  // "2023-04-01" (full match)
PRINT(result[1]);  // "2023" (first group)
PRINT(result[2]);  // "04"
PRINT(result[3]);  // "01"
```

## String Encoding

### UTF-8 Operations
```chronovyan
// Get bytes (UTF-8 encoded)
"こんにちは".as_bytes()  // [0xe3, 0x81, 0x93, ...]

// Create from bytes
String::from_utf8([0x48, 0x69])

// Iterate over characters
FOR char IN "hello".chars() {
    PRINT(char);
}

// Iterate over bytes
FOR byte IN "hello".bytes() {
    PRINT(byte);
}
```

## Example: Word Counter

```chronovyan
IMPORT std.string as str;

FUNC MAP<STRING, INT> word_frequency(STRING text) {
    DECLARE words = text.to_lower()
                      .split_whitespace()
                      .map(word => word.trim(".,!?\"").to_string())
                      .filter(word => !word.is_empty());
    
    DECLARE freq = MAP<STRING, INT>();
    
    FOR word IN words {
        freq[word] = freq.get(word).unwrap_or(0) + 1;
    }
    
    RETURN freq;
}

// Example usage
DECLARE text = "Hello, world! Hello, Chronovyan!";
DECLARE freq = word_frequency(text);
PRINT(freq);  // {"hello": 2, "world": 1, "chronovyan": 1}
```

## Performance Considerations

- Strings are immutable; operations that modify strings create new ones
- Use `StringBuilder` for building large strings efficiently
- Be mindful of Unicode grapheme clusters when processing text
- Regular expressions can be compiled once and reused

## Common Patterns

### String Templating

```chronovyan
IMPORT std.string as str;

// Simple string interpolation
LET name = "Alice";
LET greeting = str.format("Hello, {}! Today is {}", name, str.date("%Y-%m-%d"));

// Named parameters
LET message = str.format(
    "{user} has {count} new {message}s", 
    [("user", "Bob"), ("count", 5), ("message", "notification")]
);

// Template strings with custom delimiters
LET template = "Hello, {{name}}! Your score is {{score}}/{{max}}.";
LET ctx = [("name", "Charlie"), ("score", 95), ("max", 100)];
LET result = str.template(template, ctx);
```

### Text Processing Pipeline

```chronovyan
IMPORT std.string as str;

FUNC analyze_text(text: STRING) -> MAP<STRING, ANY> {
    // Normalize whitespace and convert to lowercase
    LET normalized = text
        .trim()
        .to_lowercase()
        .replace(r"\s+", " ");
    
    // Count words and characters
    LET words = normalized.split(' ');
    LET word_count = words.len();
    LET char_count = text.chars().count();
    
    // Find most common words
    DECLARE word_freq = MAP<STRING, INT>();
    FOR word IN words {
        word_freq[word] = word_freq.get(word).unwrap_or(0) + 1;
    }
    
    // Sort by frequency
    LET common_words = word_freq
        .iter()
        .sorted_by(|a, b| b.1.cmp(a.1))
        .take(5)
        .collect::<VEC<_>>();
    
    RETURN [
        ("word_count", word_count as ANY),
        ("char_count", char_count as ANY),
        ("avg_word_length", char_count as FLOAT / word_count as FLOAT),
        ("common_words", common_words as ANY)
    ];
}

## Performance Considerations

### String Building

```chronovyan
// Inefficient: Creates multiple intermediate strings
LET result = "";
FOR i IN 1..=1000 {
    result += i.to_string();
}

// Efficient: Uses a string builder
LET mut builder = str.StringBuilder::new();
FOR i IN 1..=1000 {
    builder.push_str(&i.to_string());
}
LET result = builder.build();
```

### Regular Expressions

```chronovyan
// Compile regex once if used repeatedly
LET RE_DATE = str.Regex::new(r"\d{4}-\d{2}-\d{2}");

FUNC extract_dates(text: STRING) -> VEC<STRING> {
    RETURN RE_DATE.find_all(text)
        .map(|m| m.as_str())
        .collect();
}

## Text Encoding and Unicode

### Normalization

```chronovyan
// Convert between different Unicode normal forms
LET s = "café";
LET nfd = str.normalize(s, "NFD");  // 'e' + '\u0301'
LET nfc = str.normalize(s, "NFC");  // 'é'
```

### Grapheme Clusters

```chronovyan
// Count user-perceived characters (not code points)
LET s = "नमस्ते";  // Hindi "Namaste"
LET char_count = str.graphemes(s).count();  // 6
LET code_point_count = s.chars().count();   // 7
```

## Related

- [Collections](./collections.md) - For working with collections of strings
- [IO](./io.md) - For reading and writing strings to files
- [Regex](./regex.md) - For advanced pattern matching
- [Testing](./testing.md) - For string assertions and test utilities

## Best Practices

1. **Use string builders** for concatenation in loops
2. **Prefer string slices** (`&str`) over owned strings when possible
3. **Be explicit about encodings** when working with external data
4. **Use Unicode-aware functions** for text processing
5. **Consider using string interning** for many small strings
6. **Validate and sanitize** all user input

## See Also

- [Standard Library](../stdlib/index.md) - Overview of all standard library modules
- [Chronovyan Language Specification](reference/language/specification.md) - For string literals and built-in string operations
