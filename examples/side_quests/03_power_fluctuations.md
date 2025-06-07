# Side Quest: Power Fluctuations

## Overview

**Quest Title:** Power Fluctuations  
**Chapter:** Act I, Chapter 2: First Interactions  
**Difficulty:** Beginner  
**Concepts Taught:** Conditional statements, logical operators, decision-making  
**Reward:** "Core Regulation Protocol" code example  

## Quest Description

As Alex and CORA explore further from the crash site, CORA's power core begins to fluctuate wildly. Her interface flickers, and her hovering becomes unstable. Diagnostics indicate that her power regulation system is malfunctioning, unable to properly respond to the unique energy patterns of Chronovya.

CORA explains that her core was designed for Earth's energy environment, but Chronovya's temporal flux is causing unpredictable power surges and drops. Without proper regulation, she could experience a critical failure, losing both functionality and stored data.

## Gameplay Elements

### Environmental Puzzle
- CORA's external power indicator fluctuates visibly (cycling between red, yellow, and green)
- Strange energy patterns visible in the air (temporal disturbances)
- A small Chronovyan power regulation shrine with a terminal interface
- Visual representation of power levels through glowing conduits

### CORA's Role
- CORA provides initial problem: "My power regulation system is failing to adapt to Chronovyan energy patterns."
- CORA can analyze her own systems: "My diagnostics indicate power spikes of 142% followed by drops to 23% - outside safe operating parameters."
- CORA's functionality diminishes as her power becomes more erratic (visual effects and limited capabilities)

### Coding Challenge
Players must write code to:
1. Constantly monitor CORA's power levels
2. Identify different power states (critical low, low, normal, high, critical high)
3. Apply different regulation responses for each state
4. Implement a safety threshold to prevent damage

## Code Solution

```chronovyan
// Function to regulate CORA's power core
function regulate_power_core() {
    // Get current power level (0-200%, where 100% is nominal)
    power_level = CORA.get_power_level()
    print_to_cora("Current power level: " + power_level + "%")
    
    // Determine power state and apply appropriate regulation
    if (power_level < 20) {
        // Critical low power - Emergency power conservation
        print_to_cora("CRITICAL LOW POWER STATE")
        print_to_cora("Activating emergency power conservation")
        CORA.set_operating_mode("minimal")
        CORA.disable_non_essential_systems()
        CORA.activate_energy_absorption("high")
    } else if (power_level < 50) {
        // Low power - Increase absorption rate
        print_to_cora("LOW POWER STATE")
        print_to_cora("Increasing energy absorption rate")
        CORA.set_operating_mode("reduced")
        CORA.activate_energy_absorption("medium")
    } else if (power_level <= 150) {
        // Normal operating range - Balanced mode
        print_to_cora("NORMAL POWER STATE")
        print_to_cora("Maintaining balanced power regulation")
        CORA.set_operating_mode("standard")
        CORA.activate_energy_absorption("normal")
    } else if (power_level <= 180) {
        // High power - Reduce absorption, increase consumption
        print_to_cora("HIGH POWER STATE")
        print_to_cora("Reducing energy absorption, increasing consumption")
        CORA.set_operating_mode("enhanced")
        CORA.activate_energy_absorption("low")
    } else {
        // Critical high power - Emergency venting
        print_to_cora("CRITICAL HIGH POWER STATE")
        print_to_cora("Initiating emergency power venting")
        CORA.set_operating_mode("safety")
        CORA.activate_energy_absorption("none")
        CORA.vent_excess_energy()
    }
    
    // Return new stabilized power level
    return CORA.get_power_level()
}

// Call the regulation function and report results
initial_power = CORA.get_power_level()
new_power = regulate_power_core()

// Verify regulation was successful
if (new_power >= 50 && new_power <= 150) {
    print_to_cora("Power successfully regulated to safe levels: " + new_power + "%")
} else {
    print_to_cora("Power levels still outside optimal range: " + new_power + "%")
    print_to_cora("Additional regulation cycles required")
}
```

## Learning Outcomes

Through this quest, players learn:
- How to use if/else if/else conditional statements for multiple cases
- How to structure logical conditions with various comparison operators
- How to implement a decision tree for different scenarios
- How to call functions with different parameters based on conditions
- How to verify results using additional conditional checks

## Success Criteria
- CORA's power indicator stabilizes to a steady blue glow
- The power fluctuations visible in her hovering and interface stop
- CORA confirms regulation success: "Power core successfully regulated. Operating within normal parameters."
- The regulation shrine's conduits glow with steady, balanced energy

## Reward Delivery

Upon successful regulation, the shrine's terminal displays:
- A detailed power diagnostic showing stabilized readings
- A small cache of power crystals (+20% power for CORA)
- A data fragment containing Professor Thorne's notes on Chronovyan power systems

CORA analyzes the data fragment: "This appears to be Professor Thorne's core regulation protocol! He developed this algorithm to maintain stability in his equipment despite Chronovya's temporal energy fluctuations. This will significantly improve my operational capabilities."

The player receives the **"Core Regulation Protocol"** code example, which becomes accessible in CORA's database for future reference.

## Connection to Main Quest

This side quest supports the "Stabilizing CORA's Power Core" main quest by:
1. Teaching the player how to monitor and respond to different power states
2. Providing the regulation algorithm needed to maintain stable power levels
3. Ensuring CORA has sufficient power to support upcoming exploration and mapping activities
4. Building on the previous environmental data quests with practical application

## CORA Dialogue Samples

**Initial Problem:**
> "Alex, I'm experiencing dangerous power fluctuations. My core wasn't designed to handle Chronovya's temporal energy patterns. Without proper regulation, I could experience critical failure."

**When Accessing Terminal:**
> "This appears to be a Chronovyan power regulation shrine. The interface shows multiple energy states and corresponding regulation protocols. I believe we could adapt these to stabilize my power core."

**During Power Spike:**
> "Warning! Power level exceeding 180% of capacity. Emergency venting required to prevent core damage."

**During Power Drop:**
> "Alert: Power dropping below 30% threshold. Non-essential systems shutting down to conserve energy. Please implement regulation protocol soon."

**After Successful Regulation:**
> "Power regulation successful! My core is now adapting to Chronovyan energy patterns. This will significantly extend my operational capabilities and prevent unexpected shutdowns."

**Upon Finding Thorne's Notes:**
> "These are Professor Thorne's power regulation algorithms! He faced similar challenges with his equipment. His notes indicate that Chronovyan energy follows temporal patterns that can be predicted and managed with the right protocols. I've added this example to my database."

**Suggesting Next Steps:**
> "With my power core now stable, we can explore further without risk of my systems failing. We should continue searching for the Wayfinder Monoliths to map the area and establish a safe zone." 