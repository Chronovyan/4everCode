---
title: 4ever Compiler
description: Documentation for README_COMPILER.md
weight: 100
draft: true
---

# 4ever Compiler

The 4ever Compiler is a C++ implementation of a compiler for the 4ever programming language, a domain-specific language designed for temporal programming and resource management.

## Overview

The compiler consists of several main components:

1. **Lexer**: Analyzes the source code and produces a stream of tokens.
2. **Parser**: Transforms the token stream into an Abstract Syntax Tree (AST).
3. **Semantic Analyzer**: Performs type checking and ensures semantic correctness.
4. **Code Generator**: Generates C code from the AST.

## Features

- Support for various data types: integers, floats, booleans, strings, etc.
- Resource management with chronons and aethel.
- Variable declarations and assignments.
- Control flow statements (if, while).
- Resource-aware execution blocks (allocate, expend, borrow, repay).
- Type checking and semantic analysis.
- C code generation for easier portability.

## Building the Compiler

### Prerequisites

- C++17 compatible compiler
- CMake 3.12 or higher

### Build Instructions

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Usage

```bash
./4ever_compiler <input_file> [-o <output_file>] [-v]
```

Options:
- `-o <output_file>`: Specify output file (default: `<input_file>.c`)
- `-v, --verbose`: Enable verbose output

## Example

A simple 4ever program:

```
// Hello World program in 4ever language
allocate 10 chronons, 5 aethel {
    var string message = "Hello, 4ever World!";
    println(message);
    
    var int counter = 0;
    while (counter < 5) {
        expend 1 chronons {
            println("Counter: " + counter);
            counter = counter + 1;
        }
    }
}
```

## Project Structure

- `src/compiler/`: Source code for the compiler
  - `include/compiler/`: Header files
  - `src/`: Implementation files
- `samples/`: Sample 4ever programs
- `tests/`: Unit tests for the compiler

## Runtime

The compiler generates C code that depends on a small runtime library (`4ever_runtime.h`), which provides:

- Resource management functions
- Temporal operations
- Basic I/O functionality

The runtime is included in the generated code, so no separate installation is required.

## License

This project is licensed under the MIT License - see the LICENSE file for details.