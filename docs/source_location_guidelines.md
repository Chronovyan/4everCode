---
title: Source Location Guidelines
description: Documentation for source_location_guidelines.md
weight: 100
draft: true
---

# Source Location Guidelines

## Overview

The SourceLocation system in 4ever is a fundamental component for error reporting, debugging, and code analysis. It provides the spatial coordinates within source code that enable precise error messages, stack traces, and debugging information.

This document outlines the best practices for using the SourceLocation system to ensure consistent and accurate source location tracking throughout the codebase.

## Key Components

The source location system consists of the following main components:

1. **SourceLocation**: A struct representing a location in the source code (file, line, column, position).
2. **SourceFile**: A class representing a source file with utility methods for line/column tracking.
3. **Helper Functions**: Utility functions for creating and manipulating source locations.

## SourceLocation Usage

### Creating Source Locations

When creating a SourceLocation, always prefer using actual locations from the source when available:

```cpp
// Create a location with known coordinates
SourceLocation location(sourceFile, line, column, position);

// Use helper function to create the best possible location
SourceLocation bestLocation = createBestLocation(sourceFile, line, column, position);
```

Only use the default constructor when the location is genuinely unknown:

```cpp
// Only use this when the location is truly unknown
SourceLocation unknownLocation;
```

### Checking Validity

Always check if a source location is valid before using it in critical operations:

```cpp
if (location.isValid()) {
    // Use the location
} else {
    // Fall back to a default behavior
}
```

### Creating Location Ranges

Use the `createRange` method to create a location that spans from one location to another:

```cpp
try {
    SourceLocation range = startLocation.createRange(endLocation);
    // Use the range
} catch (const std::invalid_argument& e) {
    // Handle the error (locations from different files, etc.)
}
```

Or use the `mergeLocations` helper function for a safer approach:

```cpp
SourceLocation range = mergeLocations(startLocation, endLocation);
// This will never throw, even if the locations are invalid or from different files
```

### Comparing Locations

Use the comparison methods to determine the order of locations:

```cpp
if (location1.isBefore(location2)) {
    // location1 is before location2 in the source code
}

if (location1.isAfter(location2)) {
    // location1 is after location2 in the source code
}

if (location1 == location2) {
    // Locations are the same
}
```

### Getting Detailed Information

Use the `toDetailedString` method to get a more detailed representation of a location:

```cpp
// Get a detailed string with the line content
std::string detailed = location.toDetailedString(true);
std::cout << detailed << std::endl;
/* Output example:
file.chv:10:15
some code here
              ^
*/
```

## Best Practices

### 1. Always Propagate Source Locations

When creating AST nodes, tokens, or other syntax elements, always propagate the source location information:

```cpp
// Good: Propagate the location from child to parent
ExprNode::ExprNode(const SourceLocation& location) : ASTNode(location) {}

// Better: Use child's location if available, otherwise default
BinaryExprNode::BinaryExprNode(
    std::unique_ptr<ExprNode> left,
    Token op,
    std::unique_ptr<ExprNode> right
) : ExprNode(op.location), // Use the operator's location as the base
    m_left(std::move(left)),
    m_operator(std::move(op)),
    m_right(std::move(right)) {}
```

### 2. Use mergeLocations for Compound Expressions

When creating nodes that span multiple tokens or expressions, use `mergeLocations` to create a location that covers the entire range:

```cpp
// Create a location that spans from the start of the condition to the end of the else branch
SourceLocation ifStmtLocation = mergeLocations(
    condition->getLocation(),
    elseBranch ? elseBranch->getLocation() : thenBranch->getLocation()
);

// Create the if statement node with the merged location
return std::make_unique<IfStmtNode>(ifStmtLocation, std::move(condition), std::move(thenBranch), std::move(elseBranch));
```

### 3. Never Use Default Locations for Errors

Always provide a valid source location when reporting errors:

```cpp
// Bad: Using default location loses context
ErrorHandler::getInstance().reportError(
    SourceLocation(),
    "Something went wrong"
);

// Good: Using the actual location
ErrorHandler::getInstance().reportError(
    node->getLocation(),
    "Something went wrong"
);
```

### 4. Use Detailed Locations for Important Errors

For important errors, use detailed location information:

```cpp
// Provide detailed context for important errors
std::string errorMessage = "Invalid type conversion from " + sourceType + " to " + targetType;
ErrorHandler::getInstance().reportError(
    node->getLocation(),
    errorMessage
);

// Log detailed location for debugging
std::cerr << "Error at " << node->getLocation().toDetailedString(true) << std::endl;
```

### 5. Create Helper Methods for Common Location Patterns

If you find yourself repeatedly creating locations in a certain way, consider adding helper methods:

```cpp
// Helper to create a location for a binary operation
SourceLocation createBinaryOpLocation(
    const ExprNode* left,
    const Token& op,
    const ExprNode* right
) {
    return mergeLocations(left->getLocation(), right->getLocation());
}
```

## Common Pitfalls

### Losing Location Information

A common pitfall is losing source location information when creating new AST nodes or tokens:

```cpp
// Bad: Using default location loses information
std::unique_ptr<ExprNode> expr = std::make_unique<LiteralExprNode>(
    SourceLocation(),
    value
);

// Good: Propagating the location
std::unique_ptr<ExprNode> expr = std::make_unique<LiteralExprNode>(
    token.location,
    value
);
```

### Using Invalid Locations

Another common issue is using invalid locations without checking:

```cpp
// Bad: Might crash if location is invalid
std::string locationStr = location.toString();
ErrorHandler::getInstance().reportError(location, "Error message");

// Good: Check validity
if (location.isValid()) {
    ErrorHandler::getInstance().reportError(location, "Error message");
} else {
    ErrorHandler::getInstance().reportError(
        createBestLocation(currentFile), 
        "Error message (location unknown)"
    );
}
```

### Not Handling Exceptions

Some SourceLocation methods can throw exceptions. Always handle them appropriately:

```cpp
// Bad: Might throw
SourceLocation range = location1.createRange(location2);

// Good: Handle exceptions
try {
    SourceLocation range = location1.createRange(location2);
    // Use the range
} catch (const std::invalid_argument& e) {
    // Fall back to a simpler approach
    SourceLocation fallback = location1;
    // Use the fallback
}

// Alternative: Use the safer helper function
SourceLocation range = mergeLocations(location1, location2);
// This will never throw
```

## Integration with Error Handler

The SourceLocation system is designed to work seamlessly with the ErrorHandler class. When reporting errors, always provide the most specific source location available:

```cpp
// Report an error with a specific location
ErrorHandler::getInstance().reportError(
    node->getLocation(),
    "Invalid operation for type " + typeStr
);

// Report a warning with a specific location
ErrorHandler::getInstance().reportWarning(
    token.location,
    "Deprecated syntax - use the new form instead"
);

// Report a fatal error with a specific location
ErrorHandler::getInstance().reportFatalError(
    expr->getLocation(),
    "Unrecoverable type mismatch"
);
```

## Extending the System

If you need to extend the SourceLocation system, consider:

1. Adding new helper methods to create locations for specific syntax constructs.
2. Enhancing the `toDetailedString` method to provide more context.
3. Adding support for multi-file ranges (for include/import scenarios).
4. Integrating with an IDE or debugging tool for visual highlighting.

## Conclusion

The SourceLocation system is a critical component of the 4ever codebase, enabling precise error reporting and debugging. By following these guidelines, you can ensure that source locations are tracked consistently and accurately throughout the codebase, leading to better error messages and debugging experience for users.