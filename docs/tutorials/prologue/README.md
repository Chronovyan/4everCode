---
title: 'Prologue: The Lab Incident'
description: Documentation for tutorials\prologue\README.md
weight: 140
draft: true
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Prologue: The Lab Incident

## Overview

Welcome to the beginning of your journey into Chronovya and the [Chronovyan](https://chronovyan.github.io/h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/)h)t)t)p)s):)/)/)c)h)r)o)n)o)v)y)a)n).)g)i)t)h)u)b).)i)o)/) "The) temporal) programming) language) and) runtime") programming language. In this prologue, you'll step into the shoes of Alex, a curious intern who discovers Professor Thorne's hidden research lab. Through exploring the lab and interacting with Thorne's computer terminal, you'll learn the basics of Chronovyan programming while unraveling the mystery of the professor's disappearance.

## Learning Objectives

By completing the Prologue, you'll be able to:
- Understand and use basic Chronovyan syntax
- Declare and assign values to variables
- Use simple pre-defined commands
- Read and modify existing code
- Execute sequences of Chronovyan code

## Story Synopsi
Three weeks ago, the brilliant but eccentric Professor Thorne vanished without a trace. As his research intern, you've been searching for clues to his whereabouts. Following a cryptic email that arrived on a delayed send, you've discovered his hidden lab beneath the university physics building. Inside, you find strange equipment, mysterious notes, and a computer terminal running an unfamiliar programming language: Chronovyan.*## Tutorial Walkthrough

###

``text
    To wake terminal:
    Run hello_chronovyan()
```text**Solution:**```text
hello_chronovyan()
```text**Game Response:**The terminal screen lights up with a welcome message:
```text
"Welcome to Chronovyan Terminal v0.9.4 (Thorne Edition)
Timeline Stability: NOMINAL
Last user: Prof. J. Thorne
Last login: 21 days ago"
```text**Tutorial Notes:**This introduces the concept of function calls without parameters. The player learns that commands in Chronovyan are followed by parentheses to execute them.

###

   To access lab log, set user credential variables and run access_log()
    Required variables:
    - username (string)
    - access_level (number)
```text**Solution:**```text
DECLARE CONF username : STRING = "Alex";
DECLARE CONF access_level : INT = 1;
access_log();
```text**Game Response:**```text
"Access granted to user: Alex (Level 1)
WARNING: Level 1 access only permits viewing recent entries.
For full access, level 3 required.

LATEST LOG ENTRY:
Day 342: The calculations are finally converging. The Chronovyan equations
predict a stable connection point. If my theory is correct, this could
change everything we know about spacetime. I'm going to attempt a small-scale
test tonight. Just need to finish calibrating the temporal stabilizers..."
```text**Tutorial Notes:**This introduces variable declaration and assignment, as well as different data types (strings and numbers). The player learns that variables store information that can be used by functions.

###

``text
    "If you're reading this, you may need to adjust security variables.
    Remember: variables can be changed. Admin override: access_level = 3"
```text**Solution:**```text
access_level = 3;
access_log();
```text**Game Response:**```text
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
```text**Tutorial Notes:**The player learns that variables can be reassigned with new values, and that changing variables can affect program behavior.

###

``text
    "Cabinet Security System
    Current status: LOCKED
    To unlock, run the following sequence:
    1. Set security_override to true
    2. Set authorization_code to 7429
    3. Run unlock_cabinet()"
```text**Solution:**```text
DECLARE CONF security_override : BOOL = true;
DECLARE CONF authorization_code : INT = 7429;
unlock_cabinet();
```text**Game Response:**
*The cabinet clicks and slides open, revealing a strange device with glowing components. It appears to be some kind of prototype. A small tablet next to it shows technical specifications for what it calls a "Chronovyan Rift Stabilizer (Prototype)"
*Tutorial Notes:**This reinforces variable assignment with a new data type (boolean) and shows how a sequence of commands can be used to accomplish a goal.

###

``text
    "Chronovyan Rift Stabilizer - Diagnostic Interface
    To run diagnostics, set the following parameters:
    - power_level (range 1-10)
    - target_frequency (value from Thorne's notes: 428.7)
    - diagnostic_mode (set to true)
    Then run start_diagnostic()"
```text

Looking around, you find Thorne's notes indicating he was using power_level 8 in his tests.**Solution:**```text
DECLARE CONF power_level : INT = 8;
DECLARE CONF target_frequency : FLOAT = 428.7;
DECLARE CONF diagnostic_mode : BOOL = true;
start_diagnostic();
```text**Game Response:**```text
"DIAGNOSTIC RUNNING...
Power level: 8 (CAUTION: HIGH)
Target frequency: 428.7 MHz (MATCH: CHRONOVYA RESONANCE)
Diagnostic mode: ACTIVE

WARNING: Temporal field detected! Anomaly forming...
System detecting temporal instability!
Emergency shutdown recommended!

To abort: run abort_sequence()"
```text**Tutorial Notes:**This combines all concepts learned so far, showing how variables of different types can be used together to configure a complex system.

###

``text
    "CRITICAL WARNING: \1TEMPORAL\2/core/Core Concepts - The Foundation of Temporal Programming.md#temporal\3 RIFT FORMING
    Options:
    1. abort_sequence() - Emergency shutdown
    2. stabilize_field() - Attempt to maintain current configuration
    3. increase_power() - WARNING: Untested, may increase rift size

    Choose wisely!"
```text**Solutions (multiple paths):**

Path 1 (try to shut down):
```text
abort_sequence()
```text

Game Response:
```text
"ABORT SEQUENCE INITIATED...
ERROR: FIELD SELF-SUSTAINING
SHUTDOWN FAILURE
Portal expanding..."
```text

Path 2 (try to stabilize):
```text
stabilize_field()
```text

Game Response:
```text
"STABILIZATION SEQUENCE RUNNING...
Partial stabilization achieved.
Portal stable but active.
Sensors detecting habitable environment beyond threshold."
```text

Path 3 (increase power):
```text
power_level = 10;
increase_power();
```text

Game Response:
```text
"POWER LEVEL INCREASED...
WARNING: \1TEMPORAL\2/core/Core Concepts - The Foundation of Temporal Programming.md#temporal\3 RIFT FORMING
System detecting temporal instability!
Emergency shutdown recommended!"
```