---
title: 'Prologue: The Lab Incident'
description: Documentation for tutorials\prologue\README.md
weight: 140
draft: true
---

# Prologue: The Lab Incident

## Overview

Welcome to the beginning of your journey into Chronovya and the Chronovyan programming language. In this prologue, you'll step into the shoes of Alex, a curious intern who discovers Professor Thorne's hidden research lab. Through exploring the lab and interacting with Thorne's computer terminal, you'll learn the basics of Chronovyan programming while unraveling the mystery of the professor's disappearance.

## Learning Objectives

By completing the Prologue, you'll be able to:
- Understand and use basic Chronovyan syntax
- Declare and assign values to variables
- Use simple pre-defined commands
- Read and modify existing code
- Execute sequences of Chronovyan code

## Story Synopsis

*Three weeks ago, the brilliant but eccentric Professor Thorne vanished without a trace. As his research intern, you've been searching for clues to his whereabouts. Following a cryptic email that arrived on a delayed send, you've discovered his hidden lab beneath the university physics building. Inside, you find strange equipment, mysterious notes, and a computer terminal running an unfamiliar programming language: Chronovyan.*

## Tutorial Walkthrough

### Chapter 1: The Hidden Lab

**Narrative:** Alex enters the lab for the first time, discovering strange equipment and Thorne's workstation.

**Challenge 1: Power Up the Terminal**

You notice Professor Thorne's terminal is in sleep mode. A sticky note on the monitor reads:

```
To wake terminal:
Run hello_chronovyan()
```

**Solution:**
```
hello_chronovyan()
```

**Game Response:** The terminal screen lights up with a welcome message:
```
"Welcome to Chronovyan Terminal v0.9.4 (Thorne Edition)
Timeline Stability: NOMINAL
Last user: Prof. J. Thorne
Last login: 21 days ago"
```

**Tutorial Notes:** This introduces the concept of function calls without parameters. The player learns that commands in Chronovyan are followed by parentheses to execute them.

### Chapter 2: First Steps in Chronovyan

**Narrative:** With the terminal active, you search through the professor's files for clues.

**Challenge 2: Access the Lab Log**

You notice a program called `lab_log` on the desktop. The terminal shows:

```
To access lab log, set user credential variables and run access_log()
Required variables:
- username (string)
- access_level (number)
```

**Solution:**
```
DECLARE CONF username : STRING = "Alex";
DECLARE CONF access_level : INT = 1;
access_log();
```

**Game Response:**
```
"Access granted to user: Alex (Level 1)
WARNING: Level 1 access only permits viewing recent entries.
For full access, level 3 required.

LATEST LOG ENTRY:
Day 342: The calculations are finally converging. The Chronovyan equations
predict a stable connection point. If my theory is correct, this could 
change everything we know about spacetime. I'm going to attempt a small-scale
test tonight. Just need to finish calibrating the temporal stabilizers..."
```

**Tutorial Notes:** This introduces variable declaration and assignment, as well as different data types (strings and numbers). The player learns that variables store information that can be used by functions.

### Chapter 3: Modifying Values

**Narrative:** You realize you need higher access to see more log entries that might reveal what happened to the professor.

**Challenge 3: Increase Access Level**

You find a note in the professor's drawer:

```
"If you're reading this, you may need to adjust security variables.
Remember: variables can be changed. Admin override: access_level = 3"
```

**Solution:**
```
access_level = 3;
access_log();
```

**Game Response:**
```
"Access granted to user: Alex (Level 3)
FULL LOG ACCESS ENABLED

EARLIER LOG ENTRY (Day 340):
The equations suggest that Chronovya exists in a parallel timeline, accessible
only through a specific quantum frequency. The language constructs I've been
developing should allow for direct manipulation of the timeline structures
if my theory is correct.

EARLIEST LOG ENTRY (Day 338):
I've decoded enough of the signal to create a basic syntax structure.
I'm calling it 'Chronovyan' after the apparent source of the transmission.
It appears to be both a programming language and a means of communication.
Remarkable how it uses temporal logic as its foundation rather than binary..."
```

**Tutorial Notes:** The player learns that variables can be reassigned with new values, and that changing variables can affect program behavior.

### Chapter 4: Combining Commands

**Narrative:** You discover a locked cabinet containing what appears to be experimental equipment.

**Challenge 4: Access the Equipment Cabinet**

A keypad on the cabinet has a small screen displaying:

```
"Cabinet Security System
Current status: LOCKED
To unlock, run the following sequence:
1. Set security_override to true
2. Set authorization_code to 7429
3. Run unlock_cabinet()"
```

**Solution:**
```
DECLARE CONF security_override : BOOL = true;
DECLARE CONF authorization_code : INT = 7429;
unlock_cabinet();
```

**Game Response:**
*The cabinet clicks and slides open, revealing a strange device with glowing components. It appears to be some kind of prototype. A small tablet next to it shows technical specifications for what it calls a "Chronovyan Rift Stabilizer (Prototype)"*

**Tutorial Notes:** This reinforces variable assignment with a new data type (boolean) and shows how a sequence of commands can be used to accomplish a goal.

### Chapter 5: Examining the Device

**Narrative:** You examine the prototype device, trying to understand its purpose.

**Challenge 5: Activate Diagnostic Mode**

The tablet connected to the device shows:

```
"Chronovyan Rift Stabilizer - Diagnostic Interface
To run diagnostics, set the following parameters:
- power_level (range 1-10)
- target_frequency (value from Thorne's notes: 428.7)
- diagnostic_mode (set to true)
Then run start_diagnostic()"
```

Looking around, you find Thorne's notes indicating he was using power_level 8 in his tests.

**Solution:**
```
DECLARE CONF power_level : INT = 8;
DECLARE CONF target_frequency : FLOAT = 428.7;
DECLARE CONF diagnostic_mode : BOOL = true;
start_diagnostic();
```

**Game Response:**
```
"DIAGNOSTIC RUNNING...
Power level: 8 (CAUTION: HIGH)
Target frequency: 428.7 MHz (MATCH: CHRONOVYA RESONANCE)
Diagnostic mode: ACTIVE

WARNING: Temporal field detected! Anomaly forming...
System detecting temporal instability!
Emergency shutdown recommended!

To abort: run abort_sequence()"
```

**Tutorial Notes:** This combines all concepts learned so far, showing how variables of different types can be used together to configure a complex system.

### Final Challenge: The Portal Opens

**Narrative:** The device begins to hum loudly, and a swirling vortex of light forms above it. You realize this might be what happened to Professor Thorne!

**Challenge 6: Attempt to Shut Down or Investigate**

The tablet screen flashes urgently:

```
"CRITICAL WARNING: TEMPORAL RIFT FORMING
Options:
1. abort_sequence() - Emergency shutdown
2. stabilize_field() - Attempt to maintain current configuration
3. increase_power() - WARNING: Untested, may increase rift size

Choose wisely!"
```

**Solutions (multiple paths):**

Path 1 (try to shut down):
```
abort_sequence()
```

Game Response:
```
"ABORT SEQUENCE INITIATED...
ERROR: FIELD SELF-SUSTAINING
SHUTDOWN FAILURE
Portal expanding..."
```

Path 2 (try to stabilize):
```
stabilize_field()
```

Game Response:
```
"STABILIZATION SEQUENCE RUNNING...
Partial stabilization achieved.
Portal stable but active.
Sensors detecting habitable environment beyond threshold."
```

Path 3 (increase power):
```
power_level = 10;
increase_power();
```

Game Response:
```
"POWER LEVEL INCREASED...
WARNING: TEMPORAL RIFT FORMING
System detecting temporal instability!
Emergency shutdown recommended!"
```