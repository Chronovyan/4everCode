---
title: IO Module
description: Input/Output operations
weight: 40
---

# IO Module

## Overview

The `std.io` module provides facilities for performing input and output operations in Chronovyan, including file I/O, standard streams, and serialization.

## Standard Streams

### `print` and `println`
Write to standard output.

```chronovyan
IMPORT std.io;

io.print("Hello, ");  // No newline
io.println("World!");  // With newline
```

### `eprint` and `eprintln`
Write to standard error.

```chronovyan
io.eprintln("Error: Something went wrong!");
```

### `read_line`
Read a line from standard input.

```chronovyan
io.print("Enter your name: ");
DECLARE name = io.read_line();
println("Hello, ", name, "!");
```

## File I/O

### `File`
Represents a file handle.

```chronovyan
// Open a file for reading
DECLARE file = io.File.open("data.txt", "r");

// Read all content
DECLARE content = file.read_all();

// Read line by line
FOR line IN file.lines() {
    println(line);
}

// Don't forget to close the file
file.close();

// Using 'with' for automatic resource management
WITH file = io.File.open("output.txt", "w") {
    file.write_line("Hello, file!");
}  // Automatically closed here
```

### File Modes

| Mode | Description |
|------|-------------|
| "r"  | Read (default) |
| "w"  | Write (truncate) |
| "a"  | Append |
| "r+" | Read/write |
| "w+" | Read/write (truncate) |
| "a+" | Read/append |

## Serialization

### `to_json` and `from_json`
Convert between Chronovyan values and JSON.

```chronovyan
DECLARE data = {
    "name": "Alice",
    "age": 30,
    "tags": ["rust", "python"]
};

// Convert to JSON string
DECLARE json_str = io.to_json(data);

// Parse JSON string back to value
DECLARE parsed = io.from_json<MAP<STRING, ANY>>(json_str);
```

## Path Manipulation

### `Path`
Platform-independent path manipulation.

```chronovyan
DECLARE path = io.Path("docs/reference");

// Join paths
DECLARE full_path = path.join("stdlib/io.md");

// Get file extension
PRINT(full_path.extension());  // "md"

// Check if path exists
IF (full_path.exists()) {
    PRINT("File exists!");
}
```

## Temporary Files and Directories

### `tempfile`
Create temporary files and directories.

```chronovyan
// Create a temporary file
WITH temp_file = io.tempfile("prefix_", ".txt") {
    temp_file.write_line("Temporary data");
    // File is automatically deleted when 'with' block ends
}

// Create a temporary directory
WITH temp_dir = io.tempdir() {
    DECLARE file = temp_dir.join("data.txt");
    file.write_text("Some data");
    // Directory and its contents are automatically deleted
}
```

## Example: File Copy

```chronovyan
IMPORT std.io;

FUNC VOID copy_file(STRING source, STRING dest) {
    WITH src = io.File.open(source, "r"),
         dst = io.File.open(dest, "w") {
        
        // Copy in chunks to handle large files
        WHILE (TRUE) {
            DECLARE chunk = src.read(8192);  // 8KB chunks
            IF (chunk.is_empty()) BREAK;
            dst.write(chunk);
        }
    }
}
```

## Performance Considerations

- Use buffered I/O for better performance with small, frequent writes
- Always close files explicitly or use `with` statements
- For large files, read/write in chunks rather than loading everything into memory
- Consider memory-mapped files for very large files

## Error Handling

All I/O operations return `Result<T, IOError>` which must be handled:

```chronovyan
MATCH io.File.open("nonexistent.txt", "r") {
    OK(file) => {
        // Handle file
        file.close();
    },
    ERR(e) => {
        eprintln("Error: ", e.message);
    }
}
```

## Common Patterns

### File Processing Pipeline

```chronovyan
IMPORT std.io as io;
IMPORT std.string as str;

// Process a log file and count IP addresses
FUNC MAP<STRING, INT> count_ips(filename: STRING) -> MAP<STRING, INT> {
    DECLARE counts = MAP<STRING, INT>();
    
    // Process file line by line to handle large files
    WITH file = io.File.open(filename, "r") {
        FOR line IN file.lines() {
            // Extract IP address (simplified)
            IF (LET SOME(ip) = str.extract(line, r"\b(?:\d{1,3}\.){3}\d{1,3}\b")) {
                counts[ip] = counts.get(ip).unwrap_or(0) + 1;
            }
        }
    }
    
    RETURN counts;
}

// Example usage
DECLARE ip_counts = count_ips("access.log");
FOR (ip, count) IN ip_counts.iter().sorted_by(|a, b| b.1.cmp(a.1)).take(10) {
    io.println(ip, ": ", count);
}
```

### Config File Parser

```chronovyan
IMPORT std.io as io;
IMPORT std.string as str;

STRUCT Config {
    host: STRING,
    port: INT,
    debug: BOOL,
    log_level: STRING,
}

FUNC Config load_config(filename: STRING) -> Config {
    DECLARE config = Config {
        host: "localhost",
        port: 8080,
        debug: FALSE,
        log_level: "info"
    };
    
    WITH file = io.File.open(filename, "r") {
        FOR line IN file.lines() {
            LET trimmed = str.trim(line);
            IF (trimmed.is_empty() || trimmed.starts_with("#")) {
                CONTINUE;
            }
            
            IF (LET [key, value] = str.split_once(trimmed, "=")) {
                MATCH str.trim(key).to_lowercase() {
                    "host" => config.host = str.trim(value).trim_matches('"'),
                    "port" => config.port = str.parse::<INT>(str.trim(value)).unwrap_or(8080),
                    "debug" => config.debug = str.parse_bool(str.trim(value)).unwrap_or(FALSE),
                    "log_level" => config.log_level = str.trim(value).to_lowercase(),
                    _ => {}
                }
            }
        }
    }
    
    RETURN config;
}
```

## Performance Considerations

### Buffering

```chronovyan
// Good: Uses buffering
WITH file = io.File.open_buffered("large_file.txt", "r") {
    // Operations are buffered for better performance
}

// Bad: Unbuffered I/O
WITH file = io.File.open_unbuffered("large_file.txt", "r") {
    // Each read operation hits the disk
}
```

### Memory-Mapped Files

For very large files, consider using memory mapping:

```chronovyan
IMPORT std.io.mmap as mmap;

WITH m = mmap.MemoryMap::open("huge_file.bin") {
    // Access file contents as a byte slice
    DECLARE data = m.as_slice();
    // Process data...
}
```

## Error Handling Best Practices

```chronovyan
FUNC RESULT<STRING, io.Error> read_config(path: STRING) -> RESULT<STRING, io.Error> {
    WITH file = TRY(io.File.open(path, "r")) {
        RETURN file.read_all();
    }
}

// Usage
MATCH read_config("config.json") {
    OK(contents) => process_config(contents),
    ERR(io.Error::NotFound) => create_default_config(),
    ERR(e) => eprintln("Error reading config: ", e),
}
```

## Related

- [Concurrency](./concurrency.md) - For asynchronous I/O operations
- [String](./string.md) - For text processing and manipulation
- [Collections](./collections.md) - For working with in-memory data structures
- [Testing](./testing.md) - For testing I/O operations

## Best Practices

1. **Always close files** using `WITH` or explicit `close()`
2. **Handle errors** explicitly
3. **Use buffering** for better performance with small, frequent I/O
4. **Be mindful of encoding** when reading text files
5. **Use absolute paths** or properly resolve relative paths
6. **Clean up temporary files** after use

## See Also

- [Standard Library](../stdlib/index.md) - Overview of all standard library modules
- [Chronovyan Language Specification](reference/language/specification.md) - For I/O related language features
- [Collections](./collections.md) - For working with data structures
