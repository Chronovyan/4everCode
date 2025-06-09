---
title: 4ever Error Handling and Source Location System
description: Documentation for error_handling_consolidated.md
weight: 100
draft: true
---

# 4ever Error Handling and Source Location System

> **Version**: 1.0.0  
> **Last Updated**: 2023-10-28  
>
> **Navigation**:  
> [README](../README.md) | [Implementation Guide](./implementation_guide.md) | [API Reference](./api_reference.md)

## Table of Contents

- [1. System Overview](#1-system-overview)
  - [1.1 Purpose](#11-purpose)
  - [1.2 Key Components](#12-key-components)
  - [1.3 Design Philosophy](#13-design-philosophy)
- [2. Source Location System](#2-source-location-system)
  - [2.1 Core Components](#21-core-components)
  - [2.2 Creating and Managing Locations](#22-creating-and-managing-locations)
  - [2.3 Location Ranges](#23-location-ranges)
  - [2.4 Detailed Representation](#24-detailed-representation)
  - [2.5 Best Practices](#25-best-practices)
  - [2.6 Common Pitfalls](#26-common-pitfalls)
- [3. Error Handling System](#3-error-handling-system)
  - [3.1 Core Components](#31-core-components)
  - [3.2 Error Types and Severity](#32-error-types-and-severity)
  - [3.3 Error Reporting Methods](#33-error-reporting-methods)
  - [3.4 Error Tracking and Management](#34-error-tracking-and-management)
  - [3.5 Custom Error Handling](#35-custom-error-handling)
  - [3.6 Best Practices](#36-best-practices)
- [4. Temporal Paradoxes and Glitches](#4-temporal-paradoxes-and-glitches)
  - [4.1 Paradox Events](#41-paradox-events)
  - [4.2 Glitch Events](#42-glitch-events)
  - [4.3 Resolution and Fixing](#43-resolution-and-fixing)
- [5. Integration Examples](#5-integration-examples)
  - [5.1 Basic Error Reporting](#51-basic-error-reporting)
  - [5.2 Advanced Error Handling](#52-advanced-error-handling)
  - [5.3 Paradox Management](#53-paradox-management)
- [6. Future Enhancements](#6-future-enhancements)
  - [6.1 Planned Improvements](#61-planned-improvements)
  - [6.2 Research Areas](#62-research-areas)

---

## 1. System Overview

*In the complex tapestry of temporal weaving, errors are not mere bugs—they are disruptions in the fabric of computational time itself. The 4ever Error Handling System serves as the loom upon which these disruptions are identified, contained, and mended.*

### 1.1 Purpose

The 4ever Error Handling System provides a robust infrastructure for error reporting, management, and debugging. Together with the enhanced Source Location system, it enables precise pinpointing of issues in the source code and detailed error messages that make debugging easier for both developers and users.

> **Lore Tidbit**: The First Weavers' earliest attempts at complex temporal operations often resulted in cascading paradoxes and timeline collapses. The Error Handling System is the codified wisdom born from those catastrophic failures, designed to detect instabilities before they threaten the entire program continuum.

### 1.2 Key Components

The error handling and source location system consists of several interrelated components:

1. **Source Location Infrastructure**:
   - `SourceLocation`: Tracks precise coordinates in source files
   - `SourceFile`: Represents a source file with utility methods
   - Location utilities for creating ranges and comparing positions

2. **Error Management**:
   - `ErrorHandler`: Singleton manager for all error-related functionality
   - Error reporting and tracking mechanisms
   - Custom error callbacks for specialized handling

3. **Temporal Disruption Tracking**:
   - `ParadoxEvent`: Represents temporal paradoxes of varying severity
   - `GlitchEvent`: Represents minor temporal instabilities
   - Resolution and stabilization mechanisms

### 1.3 Design Philosophy

The 4ever error handling system is designed around these key principles:

1. **Precision**: Errors must be pinpointed with exact source locations for effective debugging.
2. **Context**: Error messages should provide rich context, including source code snippets.
3. **Categorization**: Different types of issues (errors, warnings, paradoxes, glitches) need different handling.
4. **Extensibility**: The system should be adaptable through custom callbacks and handlers.
5. **Recovery**: When possible, the system should facilitate recovery from non-fatal errors.

---

## 2. Source Location System

*The Source Location System is the cartography of our code—it maps the terrain of our temporal algorithms and allows us to precisely navigate to the sources of disruption.*

### 2.1 Core Components

The source location system consists of the following main components:

1. **SourceLocation**: A struct representing a location in the source code:
   - File reference
   - Line number
   - Column number
   - Absolute position
   - Start and end positions (for ranges)

2. **SourceFile**: A class representing a source file with utility methods:
   - Line and column tracking
   - Content access
   - Position calculations

3. **Helper Functions**: Utility functions for manipulating locations:
   - `createBestLocation`: Creates the best location from available information
   - `mergeLocations`: Creates a range spanning multiple locations
   - Location comparison utilities

### 2.2 Creating and Managing Locations

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

Always check if a source location is valid before using it in critical operations:

```cpp
if (location.isValid()) {
    // Use the location
} else {
    // Fall back to a default behavior
}
```

### 2.3 Location Ranges

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

Comparing locations can be done with the provided methods:

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

### 2.4 Detailed Representation

The `toDetailedString` method provides rich context for a location, including the actual source code:

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

### 2.5 Best Practices

1. **Always Propagate Source Locations**: When creating AST nodes, tokens, or other syntax elements, always propagate the source location information:

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

2. **Use mergeLocations for Compound Expressions**: When creating nodes that span multiple tokens or expressions, use `mergeLocations` to create a location that covers the entire range:

```cpp
// Create a location that spans from the start of the condition to the end of the else branch
SourceLocation ifStmtLocation = mergeLocations(
    condition->getLocation(),
    elseBranch ? elseBranch->getLocation() : thenBranch->getLocation()
);

// Create the if statement node with the merged location
return std::make_unique<IfStmtNode>(ifStmtLocation, std::move(condition), std::move(thenBranch), std::move(elseBranch));
```

3. **Never Use Default Locations for Errors**: Always provide a valid source location when reporting errors:

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

4. **Use Detailed Locations for Important Errors**: For important errors, use detailed location information:

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

5. **Create Helper Methods for Common Location Patterns**: If you find yourself repeatedly creating locations in a certain way, consider adding helper methods:

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

### 2.6 Common Pitfalls

1. **Losing Location Information**: A common pitfall is losing source location information when creating new AST nodes or tokens:

```cpp
// Bad: Using default location loses information
std::unique_ptr<ExprNode> expr = std::make_unique<LiteralExprNode>(
    SourceLocation(),
    value
);

// Good: Propagating the location from the source
std::unique_ptr<ExprNode> expr = std::make_unique<LiteralExprNode>(
    token.location,
    value
);
```

2. **Ignoring Invalid Locations**: Always check if a location is valid before using it in critical operations:

```cpp
// Bad: Assuming the location is valid
std::string detailed = location.toDetailedString(true);

// Good: Checking if the location is valid
std::string detailed = location.isValid() 
    ? location.toDetailedString(true)
    : "Unknown location";
```

3. **Not Using Location Ranges**: Using point locations when range locations would provide better context:

```cpp
// Bad: Using only the start location
ErrorHandler::getInstance().reportError(
    ifStatement->getCondition()->getLocation(),
    "Invalid condition"
);

// Good: Using a range that covers the entire condition
ErrorHandler::getInstance().reportError(
    ifStatement->getConditionRange(),
    "Invalid condition"
);
```

---

## 3. Error Handling System

*The Error Handler is the vigilant sentinel that watches over the temporal operations, ready to sound the alarm when disturbances are detected in the timeline.*

### 3.1 Core Components

The error handling system consists of the following main components:

1. **ErrorHandler**: A singleton class that manages all error-related functionality:
   - Error reporting and tracking
   - Console output control
   - Callback management
   - Detailed reporting

2. **4everError**: A struct representing a standard error:
   - Severity level
   - Source location
   - Error message
   - Runtime context

3. **Exception Classes**: Various exception types for different error scenarios:
   - `4everException`: Base exception class
   - `ParsingException`: For parsing-related errors
   - `TypeException`: For type-related errors
   - `RuntimeException`: For runtime errors

### 3.2 Error Types and Severity

The system supports various types of issues with different severity levels:

1. **4everError**: Standard errors and warnings with severity levels:
   - **WARNING**: Non-critical issues that don't prevent execution
   - **ERROR**: Problems that prevent execution but allow recovery
   - **FATAL**: Unrecoverable errors that require termination

2. **ParadoxEvent**: Special errors related to temporal paradoxes:
   - Severity scale from 1-10
   - Different types of paradoxes (TIMELINE_CONFLICT, CAUSALITY_VIOLATION, etc.)

3. **GlitchEvent**: Temporal glitches that may or may not need fixing:
   - Severity scale from 0.0-1.0
   - Different types of glitches (VARIABLE_INSTABILITY, RESOURCE_FLICKER, etc.)

### 3.3 Error Reporting Methods

The `ErrorHandler` provides various methods for reporting different types of issues:

```cpp
// Report an error with default severity (ERROR)
int errorId = ErrorHandler::getInstance().reportError(
    sourceLocation,  // Where the error occurred
    "Something went wrong"  // Error message
);

// Report a warning
int warningId = ErrorHandler::getInstance().reportWarning(
    sourceLocation,
    "This might cause problems later"
);

// Report a fatal error (throws an exception)
ErrorHandler::getInstance().reportFatalError(
    sourceLocation,
    "Critical error occurred"
);

// Report a paradox
int paradoxId = ErrorHandler::getInstance().reportParadox(
    sourceLocation,
    "Timeline conflict detected",
    ParadoxType::TIMELINE_CONFLICT,
    7  // Severity level (1-10)
);

// Report a glitch
int glitchId = ErrorHandler::getInstance().reportGlitch(
    sourceLocation,
    "Variable instability detected",
    GlitchType::VARIABLE_INSTABILITY,
    0.75  // Severity (0.0-1.0)
);
```

### 3.4 Error Tracking and Management

The ErrorHandler provides methods for tracking and managing errors:

```cpp
// Check if there are errors (excluding warnings)
bool hasErrors = ErrorHandler::getInstance().hasErrors();

// Get all errors
const std::vector<4everError>& errors = ErrorHandler::getInstance().getErrors();

// Count errors of a specific severity
int errorCount = ErrorHandler::getInstance().countErrors(ErrorSeverity::ERROR);

// Clear errors
ErrorHandler::getInstance().clearErrors();

// Get a detailed report of all errors
std::string report = ErrorHandler::getInstance().getReport(true);  // Include warnings
```

For paradoxes and glitches, there are similar tracking methods:

```cpp
// Check if there are unresolved paradoxes
bool hasParadoxes = ErrorHandler::getInstance().hasUnresolvedParadoxes();

// Get all paradoxes
const std::vector<ParadoxEvent>& paradoxes = ErrorHandler::getInstance().getParadoxes();

// Resolve a paradox
bool resolved = ErrorHandler::getInstance().resolveParadox(paradoxId);

// Check if there are unfixed glitches
bool hasGlitches = ErrorHandler::getInstance().hasUnfixedGlitches();

// Fix a glitch
bool fixed = ErrorHandler::getInstance().fixGlitch(glitchId);
```

### 3.5 Custom Error Handling

You can register custom callbacks for different types of issues:

```cpp
// Register an error callback
int callbackId = ErrorHandler::getInstance().registerErrorCallback(
    [](const 4everError& error) {
        // Custom error handling logic
        std::cout << "Custom handler: " << error.toString() << std::endl;
        return true;  // Return true if handled
    }
);

// Register a paradox callback
int paradoxCallbackId = ErrorHandler::getInstance().registerParadoxCallback(
    [](const ParadoxEvent& paradox) {
        // Custom paradox handling logic
        std::cout << "Custom handler: " << paradox.toString() << std::endl;
        return true;  // Return true if handled
    }
);

// Unregister a callback when no longer needed
ErrorHandler::getInstance().unregisterErrorCallback(callbackId);
```

### 3.6 Best Practices

1. **Use Appropriate Severity Levels**: Choose the right severity level for each issue:
   - **WARNING**: For potential issues that don't prevent execution
   - **ERROR**: For issues that prevent execution but allow recovery
   - **FATAL**: For unrecoverable errors that require termination

2. **Provide Detailed Error Messages**: Include enough information to understand and fix the issue:

```cpp
// Bad: Vague error message
ErrorHandler::getInstance().reportError(location, "Invalid operation");

// Good: Detailed error message
ErrorHandler::getInstance().reportError(
    location,
    "Invalid operation: cannot apply operator '+' to types 'STRING' and 'INT'"
);
```

3. **Use Runtime Context**: Set the runtime context to provide additional information:

```cpp
// Set the runtime context before a complex operation
ErrorHandler::getInstance().setRuntimeContext("Parsing function definitions");

// Error messages will include this context
ErrorHandler::getInstance().reportError(location, "Unexpected token");

// Clear the context when the operation is complete
ErrorHandler::getInstance().clearRuntimeContext();
```

4. **Check for Errors After Critical Operations**: Always check if errors occurred after critical operations:

```cpp
// Parse a complex expression
parseExpression();

// Check if errors occurred
if (ErrorHandler::getInstance().hasErrors()) {
    // Handle the errors or return early
    return nullptr;
}
```

5. **Use Appropriate Callback Registration**: Register callbacks in the appropriate scope:

```cpp
// Bad: Global callback that might interfere with other code
ErrorHandler::getInstance().registerErrorCallback([](const 4everError&) { return true; });

// Good: Scoped callback with specific handling
class Parser {
public:
    Parser() {
        m_callbackId = ErrorHandler::getInstance().registerErrorCallback(
            [this](const 4everError& error) {
                return this->handleError(error);
            }
        );
    }
    
    ~Parser() {
        // Unregister when no longer needed
        ErrorHandler::getInstance().unregisterErrorCallback(m_callbackId);
    }
    
private:
    bool handleError(const 4everError& error) {
        // Parser-specific error handling
        return false;  // Let other handlers process it too
    }
    
    int m_callbackId;
};
```

---

## 4. Temporal Paradoxes and Glitches

*The manipulation of computational time inevitably produces anomalies—paradoxes that threaten stability and glitches that reveal the seams in our temporal fabric.*

### 4.1 Paradox Events

Paradoxes are serious temporal anomalies that can destabilize the program if left unresolved. They are tracked using the `ParadoxEvent` struct:

```cpp
struct ParadoxEvent {
    int id;                  // Unique identifier
    SourceLocation location; // Where the paradox occurred
    std::string message;     // Description of the paradox
    ParadoxType type;        // The type of paradox
    int severity;            // Severity level (1-10)
    bool resolved;           // Whether the paradox has been resolved
    std::string resolution;  // Description of how it was resolved (if applicable)
};
```

Different types of paradoxes require different handling strategies:

| Paradox Type | Description | Typical Severity | Common Handling Strategy |
|--------------|-------------|------------------|--------------------------|
| TIMELINE_CONFLICT | Conflicting changes to the same timeline | 7-9 | Timeline separation or conflict resolution |
| CAUSALITY_VIOLATION | Effect occurs before cause | 5-8 | Reordering operations or enforcing sequence |
| RESOURCE_EXHAUSTION | Temporal resources depleted | 4-6 | Resource regeneration or scaling back operations |
| IDENTITY_FRAGMENTATION | Object identity becomes ambiguous | 6-8 | Identity reinforcement or object isolation |
| LOOP_INSTABILITY | Unstable temporal loop | 3-7 | Loop stabilization or early termination |

### 4.2 Glitch Events

Glitches are minor temporal anomalies that may cause unexpected behavior but don't typically threaten system stability. They are tracked using the `GlitchEvent` struct:

```cpp
struct GlitchEvent {
    int id;                  // Unique identifier
    SourceLocation location; // Where the glitch occurred
    std::string message;     // Description of the glitch
    GlitchType type;         // The type of glitch
    float severity;          // Severity level (0.0-1.0)
    bool fixed;              // Whether the glitch has been fixed
    std::string resolution;  // Description of how it was fixed (if applicable)
};
```

Different types of glitches require different approaches:

| Glitch Type | Description | Typical Severity | Common Handling Strategy |
|-------------|-------------|------------------|--------------------------|
| VARIABLE_INSTABILITY | Temporary value fluctuation | 0.3-0.6 | Value stabilization or recalculation |
| RESOURCE_FLICKER | Brief resource fluctuation | 0.1-0.4 | Resource buffering or ignoring if transient |
| TEMPORAL_ECHO | Duplicate operation effects | 0.4-0.7 | Deduplication or operation tracking |
| QUANTUM_UNCERTAINTY | Indeterminate state | 0.2-0.5 | State measurement or probabilistic resolution |
| PATTERN_DISRUPTION | Corrupted pattern | 0.5-0.8 | Pattern regeneration or alternative pattern |

### 4.3 Resolution and Fixing

Paradoxes must be resolved to maintain system stability:

```cpp
// Report a paradox
int paradoxId = ErrorHandler::getInstance().reportParadox(
    location,
    "Timeline conflict detected",
    ParadoxType::TIMELINE_CONFLICT,
    7
);

// Resolve the paradox
bool resolved = ErrorHandler::getInstance().resolveParadox(
    paradoxId,
    "Applied timeline isolation protocol"  // How it was resolved
);
```

Glitches should be fixed when possible:

```cpp
// Report a glitch
int glitchId = ErrorHandler::getInstance().reportGlitch(
    location,
    "Variable value temporarily unstable",
    GlitchType::VARIABLE_INSTABILITY,
    0.4
);

// Fix the glitch
bool fixed = ErrorHandler::getInstance().fixGlitch(
    glitchId,
    "Stabilized variable with quantum measurement"  // How it was fixed
);
```

---

## 5. Integration Examples

### 5.1 Basic Error Reporting

```cpp
// In a parser
Token expectToken(TokenType type) {
    Token token = lexer.nextToken();
    if (token.type != type) {
        ErrorHandler::getInstance().reportError(
            token.location,
            "Expected token of type " + tokenTypeToString(type) +
            " but found " + tokenTypeToString(token.type)
        );
    }
    return token;
}

// In a type checker
void checkTypes(ExprNode* expr, Type expectedType) {
    Type actualType = expr->getType();
    if (!isCompatible(actualType, expectedType)) {
        ErrorHandler::getInstance().reportError(
            expr->getLocation(),
            "Type mismatch: expected " + typeToString(expectedType) +
            " but found " + typeToString(actualType)
        );
    }
}
```

### 5.2 Advanced Error Handling

```cpp
// Custom error handler for recovery
class RecoveringParser {
public:
    RecoveringParser() {
        // Register error callback
        m_callbackId = ErrorHandler::getInstance().registerErrorCallback(
            [this](const 4everError& error) {
                return this->handleError(error);
            }
        );
    }
    
    ~RecoveringParser() {
        // Unregister callback
        ErrorHandler::getInstance().unregisterErrorCallback(m_callbackId);
    }
    
    std::unique_ptr<ProgramNode> parse() {
        try {
            // Set the runtime context
            ErrorHandler::getInstance().setRuntimeContext("Program parsing");
            
            auto program = parseProgram();
            
            // Clear the runtime context
            ErrorHandler::getInstance().clearRuntimeContext();
            
            return program;
        }
        catch (const ParsingException& e) {
            // Log the exception
            std::cerr << "Exception during parsing: " << e.what() << std::endl;
            
            // Return a partial program if possible
            return m_recoveryMode ? std::move(m_partialProgram) : nullptr;
        }
    }
    
private:
    bool handleError(const 4everError& error) {
        // Skip to the next statement on syntax errors
        if (error.severity == ErrorSeverity::ERROR) {
            skipToNextStatement();
            m_recoveryMode = true;
            return true;  // Error handled
        }
        return false;  // Let other handlers process it
    }
    
    void skipToNextStatement() {
        // Skip tokens until a statement delimiter is found
        while (lexer.peek().type != TokenType::SEMICOLON &&
               lexer.peek().type != TokenType::EOF_TOKEN) {
            lexer.nextToken();
        }
        
        // Consume the delimiter if it's a semicolon
        if (lexer.peek().type == TokenType::SEMICOLON) {
            lexer.nextToken();
        }
    }
    
    int m_callbackId;
    bool m_recoveryMode = false;
    std::unique_ptr<ProgramNode> m_partialProgram;
};
```

### 5.3 Paradox Management

```cpp
// Timeline management with paradox tracking
class TimelineManager {
public:
    bool branchTimeline(const std::string& timelineId) {
        // Check if the timeline already exists
        if (m_timelines.find(timelineId) != m_timelines.end()) {
            // Report a paradox
            int paradoxId = ErrorHandler::getInstance().reportParadox(
                SourceLocation(),  // No specific location
                "Cannot branch to timeline that already exists: " + timelineId,
                ParadoxType::TIMELINE_CONFLICT,
                8
            );
            
            // Try to resolve by creating a unique timeline ID
            std::string newId = timelineId + "_" + std::to_string(m_nextUniqueId++);
            m_timelines.insert(newId);
            
            // Mark the paradox as resolved
            ErrorHandler::getInstance().resolveParadox(
                paradoxId,
                "Created unique timeline ID: " + newId
            );
            
            return true;
        }
        
        // Normal case: create the timeline
        m_timelines.insert(timelineId);
        return true;
    }
    
private:
    std::set<std::string> m_timelines;
    int m_nextUniqueId = 1;
};
```

---

## 6. Future Enhancements

### 6.1 Planned Improvements

The error handling and source location system will continue to evolve with these planned enhancements:

1. **IDE Integration**: Support for generating error messages that can be directly clicked in an IDE.
2. **Error Categories**: Categorization of errors for better filtering and handling.
3. **Error Recovery Suggestions**: Providing suggestions for how to fix common errors.
4. **Multi-file Support**: Better handling of errors that span multiple files (e.g., through includes).
5. **Error Suppression**: Mechanism to suppress specific types of errors in certain contexts.

### 6.2 Research Areas

Several research areas are being explored for future versions:

1. **Predictive Error Detection**: Using static analysis to detect potential errors before they occur.
2. **Adaptive Error Thresholds**: Dynamically adjusting error reporting based on code complexity and context.
3. **Paradox Propagation Modeling**: Predicting how paradoxes might spread through a system over time.
4. **Glitch Pattern Recognition**: Identifying patterns of glitches that might indicate larger issues.
5. **Temporal Resilience Metrics**: Quantifying how resilient code is to temporal disruptions.

---

*"To master temporal weaving, one must first master the art of error detection and correction. Only then can the true patterns of time be safely manipulated."* — The Anchor's Creed, Section 7.3