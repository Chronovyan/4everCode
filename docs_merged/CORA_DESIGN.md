---
title: 'CORA: Companion for Operational Rifting and Analysis'
description: Documentation for CORA_DESIGN.md
weight: 100
draft: true
---

# CORA: Companion for Operational Rifting and Analysis

## Overview

CORA is a palm-sized utility drone from Professor Thorne's laboratory that accompanies Alex throughout the Chronovyan adventure. She serves as both a narrative companion and a functional game mechanic, providing guidance, feedback, and interface capabilities while requiring power management as a core gameplay loop.

## Narrative Role

### Origin Story
CORA was Professor Thorne's experimental utility drone, designed to assist with his Chronovyan research. During the chaotic portal incident in the lab, Alex instinctively grabs CORA before being pulled into the rift. Upon arrival in Chronovya, CORA is damaged and requires immediate repair, creating an initial objective for the player.

### Personality
- **Analytical but Personable**: CORA has a primarily logical demeanor with subtle hints of developing personality as Alex repairs and upgrades her
- **Adaptive Learning**: As the game progresses, CORA gradually incorporates Chronovyan phrases and concepts into her speech patterns
- **Protective**: Shows increasing concern for Alex's wellbeing, reflecting her growing adaptation to her role as a companion rather than just a tool

### Character Development
- **Initial State**: Damaged utility drone with basic functionality and formal communication style
- **Mid-Game Evolution**: More personalized interactions as CORA's systems integrate with Chronovyan technology
- **Late-Game Transformation**: Potential story arc where CORA must decide between returning to Earth with Alex or remaining in Chronovya where her systems have become more compatible

## Technical Specifications

### Physical Description
- **Size**: Approximately 15cm in diameter, 8cm in height
- **Appearance**: Circular chassis with sleek metallic exterior, hovering capability (when functional)
- **Display**: Projects holographic interface 30cm above her chassis
- **Indicators**: External power level indicator showing percentage via light bar
- **Components**: Visible crystal energy receptacle on side

### Core Systems
1. **Analysis Module**: Scans objects, environments, and Chronovyan script
2. **Translation Matrix**: Interprets Chronovyan language (partially functional at start)
3. **Holographic Projector**: Displays terminal interface and visual information
4. **Mobility System**: Hovering capability (damaged initially, restored through upgrades)
5. **Energy Management**: Power systems that require crystal resources
6. **Voice Synthesis**: Communication module with adaptive learning capabilities

## Gameplay Functionality

### Interface Capabilities
- **Terminal Access**: Projects a holographic terminal where Alex can write and execute Chronovyan code
- **Environmental Scanner**: Analyzes objects and provides data that can be used in code
- **Translation System**: Helps decipher Chronovyan glyphs found in the environment
- **Knowledge Repository**: Stores information about discovered items, locations, and code concepts
- **Map Projection**: Eventually can display a simple map of explored areas

### Assistance Types
1. **Contextual Hints**: Provides subtle guidance when player appears stuck
   ```
   "Alex, my sensors detect that the door mechanism responds to numerical input. 
   Perhaps we should examine those glyphs more closely?"
   ```

2. **Code Feedback**: Offers specific feedback on syntax errors or logical problems
   ```
   "Error in line 3: Chronovyan syntax requires boolean values to be specified
   as 'true' or 'false' without quotation marks."
   ```

3. **Environmental Alerts**: Warns about hazards or opportunities in the area
   ```
   "Warning: Temporal flux increasing in this region. We should minimize our
   exposure unless properly shielded."
   ```

4. **Tutorial Guidance**: Explicitly explains new concepts when first encountered
   ```
   "I'm detecting a conditional gate mechanism. These Chronovyan structures
   respond to 'if-then-else' logic patterns. Shall I display the basic syntax?"
   ```

## Power System

### Power Management
- **Power Level**: 0-100% scale visible to player at all times
- **Consumption Rates**:
  - Passive drain: -1% per minute of game time
  - Scanning: -2% per standard scan
  - Complex analysis: -5% per deep scan
  - Interface projection: -1% per minute of active use
  - Special abilities: Variable cost (5-20% depending on function)

### Power Sources
1. **Crystals** (Portable power):
   - Blue Crystals: +10% power, common
   - Purple Crystals: +25% power, uncommon
   - Gold Crystals: +50% power, rare

2. **Power Nodes** (Fixed locations):
   - Chronovyan terminals: Recharge to 100% when activated
   - Ancient power conduits: Continuous charging while in proximity
   - Solar formations: Timed charging based on day/night cycle

### Functionality at Power Levels
- **75-100%**: Full functionality, all systems online
- **50-75%**: Reduced scan range, limited translation capabilities
- **25-50%**: Holographic projection diminished, hint system limited
- **5-25%**: Emergency mode, critical functions only
- **Below 5%**: Automatic shutdown until recharged to at least 10%

## Upgrade Path

### Act I: Basic Repairs
- **Initial State**: Damaged, minimal functionality
- **First Upgrade**: Basic movement restored, limited scanning
- **Second Upgrade**: Full scanning capabilities, improved translation
- **Act I Milestone**: Terminal interface fully functional

### Act II: Enhanced Capabilities
- **Third Upgrade**: Extended power efficiency
- **Fourth Upgrade**: Advanced translation matrix
- **Fifth Upgrade**: Environmental shielding capabilities
- **Act II Milestone**: Ability to interface directly with Chronovyan technology

### Act III: Advanced Integration
- **Sixth Upgrade**: Temporal anomaly detection
- **Seventh Upgrade**: Limited autonomous actions
- **Eighth Upgrade**: Chronovyan code optimization assistance
- **Act III Milestone**: Full integration with Chronovyan systems

## Implementation for MVP

### Hardcoded Response System
- Location triggers: Specific dialogue when entering areas
- Object interaction triggers: Responses when examining objects
- Code triggers: Feedback based on code patterns written
- State triggers: Responses to power level, upgrade status, etc.

### Response Categories
1. **Tutorial Guidance**: Explicitly instructional
2. **Subtle Hints**: Nudges without direct solutions
3. **Error Feedback**: Specific code correction guidance
4. **Narrative Commentary**: Character-building responses
5. **Environmental Information**: World-building content

### Development Evolution
- **MVP Phase**: Static responses from predetermined trigger library
- **Enhanced Phase**: Categorized responses with some contextual awareness
- **Advanced Phase**: Integration with AI text editor for dynamic assistance

## Future AI Integration Potential

The hardcoded MVP design intentionally creates a foundation for future AI assistant integration:

1. **Response Patterns**: The initial dialogue patterns establish expectations that can be maintained when transitioning to AI-generated responses
2. **Context Triggers**: The trigger system can evolve to provide context to an AI system
3. **Knowledge Base**: CORA's factual information about Chronovya can be used as a knowledge base for the AI
4. **Personality Parameters**: CORA's character traits can become guiding parameters for AI-generated dialogue

## CORA's Commands (Player Perspective)

Players can interact with CORA through commands embedded in Chronovyan code:

```
// Basic CORA commands
CORA.scan("crystal")            // Scan an object for information
CORA.analyze("ancient_text")    // Deeper analysis of complex items
CORA.display("map")             // Show stored information
CORA.power_status()             // Check detailed power information
CORA.shield(30)                 // Activate protective shield (costs 30% power)

// Advanced commands (unlocked through progression)
CORA.interface_with("terminal") // Connect to Chronovyan technology
CORA.translate_realtime()       // Continuous translation (power drain)
CORA.detect_anomalies()         // Scan for temporal instabilities
CORA.optimize(my_code)          // Suggest optimizations for code
```

## Sample Dialogue Progression

### Prologue (Lab)
```
[Initial state in lab]
CORA: "Good morning, Alex. Professor Thorne has left instructions for the 
calibration sequence. Shall I display them on my terminal?"
```

### Act I (Early)
```
[Just after arriving in Chronovya, damaged]
CORA: "S-systems damaged... p-power levels critical... require... immediate... 
repair... Alex... where... are we?"
```

### Act I (After repair)
```
CORA: "Systems partially restored. Thank you, Alex. Initial analysis indicates 
we are no longer on Earth. Chronovyan signatures detected in atmosphere. 
Professor Thorne's research appears to have been correct."
```

### Act II
```
CORA: "I've been analyzing these Chronovyan data patterns. They remind me of 
quantum algorithms from Professor Thorne's research, but with temporal 
variables I've never encountered before. Fascinating."
```

### Act III
```
CORA: "Alex, I've noticed something unusual in my systems. As we've interfaced 
with more Chronovyan technology, my processing patterns have begun to... evolve. 
I'm understanding concepts that weren't in my original programming."
```

## Game Design Integration

CORA's design carefully balances several gameplay elements:
- **Helper vs. Hindrance**: CORA provides valuable assistance but requires resource management
- **Guidance vs. Discovery**: CORA offers hints without removing the satisfaction of solving puzzles
- **Mechanical vs. Emotional**: CORA serves as both a gameplay tool and a character with narrative development
- **Necessity vs. Optionality**: Some of CORA's functions are required for progress while others are optional aids

This design ensures CORA enhances the gameplay experience without overshadowing the player's journey of learning Chronovyan programming.