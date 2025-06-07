---
title: Error Handling and Source Location System
description: Documentation for error_handling_system.md
weight: 100
draft: true
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Error Handling and Source Location System

## Overview

The [Chronovyan](https://chronovyan.github.io/h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/) "The) temporal) programming) language) and) runtime") Error Handling System provides a robust infrastructure for error reporting, management, and debugging. Together with the enhanced Source Location system, it enables precise pinpointing of issues in the source code and detailed error messages that make debugging easier for both developers and users.

## Key Components

### 1. Source Location System

The enhanced `SourceLocation` struct now provides
*Precise Location Information**: Tracks file, line, column, and absolute position within the source.
- **Range Support**: Can represent a range of source code (from start location to end location).
- **Location Comparison**: Methods to compare locations (`isBefore`,`isAfter`, equality operators).
- **Detailed Representation**: The `toDetailedString` method provides context-rich location information including the actual line of code.
- **Location Merging**: The `mergeLocations` utility function creates ranges that span multiple locations.
- **Fallback Handling**: The `createBestLocation` function intelligently creates a location with whatever information is available.

### 2. Error Handler

The ErrorHandler class has been enhanced with
*Improved Error Reporting**: Methods for reporting errors, warnings, \1PARADOX\2/core/Core Concepts - The Foundation of Temporal Programming.md#paradox\3/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)p)a)r)a)d)o)x)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)[)p)a)r)a)d)o)x)])()/)c)o)r)e)/)c)o)n)c)e)p)t)s)#)p)a)r)a)d)o)x))))) "A temporal inconsistency) that) must) be) resolved""), and glitches with detailed location information.
- **Console Output Control**: Settings to control what gets printed to the console and at what severity level.
- **Detailed Error Messages**: Enhanced error formatting with source code context.
- **Error Callbacks**: Improved callback system with registration and removal capabilities.
- **Comprehensive Reporting**: The `getReport` method provides a detailed report of all errors, paradoxes, and glitches.
- **Error Counts**: Methods to count errors, paradoxes, and glitches with filtering options.

### 3. Error Types

The system supports various types of issues
*ChronovyanError**: Standard errors and warnings.
- **ParadoxEvent**: Special errors related to temporal paradoxes.
- **GlitchEvent**: Temporal glitches that may or may not need fixing.

## Major Enhancements

### Source Location Enhancements

1. **Location Ranges**: SourceLocation can now represent a range of source code, not just a single point.
2. **Detailed String Representation**: The `toDetailedString` method shows the actual source code with a pointer to the issue location.
3. **Location Comparison**: New methods to compare locations in a semantically meaningful way.
4. **Location Utilities**: Helper functions for common location operations like merging and creating ranges.

### Error Handler Enhancements

1. **Console Output Control**: Settings to control what gets printed to the console.
2. **Callback Management**: Improved callback registration with unique IDs and removal capabilities.
3. **Best Location Creation**: The system now uses the best possible location information available.
4. **Detailed Reporting**: Enhanced reporting capabilities with filtering options.

## Integration Examples

###

``cpp
    // Create a source location range
    SourceLocation startLoc(sourceFile, 10, 5, 100);
    SourceLocation endLoc(sourceFile, 12, 10, 150);
    SourceLocation range = startLoc.createRange(endLoc);

    // Get a detailed string representation with the source code
    std::string detailedLocation = range.toDetailedString(true);
```text

###

``cpp
    // Report an error with a specific location
    ErrorHandler::getInstance().reportError(
        node->getLocation(),
        "Invalid operation for type " + typeStr
    );

    // Report a fatal error with detailed location information
    ErrorHandler::getInstance().reportFatalError(
        expr->getLocation(),
        "Unrecoverable type mismatch"
    );
```text

###

``cpp
    // Register a callback for custom error handling
    ErrorHandler::getInstance().registerErrorCallback(
        [](const) ChronovyanError&) error) {
            // Custom error handling logic
            std::cerr << "Custom handler: " << error.toDetailedString(true) << std::endl;
            return true; // Error was handled
        }
    );
```text

## Best Practices

1. **Always Use Valid Locations**: Always provide the most specific source location possible when reporting errors.
2. **Check Location Validity**: Always check if a location is valid before using it in critical operations.
3. **Use Detailed Strings for Important Errors**: For important errors, use the detailed location representation.
4. **Propagate Locations**: When creating AST nodes or other syntax elements, always propagate source location information.
5. **Use Location Ranges**: For multi-token constructs, use location ranges to provide better context.

## Benefits

The enhanced error handling and source location system provides several key benefits:

1. **Improved Debugging**: Precise location information makes it easier to identify and fix issues.
2. **Better User Experience**: Detailed error messages help users understand and resolve problems.
3. **Consistent Error Reporting**: Standardized approach to error handling across the codebase.
4. **Flexible Error Management**: Callbacks and filtering options for customized error handling.
5. **Rich Context**: Source code snippets and detailed location information provide valuable context.

## Future Enhancements

Potential future enhancements to the system include:

1. **IDE Integration**: Support for generating error messages that can be directly clicked in an IDE.
2. **Error Categories**: Categorization of errors for better filtering and handling.
3. **Error Suppression**: Mechanism to suppress specific types of errors in certain contexts.
4. **Error Recovery Suggestions**: Providing suggestions for how to fix common errors.
5. **Multi-file Support**: Better handling of errors that span multiple files (e.g., through includes).