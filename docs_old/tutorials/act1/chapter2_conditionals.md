---
title: 'Act I, Chapter 2: First Interactions'
description: Documentation for tutorials\act1\chapter2_conditionals.md
weight: 140
draft: true
---

# Act I, Chapter 2: First Interactions

## The Crystal Pathway Challenge

### Narrative Context

*After surviving your first night in Chronovya, you discover a path leading to what appears to be an ancient structure in the distance. The path is lined with strange crystalline formations that glow with inner light. As you approach, you notice that some crystals are blue, others are red, and a few shimmer with a purple hue. A small device on your wrist—seemingly from Professor Thorne's lab—activates and displays a message.*

### Learning Objectives

- Understand conditional statements (if/else)
- Use comparison operators (>, <, ==, !=)
- Make decisions based on input data
- Chain multiple conditions

### Challenge Description

**The Safe Path Finder**

The wrist device contains a sensor that can analyze crystal properties. According to the readout, some crystals emit harmful radiation while others are safe. You need to program the device to guide you safely through the crystal field.

```
Device Readout:
-----------------------------
CHRONOVYAN FIELD NAVIGATOR
-----------------------------
Sensor detected crystal field ahead.
Properties identified:
- Blue crystals: Temperature below 0°C
- Red crystals: Temperature above 40°C
- Purple crystals: Temperature between 15-25°C

WARNING: Extreme temperatures can damage equipment!
Safe temperature range: 10°C - 30°C

Write a program to evaluate each crystal and indicate if it's safe to approach.
```

### Initial Code

```
// Crystal temperature is automatically detected by your wrist device
DECLARE CONF crystal_temperature : INT = 22;  // This is just a sample value

// Your code needs to set this variable to indicate if the crystal is safe
DECLARE CONF is_safe : BOOL = false;

// Evaluate temperature with detailed feedback
IF (crystal_temperature < 10) {
    is_safe = false;
    print("Warning! Crystal too cold (" + crystal_temperature + "°C). Avoid contact.");
} ELSE IF (crystal_temperature > 30) {
    is_safe = false;
    print("Warning! Crystal too hot (" + crystal_temperature + "°C). Avoid contact.");
} ELSE {
    is_safe = true;
    print("Crystal is safe. Temperature is " + crystal_temperature + "°C.");
}
```

### The Challenge

Write a program that:
1. Evaluates if a crystal's temperature is within the safe range (10°C - 30°C)
2. Sets the `is_safe` variable to `true` if safe, or keeps it as `false` if unsafe
3. Prints a message indicating whether the crystal is safe and why

### Example Solution 1: Basic Approach

```
// Crystal temperature is automatically detected by your wrist device
crystal_temperature = 22  // This is just a sample value

// Your code needs to set this variable to indicate if the crystal is safe
is_safe = false

// Evaluate if temperature is in safe range (10°C - 30°C)
if (crystal_temperature >= 10 && crystal_temperature <= 30) {
    is_safe = true
    print("Crystal is safe. Temperature is within acceptable range.")
} else {
    is_safe = false
    print("Warning! Unsafe crystal. Temperature outside safe range.")
}
```

### Example Solution 2: More Detailed Feedback

```
// Crystal temperature is automatically detected by your wrist device
crystal_temperature = 22  // This is just a sample value

// Your code needs to set this variable to indicate if the crystal is safe
is_safe = false

// Evaluate temperature with detailed feedback
if (crystal_temperature < 10) {
    is_safe = false
    print("Warning! Crystal too cold (" + crystal_temperature + "°C). Avoid contact.")
} else if (crystal_temperature > 30) {
    is_safe = false
    print("Warning! Crystal too hot (" + crystal_temperature + "°C). Avoid contact.")
} else {
    is_safe = true
    print("Crystal is safe. Temperature is " + crystal_temperature + "°C.")
}
```

### Test Cases

The game will automatically test your code with multiple crystal temperatures:

1. **Blue Crystal**: -5°C (should be unsafe)
2. **Purple Crystal**: 22°C (should be safe)
3. **Red Crystal**: 45°C (should be unsafe)
4. **Mystery Crystal**: 15°C (should be safe)
5. **Mystery Crystal**: 30°C (should be safe, edge case)
6. **Mystery Crystal**: 9°C (should be unsafe, edge case)

### Game Feedback

For each test case, the device will show:
- The crystal type and temperature
- Your code's determination (safe/unsafe)
- Whether your assessment was correct
- The path you should take based on your assessment

### Extended Challenge: Crystal Type Identification

Once the player masters basic conditionals, an extended challenge appears:

```
NAVIGATOR UPDATE AVAILABLE
Extended functionality: Crystal type identification

Task: Enhance your program to identify the crystal type based on temperature:
- If temperature is below 0°C: "Blue Crystal"
- If temperature is above 40°C: "Red Crystal"
- If temperature is between 15-25°C: "Purple Crystal"
- Otherwise: "Unknown Crystal Type"

Additionally, determine if each type is safe to approach.
```

### Extended Solution

```
// Crystal temperature is automatically detected by your wrist device
DECLARE CONF crystal_temperature : INT = 22;  // This is just a sample value

// Variables to be set by your code
DECLARE CONF is_safe : BOOL = false;
DECLARE CONF crystal_type : STRING = "Unknown";

// Identify crystal type based on temperature
IF (crystal_temperature < 0) {
    crystal_type = "Blue Crystal";
} ELSE IF (crystal_temperature > 40) {
    crystal_type = "Red Crystal";
} ELSE IF (crystal_temperature >= 15 && crystal_temperature <= 25) {
    crystal_type = "Purple Crystal";
} ELSE {
    crystal_type = "Unknown Crystal Type";
}

// Determine if crystal is safe (10°C - 30°C)
IF (crystal_temperature >= 10 && crystal_temperature <= 30) {
    is_safe = true;
    print("SAFE: " + crystal_type + " (" + crystal_temperature + "°C)");
    print("You can approach this crystal.");
} ELSE {
    is_safe = false;
    print("DANGER: " + crystal_type + " (" + crystal_temperature + "°C)");
    print("Avoid contact with this crystal!");
}
```

### Game Response

When the player successfully completes the challenge:

```
CHRONOVYAN FIELD NAVIGATOR
-----------------------------
Program verified: 6/6 crystals correctly assessed!

As you apply your new code, the wrist device projects a holographic path through 
the crystal field. Following this safe route, you navigate through the beautiful 
but potentially dangerous crystal formations.

The path leads to a small clearing where you find what appears to be an ancient 
terminal embedded in stone. Perhaps this will provide more answers about 
Chronovya...or even clues about Professor Thorne's whereabouts.

[NEW SKILL UNLOCKED: Conditional Logic]
[JOURNAL UPDATED: Crystal Types and Properties]
```

### Tutorial Notes

This puzzle introduces:
- The `if`, `else if`, and `else` syntax
- Comparison operators (`<`, `>`, `>=`, `<=`, `==`)
- Logical operators (`&&` for AND, `||` for OR)
- The concept of decision trees
- String concatenation with the `+` operator

Players learn that programs can make decisions based on data and take different actions accordingly. This is a fundamental concept that will be used throughout the game.

### Real-World Application

In the tutorial's context panel, explain how this relates to real-world programming:

```
REAL-WORLD APPLICATION
----------------------
Conditional logic is fundamental to all programming languages. It allows programs to:
- Control user access based on credentials
- Display different content based on user preferences
- Implement game rules and logic
- Handle errors and exceptions
- Process data differently based on its characteristics

Examples in everyday technology:
- Smart thermostats adjusting temperature based on conditions
- Banking apps approving or declining transactions
- Social media showing different content based on user interests
- Weather apps providing different forecasts based on location
```

### Next Challenge Preview

```
NAVIGATOR LOG:
The ancient terminal you've discovered seems to require repeated interactions 
to activate. Perhaps you need to learn about loops to efficiently communicate 
with this Chronovyan technology...
```