---
title: Readme
description: Documentation for README
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Chronovyan Language Examples

This directory contains examples of Chronovyan language programs, from basic to more advanced.

## Current Language Support Status

As of the current interpreter version, the following features are supported:

### Working Features
- Simple print statements: `print("Hello, World!");`
- Basic string literals and concatenation
- Comments (single-line and multi-line)
- String concatenation with the `+` operator
- Basic arithmetic operations (`+`, `-`, `*`, `/`)

### Partially Working or Limited Support
- Variable declarations with `DECLARE CONF` syntax
- Function declarations with `FUNCTION` keyword
- Basic control flow structures

### Syntax Requirements
- All statements must end with semicolons
- The language is case-sensitive
- Keywords are generally in UPPERCASE
- Function and variable names use snake_case

## Example Files

- `minimal.cvy`: The simplest possible Chronovyan program
- `print_only.cvy`: Basic print statement example
- `test_basic.cvy`: Tests for basic print and string concatenation
- `test_arithmetic.cvy`: Tests for basic arithmetic operations
- `variables.cvy`: Examples of variable declarations
- `functions.cvy`: Function declaration and usage examples
- `syntax_guide.cvy`: Comprehensive syntax guide with examples

## Running Examples

To run any example, use the Chronovyan interpreter:

```
.\build\chronovyan_interpreter.exe examples\01_beginner\minimal.cvy
```

## Notes on Language Development

The Chronovyan language is in active development. Some features shown in examples may not be fully implemented yet, or the syntax might change in future versions.

For the most reliable examples that work with the current interpreter version, see:
- `minimal.cvy`
- `print_only.cvy`
- `test_basic.cvy`
- `test_arithmetic.cvy`

## Examples Overview

1. **01_hello_world.cvy** - The first Chronovyan program with basic syntax introduction
2. **02_hello_world.cvy** - Simple "Hello World" example with comments and basic constructs
3. **03_hello_world_utf8.cvy** - UTF-8 version of Hello World example
4. **04_minimal.cvy** - The most minimal valid Chronovyan program
5. **05_simple_test.cvy** - Basic syntax test with simple operations
6. **06_test.cvy** - Minimal test program
7. **07_custom_types.cvy** - Introduction to custom types in Chronovyan
8. **08_digit_test.cvy** - Simple digit testing program
9. **09_digit_test_utf8.cvy** - UTF-8 version of digit testing
10. **10_main_test.cvy** - Testing the main function
11. **11_hello_timeline.cvy** - Introduction to timelines in Chronovyan
12. **12_hello_timeline_utf8.cvy** - UTF-8 version of timeline introduction

## Getting Started

Start with `01_hello_world.cvy` for an introduction to Chronovyan programming. These examples are designed to be run with the Chronovyan interpreter:

```
chronovyan examples/01_beginner/01_hello_world.cvy
```

Each example includes comments explaining the key concepts being demonstrated.

## Prerequisites

- Chronovyan interpreter installed
- Basic understanding of programming concepts

## Learning Path

Start with these examples in the following order:

1. **01_hello_world.cvy** - Basic syntax, variables, and printing
2. **02_minimal.cvy** - The simplest valid Chronovyan program
3. **03_variables.cvy** - Variable types (CONF and REB) and modifiers
4. **04_hello_timeline.cvy** - Introduction to timeline manipulation
5. **05_conditionals.cvy** - Conditional statements and logic
6. **06_loops.cvy** - Loop structures and iteration
7. **07_custom_types.cvy** - Custom data types and type system

## Expected Knowledge

After completing these examples, you should understand:

- Basic syntax and structure of Chronovyan programs
- Different variable types and their behaviors
- How to use simple timeline operations
- Basic control flow (conditionals and loops)
- How to print output and basic string manipulation
- How to define and use custom data types

## Next Steps

After mastering these examples, proceed to the **02_intermediate** directory for more advanced concepts. 