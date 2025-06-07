# Side Quest: Radiation Shield

## Overview

**Quest Title:** Radiation Shield  
**Chapter:** Act I, Chapter 2: First Interactions  
**Difficulty:** Beginner  
**Concepts Taught:** Comparison operators, threshold monitoring, safety systems  
**Reward:** "Threshold Safety System" code example  

## Quest Description

After activating the Wayfinder Monoliths and establishing a basic safe zone, Alex and CORA discover that their map shows several radiation hotspots in the surrounding area. More concerning, these hotspots appear to shift periodically, possibly due to Chronovya's unusual temporal physics.

CORA's sensors can detect radiation levels, but she lacks the proper algorithms to trigger protective measures at the right thresholds. Her Earth-based radiation detection system needs to be recalibrated for Chronovyan radiation types, which have different safety thresholds and danger levels compared to Earth radiation.

## Gameplay Elements

### Environmental Puzzle
- Radiation visualization in the environment (glowing air particles with different colors)
- A damaged Chronovyan radiation shield generator with exposed control panel
- Four different types of radiation meters showing different scales and units
- Visual effects that intensify as radiation increases in certain areas

### CORA's Role
- CORA explains: "I'm detecting four distinct types of radiation: alpha, beta, gamma, and temporal radiation. Each requires different shielding thresholds."
- CORA provides Earth-based radiation knowledge: "On Earth, 5 mSv per year is the safety limit for the public, but I'm detecting much higher levels here."
- CORA can scan areas to display current radiation readings of each type

### Coding Challenge
Players must write code to:
1. Read radiation values from CORA's sensors
2. Compare each radiation type against appropriate thresholds
3. Activate different shield types based on those comparisons
4. Create a comprehensive safety system that adapts to changing radiation levels

## Code Solution

```chronovyan
// Initialize shield system and read current radiation levels
shield_system_online = true
print_to_cora("Initializing radiation shield system...")

// Get current radiation readings (in Chronovyan units)
alpha_radiation = Sensors.get_radiation_level("alpha")
beta_radiation = Sensors.get_radiation_level("beta")
gamma_radiation = Sensors.get_radiation_level("gamma")
temporal_radiation = Sensors.get_radiation_level("temporal")

// Display current readings
print_to_cora("Current radiation levels:")
print_to_cora("Alpha radiation: " + alpha_radiation + " cSv")
print_to_cora("Beta radiation: " + beta_radiation + " cSv")
print_to_cora("Gamma radiation: " + gamma_radiation + " cSv")
print_to_cora("Temporal radiation: " + temporal_radiation + " tSv")

// Define safety thresholds (from Chronovyan shield documentation)
alpha_safe_threshold = 10.0     // cSv (Chronovyan Sievert)
alpha_danger_threshold = 25.0   // cSv
beta_safe_threshold = 15.0      // cSv
beta_danger_threshold = 35.0    // cSv
gamma_safe_threshold = 5.0      // cSv
gamma_danger_threshold = 12.0   // cSv
temporal_safe_threshold = 8.0   // tSv (Temporal Sievert)
temporal_danger_threshold = 20.0 // tSv

// Function to set shield strength based on radiation level
function set_shield_strength(radiation_type, current_level, safe_level, danger_level) {
    // Calculate what percentage of the shield strength to activate
    if (current_level <= safe_level) {
        // Below safe threshold - minimal shielding needed
        shield_strength = 20
        status = "MINIMAL"
    } else if (current_level > safe_level && current_level < danger_level) {
        // Between safe and danger - proportional shielding
        range = danger_level - safe_level
        position = current_level - safe_level
        percentage = position / range
        shield_strength = 20 + (percentage * 60)
        status = "ELEVATED"
    } else if (current_level >= danger_level) {
        // At or above danger level - maximum shielding
        shield_strength = 100
        status = "MAXIMUM"
    }
    
    // Activate the shield at calculated strength
    Shield.activate(radiation_type, shield_strength)
    
    // Return status for reporting
    return status
}

// Set shield strengths for each radiation type
alpha_status = set_shield_strength("alpha", alpha_radiation, alpha_safe_threshold, alpha_danger_threshold)
beta_status = set_shield_strength("beta", beta_radiation, beta_safe_threshold, beta_danger_threshold)
gamma_status = set_shield_strength("gamma", gamma_radiation, gamma_safe_threshold, gamma_danger_threshold)
temporal_status = set_shield_strength("temporal", temporal_radiation, temporal_safe_threshold, temporal_danger_threshold)

// Report shield status
print_to_cora("Shield status:")
print_to_cora("Alpha shield: " + alpha_status)
print_to_cora("Beta shield: " + beta_status)
print_to_cora("Gamma shield: " + gamma_status)
print_to_cora("Temporal shield: " + temporal_status)

// Check if any shield is at maximum (danger level)
if (alpha_status == "MAXIMUM" || beta_status == "MAXIMUM" || 
    gamma_status == "MAXIMUM" || temporal_status == "MAXIMUM") {
    print_to_cora("WARNING: Dangerous radiation levels detected!")
    print_to_cora("Recommend immediate relocation to safer area.")
} else if (alpha_status == "ELEVATED" || beta_status == "ELEVATED" || 
           gamma_status == "ELEVATED" || temporal_status == "ELEVATED") {
    print_to_cora("CAUTION: Elevated radiation levels detected.")
    print_to_cora("Shields operating at increased capacity.")
} else {
    print_to_cora("All radiation levels within acceptable parameters.")
    print_to_cora("Shields operating at minimal capacity to conserve power.")
}
```

## Learning Outcomes

Through this quest, players learn:
- How to use comparison operators (==, !=, <, >, <=, >=)
- How to implement threshold-based logic for safety systems
- How to create graduated responses based on value ranges
- How to combine multiple conditions using logical operators
- How to implement proportional calculations based on value positions within ranges

## Success Criteria
- The radiation shield generator activates with a visible energy field
- The shield's color and intensity adjust based on radiation levels in the environment
- CORA confirms that her systems are properly protected from all radiation types
- The shield control panel displays balanced energy consumption across all shield types
- Areas previously inaccessible due to radiation can now be explored safely

## Reward Delivery

Upon successful calibration, the shield generator fully activates and stabilizes:
- The generator's database unlocks, revealing a digital copy of a Chronovyan safety protocol
- CORA analyzes the protocol: "This appears to be a threshold monitoring system used by Chronovyans to protect their technology from environmental hazards."
- The player receives the **"Threshold Safety System"** code example, which becomes accessible in CORA's database

Additionally, Alex and CORA gain:
- The ability to safely traverse radiation zones with the shield active
- Access to a small cache of power crystals hidden behind the shield
- A data fragment containing a partial map showing a path through a nearby radiation zone

## Connection to Main Quest

This side quest supports the "Stabilizing CORA's Power Core" main quest by:
1. Teaching comparison operator skills needed for power regulation
2. Providing a model for creating graduated responses to different conditions
3. Ensuring CORA's systems are protected from radiation that could further destabilize her power core
4. Building on the environmental data monitoring skills from Chapter 1

## CORA Dialogue Samples

**Initial Problem:**
> "Alex, my sensors are detecting multiple types of radiation in this area. Unfortunately, my shield system needs recalibration for Chronovyan radiation patterns. Without proper thresholds, I can't efficiently allocate shield power where it's needed most."

**When Accessing Shield Generator:**
> "This appears to be a Chronovyan radiation shield generator. Its configuration panel is active, but the threshold algorithms are corrupted. We'll need to program new algorithms based on the radiation types I'm detecting."

**During Radiation Spike:**
> "Warning! Temporal radiation spike detected at 27.3 tSv - well above the danger threshold! We need to activate maximum shielding for that radiation type immediately."

**After Successful Calibration:**
> "Shield calibration successful! My systems are now properly protected against all detected radiation types. I'm also detecting a data cache that was previously inaccessible due to the radiation interference."

**Upon Finding Chronovyan Protocol:**
> "I've analyzed the Chronovyan safety protocol. It's quite sophisticated - it uses comparative thresholds with proportional responses, much like what we just implemented. This will be extremely useful for other environmental hazards we might encounter."

**Suggesting Next Steps:**
> "With the radiation shield operational, we can now safely explore some previously inaccessible areas. This might help us locate the components needed to stabilize my power core. According to the map fragment we found, there's a promising location just north of here." 