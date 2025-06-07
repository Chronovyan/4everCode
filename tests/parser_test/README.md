---
title: Readme
description: Documentation for README
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Chronovyan Parser Test

This directory contains a minimal test for the Chronovyan project's core components. The test focuses on the basic functionality of the `Token`, `SourceLocation`, and `SourceFile` classes.

## API Harmonization

During the development of these tests, several API inconsistencies were identified and addressed:

1. **Method Naming Consistency**:
   - The `Value` class has both primary methods and backward-compatible aliases:
     - `isBoolean()` and `isBool()`
     - `asBoolean()` and `asBool()`
     - `isInteger()` and `isInt()`
     - `asInteger()` and `asInt()`
   - The `Environment` class has both `contains()` and `exists()` with identical behavior

2. **Constructor Implementation**:
   - `VariableDeclStmtNode` constructors are defined inline in the header file to avoid redefinition errors

3. **Source File Path Access**:
   - `SourceFile` class provides `getPath()` method to access the file path

4. **ResourceOptimizer Integration**:
   - Test stubs for `ResourceOptimizer` and related classes ensure proper integration with the `Interpreter` class

## Test Structure

The minimal test covers:

1. Creating a `SourceFile` with the correct string move semantics
2. Creating a `SourceLocation` with file, line, column, and position information
3. Creating a `Token` with a specific type, lexeme, and location

## Future Improvements

Potential areas for further enhancement:

1. Expand tests to cover more of the parser functionality
2. Add tests for the `Lexer` class to ensure proper tokenization
3. Add tests for the `Interpreter` to validate execution semantics
4. Consider consolidating the naming conventions to use a single set of method names in future versions

## Integration Notes

When working with the Chronovyan API, be aware of these API patterns:

1. Use the primary method names for new code (`isBoolean()`, `asBoolean()`, etc.)
2. The aliases (`isBool()`, `asBool()`, etc.) are maintained for backward compatibility
3. Constructor overloads often include a version with source location and one without 