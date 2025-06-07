---
title: 'Act I, Chapter 3: Repeating Patterns'
description: Documentation for tutorials\act1\chapter3_loops.md
weight: 140
draft: true
---

# Act I, Chapter 3: Repeating Patterns

## The Ancient Terminal Challenge

### Narrative Context

*After navigating safely through the crystal field, you arrive at the ancient terminal embedded in stone. As you approach, it illuminates with a soft, amber glow. The terminal appears to be running a diagnostic sequence, but it's stuck in an endless initialization loop. To activate the terminal fully, you'll need to understand and repair its repeating pattern logic.*

### Learning Objectives

- Understand loop structures in Chronovyan
- Use `FOR` loops for counting and iteration
- Use `WHILE` loops for conditional repetition
- Break out of loops when appropriate
- Avoid infinite loops
- Use loop control variables

### Challenge Description

**The Repeating Signal**

The terminal's display shows a partially functioning initialization sequence. According to CORA's analysis, the terminal is attempting to send an activation signal, but it's not properly incrementing its counter, causing it to repeat the same signal indefinitely.

```
Terminal Display:
-----------------------------
CHRONOVYAN ARCHIVE TERMINAL
-----------------------------
Initialization sequence: PARTIAL FAILURE
Error detected: Signal repetition fault
System requires: 10 unique activation signals
Current status: Sending signal #1 repeatedly

To repair, modify the loop structure to correctly 
increment signal counter and terminate after 10 signals.
```

### Initial Code

```
// Terminal signal transmission system
DECLARE CONF signal_count : INT = 1;
DECLARE CONF max_signals : INT = 10;
DECLARE CONF is_complete : BOOL = false;

// Your code needs to:
// 1. Repeatedly send signals
// 2. Increment the counter
// 3. Stop after 10 signals

// Current faulty code (creates infinite loop):
WHILE (true) {
    print("Sending activation signal #" + signal_count);
    // Missing: signal_count increment
    // Missing: termination condition
}
```

### The Challenge

Write a program that:
1. Sends exactly 10 activation signals
2. Properly increments the signal counter after each transmission
3. Displays each signal number during transmission
4. Sets the `is_complete` flag to `true` when finished

### Example Solution 1: Using a WHILE Loop

```
// Terminal signal transmission system
DECLARE CONF signal_count : INT = 1;
DECLARE CONF max_signals : INT = 10;
DECLARE CONF is_complete : BOOL = false;

// Sending signals with a WHILE loop
WHILE (signal_count <= max_signals) {
    print("Sending activation signal #" + signal_count);
    signal_count = signal_count + 1;
}

// Set completion flag after loop terminates
is_complete = true;
print("Activation sequence complete!");
```

### Example Solution 2: Using a FOR Loop

```
// Terminal signal transmission system
DECLARE CONF max_signals : INT = 10;
DECLARE CONF is_complete : BOOL = false;

// Sending signals with a FOR loop
FOR (DECLARE CONF signal_count : INT = 1; signal_count <= max_signals; signal_count = signal_count + 1) {
    print("Sending activation signal #" + signal_count);
}

// Set completion flag after loop terminates
is_complete = true;
print("Activation sequence complete!");
```

### Test Cases

The terminal will test your code to ensure:
1. Exactly 10 signals are sent (no more, no less)
2. Each signal displays the correct number (1 through 10)
3. The `is_complete` flag is set to `true` at the end
4. The program terminates without manual intervention

### Game Feedback

```
CHRONOVYAN ARCHIVE TERMINAL
-----------------------------
Initialization sequence: SUCCESSFUL
Signal transmission: 10/10 completed
System status: ONLINE
Terminal access: GRANTED

The terminal's amber glow shifts to a steady blue light, and a holographic
interface materializes above it. Ancient Chronovyan text begins to scroll 
across the display, which CORA translates for you in real-time.

[SKILL UNLOCKED: Loop Structures]
[JOURNAL UPDATED: Ancient Terminal Records]
```

### Extended Challenge: Optimized Signal Pattern

After the basic activation, the terminal offers an advanced challenge:

```
TERMINAL NOTIFICATION:
Enhanced functionality available.
Optimization required: Transmit only even-numbered signals
to reduce power consumption while maintaining system integrity.
```

### Extended Solution

```
// Terminal signal transmission system
DECLARE CONF max_signals : INT = 10;
DECLARE CONF is_complete : BOOL = false;

// Send only even-numbered signals to conserve power
FOR (DECLARE CONF signal_count : INT = 2; signal_count <= max_signals; signal_count = signal_count + 2) {
    print("Sending optimized activation signal #" + signal_count);
}

// Set completion flag after loop terminates
is_complete = true;
print("Optimized activation sequence complete!");
```

### Game Response to Extended Challenge

```
CHRONOVYAN ARCHIVE TERMINAL
-----------------------------
Optimization analysis: SUCCESSFUL
Power consumption: REDUCED BY 50%
Enhanced functionality: ENABLED

The terminal's interface expands, revealing additional data sections
and control options. A small compartment in the stone pedestal slides
open, containing what appears to be a data crystal.

[NEW ITEM: Ancient Data Crystal]
[POWER CONSERVATION TECHNIQUE LEARNED]
```

### Tutorial Notes

This puzzle introduces:
- The `WHILE` loop structure for condition-based repetition
- The `FOR` loop structure for counting and iteration
- The concept of loop control variables
- The importance of termination conditions
- How to avoid infinite loops
- Using loops for repeated tasks

Players learn that loops are essential for performing repetitive tasks efficiently and that proper control flow prevents programs from getting stuck in endless repetition.

### Real-World Application

```
REAL-WORLD APPLICATION
----------------------
Loops are fundamental to all programming and power countless applications:
- Data processing (analyzing each record in a database)
- Animation (updating each frame of movement)
- Web servers (continuously listening for new connections)
- Monitoring systems (regularly checking system status)

Examples in everyday technology:
- A fitness app counting your steps as you walk
- A music player going through each song in a playlist
- A search engine examining each webpage for relevant information
- A thermostat checking temperature at regular intervals
```

### Next Challenge Preview

```
CORA'S ANALYSIS:
The data crystal you've recovered contains fragmented information about
ancient Chronovyan technology. To decode it fully, you'll need to organize
the data using functions - modular code blocks that perform specific tasks.
The next terminal may hold the key to understanding function structures...
```