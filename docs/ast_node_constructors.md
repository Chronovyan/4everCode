---
title: AST Node Constructor Guidelines
description: Documentation for ast_node_constructors.md
weight: 100
draft: true
---

# AST Node Constructor Guidelines

This document outlines the standardized approach for implementing AST node constructors in the Chronovyan codebase. Following these guidelines will help maintain consistency and prevent common errors such as constructor redefinition.

## Constructor Pattern Guidelines

### 1. Location Parameter Handling

All AST nodes should follow one of these approaches for handling source locations:

- **Primary Constructor**: Include an explicit `SourceLocation` parameter as the first parameter.
- **Convenience Constructors**: Derive the location from child nodes, or use a default location.
- **Delegation**: Convenience constructors should delegate to the primary constructor.

Example:
```cpp
// Primary constructor with explicit location
NodeType(const SourceLocation& location, /* other parameters */) 
    : BaseNode(location), /* initialize other members */ {}

// Convenience constructor deriving location from a child
NodeType(std::unique_ptr<ChildNode> child, /* other parameters */)
    : NodeType(child ? child->getLocation() : SourceLocation(), 
              std::move(child), /* other parameters */) {}
```

### 2. Constructor Implementation Location

- **Simple Constructors**: Define inline in the header file.
- **Complex Constructors**: Declare in the header, implement in the source file.
- **Never duplicate**: Never implement the same constructor in both header and source files.

### 3. Parameter Handling

- **Ownership Transfer**: Use `std::move` for transferring ownership of unique_ptr parameters.
- **Value Parameters**: Pass non-pointer value parameters by const reference or use std::move as appropriate.
- **Optional Parameters**: Use nullptr as default for optional pointer parameters.

Example:
```cpp
NodeType(
    const SourceLocation& location,
    std::string name,                                 // Will be moved
    std::unique_ptr<TypeNode> type,                   // Will be moved
    std::vector<std::unique_ptr<ExprNode>> arguments, // Will be moved
    std::unique_ptr<BlockStmtNode> body = nullptr     // Optional parameter
) : BaseNode(location),
    m_name(std::move(name)),
    m_type(std::move(type)),
    m_arguments(std::move(arguments)),
    m_body(std::move(body)) {}
```

### 4. Documentation

- Document the purpose and parameters of each constructor.
- When a constructor uses a default/derived location, clearly document this behavior.

Example:
```cpp
/**
 * @brief Construct a node with a specific source location
 * @param location The source location
 * @param name The node name
 */
NodeType(const SourceLocation& location, std::string name);

/**
 * @brief Backward-compatible constructor (uses a default source location)
 * @param name The node name
 */
explicit NodeType(std::string name);
```

## Common Patterns

### Nodes with Child Expressions/Statements

For nodes containing child expressions or statements, derive the location from the first child if possible:

```cpp
ExprStmtNode::ExprStmtNode(std::unique_ptr<ExprNode> expression)
    : StmtNode(expression ? expression->getLocation() : SourceLocation()),
      m_expression(std::move(expression)) {}
```

### Nodes with Multiple Children

For nodes with multiple children, use the location of the first child or a provided token:

```cpp
BinaryExprNode::BinaryExprNode(
    std::unique_ptr<ExprNode> left,
    Token op,
    std::unique_ptr<ExprNode> right
) : ExprNode(op.location),  // Use the operator's location
    m_left(std::move(left)),
    m_operator(std::move(op)),
    m_right(std::move(right)) {}
```

## Benefit of This Approach

- **Prevents Redefinition Errors**: By ensuring constructors are defined in only one place.
- **Improves Maintainability**: Changes to constructor signatures only need to be made in one place.
- **Enhances Clarity**: Makes it clear how source locations are derived and used.
- **Reduces Bugs**: Standardized patterns help prevent common errors.