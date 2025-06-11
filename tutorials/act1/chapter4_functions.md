---
title: 'Act I, Chapter 4: Functional Fragments'
description: Documentation for tutorials\act1\chapter4_functions.md
weight: 140
draft: true
---

# Act I, Chapter 4: Functional Fragments

## The Data Crystal Challenge

### Narrative Context

*With the ancient data crystal recovered from the terminal, you seek a way to decode its contents. CORA identifies another structure in the distanceâ€”a small dome with a crystalline pedestal at its center. As you approach, the pedestal responds to the data crystal, projecting fragmented code sequences. To unlock the crystal's secrets, you'll need to organize these fragments into functional units.*

### Learning Objectives

- Understand function structure and purpose in 4ever
- Create functions with the `func` keyword
- Pass parameters to functions
- Return values from functions
- Call functions appropriately
- Organize code using functions
- Create modular, reusable code

### Challenge Description

**The Fragmented Code**

The crystal pedestal reveals scattered code fragments that appear to be part of a decryption system. CORA's analysis suggests that organizing these fragments into proper functions will unlock the crystal's encoded data.

```
Pedestal Display:
-----------------------------
4ever DATA DECRYPTION
-----------------------------
Fragment analysis: UNSTRUCTURED CODE DETECTED
Recommendation: Organize into functional units
Requirements: Create decryption functions for each data type
Current status: Awaiting code restructuring

Organize the scattered code fragments into proper functions
to decode the crystal's contents.
```

### Initial Code

```
// Data crystal decryption system
// These code fragments need to be organized into functions

// Fragment 1: Numerical decryption logic
value = raw_numerical_data * 2;
value = value - 5;
value = value / 3;
print("Decoded numerical value: " + value);

// Fragment 2: Text decryption logic
decrypted_text = "";
FOR (DECLARE CONF i : INT = 0; i < encrypted_text.length; i = i + 1) {
    character = encrypted_text.charAt(i);
    ascii_value = character.toAscii();
    shifted_ascii = ascii_value - 3;
    decrypted_text = decrypted_text + String.fromAscii(shifted_ascii);
}
print("Decoded text: " + decrypted_text);

// Fragment 3: Coordinate decryption logic
x_coord = raw_coordinates[0] / 10;
y_coord = raw_coordinates[1] / 10;
z_coord = raw_coordinates[2] / 10;
print("Decoded coordinates: [" + x_coord + ", " + y_coord + ", " + z_coord + "]");

// Main code - Current structure doesn't allow for reuse
DECLARE CONF raw_numerical_data : INT = 42;
DECLARE CONF encrypted_text : STRING = "Khoor#Zruog";
DECLARE CONF raw_coordinates : VECTOR = [1250, 760, 890];

// Need to reorganize fragments into functions and call them here
```

### The Challenge

Write a program that:
1. Creates three separate functions for each type of decryption
2. Passes the appropriate parameters to each function
3. Returns the decoded values from the functions
4. Calls the functions from the main code
5. Prints the results

### Example Solution

```
// Data crystal decryption system

// Function for numerical decryption
func decryptNumerical(DECLARE CONF raw_data : INT) : INT {
    DECLARE CONF value : INT = raw_data * 2;
    value = value - 5;
    value = value / 3;
    RETURN value;
}

// Function for text decryption
func decryptText(DECLARE CONF encrypted_text : STRING) : STRING {
    DECLARE CONF decrypted_text : STRING = "";
    FOR (DECLARE CONF i : INT = 0; i < encrypted_text.length; i = i + 1) {
        DECLARE CONF character : CHAR = encrypted_text.charAt(i);
        DECLARE CONF ascii_value : INT = character.toAscii();
        DECLARE CONF shifted_ascii : INT = ascii_value - 3;
        decrypted_text = decrypted_text + String.fromAscii(shifted_ascii);
    }
    RETURN decrypted_text;
}

// Function for coordinate decryption
func decryptCoordinates(DECLARE CONF raw_coordinates : VECTOR) : VECTOR {
    DECLARE CONF x_coord : FLOAT = raw_coordinates[0] / 10;
    DECLARE CONF y_coord : FLOAT = raw_coordinates[1] / 10;
    DECLARE CONF z_coord : FLOAT = raw_coordinates[2] / 10;
    RETURN [x_coord, y_coord, z_coord];
}

// Main code - Using functions for better organization
DECLARE CONF raw_numerical_data : INT = 42;
DECLARE CONF encrypted_text : STRING = "Khoor#Zruog";
DECLARE CONF raw_coordinates : VECTOR = [1250, 760, 890];

// Call the functions with appropriate parameters
DECLARE CONF decoded_number : INT = decryptNumerical(raw_numerical_data);
DECLARE CONF decoded_text : STRING = decryptText(encrypted_text);
DECLARE CONF decoded_coords : VECTOR = decryptCoordinates(raw_coordinates);

// Print the results
print("Decoded numerical value: " + decoded_number);
print("Decoded text: " + decoded_text);
print("Decoded coordinates: [" + decoded_coords[0] + ", " + decoded_coords[1] + ", " + decoded_coords[2] + "]");
```

### Test Cases

The pedestal will test your code with various inputs:
1. Numerical data: 42, 15, 78
2. Text strings: "Khoor#Zruog", "Fkurqrybdq", "Surihvvru#Wkruqh"
3. Coordinate sets: [1250, 760, 890], [2000, 1500, 3000], [500, 200, 100]

### Game Feedback

```
4ever DATA DECRYPTION
-----------------------------
Functional organization: SUCCESSFUL
Decryption performance: OPTIMAL
Crystal data: UNLOCKED

The data crystal emits a soft pulse of light as the decryption completes.
The pedestal projects a holographic map showing what appears to be a
facility located at the decoded coordinates. The text decryption reveals
messages written by Professor Thorne about a "Temporal Core" that may be
the key to stabilizing the fluctuating environment.

[SKILL UNLOCKED: Function Structure]
[JOURNAL UPDATED: Professor Thorne's Research Notes]
[MAP UPDATED: Temporal Core Facility Location]
```

### Extended Challenge: Function Optimization

After the basic decryption, the pedestal offers an advanced challenge:

```
PEDESTAL NOTIFICATION:
Advanced functionality detected.
Optimization opportunity: Create a universal decoder function
that determines the data type and calls the appropriate specialized function.
```

### Extended Solution

```
// Data crystal decryption system with unified interface

// Specialized decryption functions
func decryptNumerical(DECLARE CONF raw_data : INT) : INT {
    DECLARE CONF value : INT = raw_data * 2;
    value = value - 5;
    value = value / 3;
    RETURN value;
}

func decryptText(DECLARE CONF encrypted_text : STRING) : STRING {
    DECLARE CONF decrypted_text : STRING = "";
    FOR (DECLARE CONF i : INT = 0; i < encrypted_text.length; i = i + 1) {
        DECLARE CONF character : CHAR = encrypted_text.charAt(i);
        DECLARE CONF ascii_value : INT = character.toAscii();
        DECLARE CONF shifted_ascii : INT = ascii_value - 3;
        decrypted_text = decrypted_text + String.fromAscii(shifted_ascii);
    }
    RETURN decrypted_text;
}

func decryptCoordinates(DECLARE CONF raw_coordinates : VECTOR) : VECTOR {
    DECLARE CONF x_coord : FLOAT = raw_coordinates[0] / 10;
    DECLARE CONF y_coord : FLOAT = raw_coordinates[1] / 10;
    DECLARE CONF z_coord : FLOAT = raw_coordinates[2] / 10;
    RETURN [x_coord, y_coord, z_coord];
}

// Universal decryption function that determines type and calls appropriate function
func universalDecrypt(DECLARE CONF data : ANY) : ANY {
    // Check data type and call appropriate function
    IF (type(data) == "INT") {
        RETURN decryptNumerical(data);
    } ELSE IF (type(data) == "STRING") {
        RETURN decryptText(data);
    } ELSE IF (type(data) == "VECTOR") {
        RETURN decryptCoordinates(data);
    } ELSE {
        // Handle unsupported type
        print("Error: Unsupported data type for decryption");
        RETURN null;
    }
}

// Main code using universal decryption
DECLARE CONF raw_numerical_data : INT = 42;
DECLARE CONF encrypted_text : STRING = "Khoor#Zruog";
DECLARE CONF raw_coordinates : VECTOR = [1250, 760, 890];

// Use the universal function for all decryption
DECLARE CONF decoded_number : INT = universalDecrypt(raw_numerical_data);
DECLARE CONF decoded_text : STRING = universalDecrypt(encrypted_text);
DECLARE CONF decoded_coords : VECTOR = universalDecrypt(raw_coordinates);

// Print the results
print("Decoded numerical value: " + decoded_number);
print("Decoded text: " + decoded_text);
print("Decoded coordinates: [" + decoded_coords[0] + ", " + decoded_coords[1] + ", " + decoded_coords[2] + "]");
```

### Game Response to Extended Challenge

```
4ever DATA DECRYPTION
-----------------------------
Universal decoder: IMPLEMENTED
Functional hierarchy: OPTIMAL
Advanced data revealed: ACCESSING...

The pedestal's projection expands dramatically, revealing a complex
schematic of the Temporal Core and detailed notes from Professor Thorne.
According to the data, the professor discovered that the Core is
experiencing instability and created a stabilization protocol.
The crystal contains the exact sequence needed to restore balance.

[NEW SKILL: Functional Hierarchies]
[QUEST UNLOCKED: Core Stabilization]
```

### Tutorial Notes

This puzzle introduces:
- The `func` keyword for defining functions
- Parameter passing and type annotations
- Return values with the `RETURN` keyword
- Function calling syntax
- Code organization and reusability
- Function hierarchies (functions calling other functions)

Players learn that functions are essential for organizing code into reusable, modular components that make programs easier to understand, maintain, and extend.

### Real-World Application

```
REAL-WORLD APPLICATION
----------------------
Functions are the building blocks of organized, maintainable software:
- Software libraries provide functions for common tasks
- APIs expose functionality through well-defined function interfaces
- Complex programs are broken down into manageable functional units
- Team development relies on functional separation of concerns

Examples in everyday technology:
- A smartphone camera app using functions for different filters
- Banking software using functions to process different transaction types
- Navigation systems using functions to calculate optimal routes
- Social media platforms using functions to display different content types
```

### Next Challenge Preview

```
CORA'S ANALYSIS:
The coordinates and decryption protocol you've recovered lead to the
Temporal Core facility. According to Professor Thorne's notes, the
Core requires immediate stabilization to prevent further temporal
distortions. This will be your first major test, requiring all the
programming skills you've learned so far...
```