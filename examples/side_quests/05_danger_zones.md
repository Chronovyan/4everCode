# Side Quest: Danger Zones

## Overview

**Quest Title:** Danger Zones  
**Chapter:** Act I, Chapter 2: First Interactions  
**Difficulty:** Beginner  
**Concepts Taught:** Nested conditionals, complex decision trees, prioritization logic  
**Reward:** "Advanced Environmental Response" code example  

## Quest Description

With the radiation shield operational and CORA's power regulation functioning better, Alex discovers an ancient Chronovyan security station at the edge of their safe zone. The security console appears to control access to several surrounding areas, each marked as potential "danger zones" on their map.

CORA's analysis reveals that the security system once categorized areas based on multiple environmental factors, determining whether explorers could safely enter each zone. However, the classification algorithm is corrupted, leaving the system unable to properly assess the dangers.

## Gameplay Elements

### Environmental Puzzle
- A circular console with holographic projections of nearby areas
- Four distinct zones visible on the projection, each with different environmental characteristics
- Multiple sensor readings flickering for each zone (temperature, radiation, air quality, temporal stability)
- Color-coded warning indicators (green, yellow, orange, red) currently malfunctioning

### CORA's Role
- CORA explains: "This appears to be a security classification system. It should categorize each zone by danger level to prevent accidental entry into hazardous areas."
- CORA provides detailed sensor readings: "I'm receiving multiple environmental factors for each zone. We need to analyze these together to properly assess the danger."
- CORA highlights a Chronovyan classification system: "Their safety protocols appear more complex than Earth systems, with multiple conditions and subcategories."

### Coding Challenge
Players must write code to:
1. Assess multiple environmental factors for each zone
2. Implement nested conditional logic to classify zones by danger level
3. Prioritize certain dangers over others (e.g., extreme radiation supersedes mild temperature concerns)
4. Generate appropriate warnings and recommendations for each zone

## Code Solution

```chronovyan
// Initialize zone classification system
print_to_cora("Initializing Chronovyan security classification system...")
print_to_cora("Analyzing environmental data for nearby zones...")

// Function to classify danger level based on multiple factors
function classify_zone(zone_name, temperature, radiation_level, air_quality, temporal_stability) {
    print_to_cora("-------------------------------------")
    print_to_cora("ANALYZING ZONE: " + zone_name)
    print_to_cora("Temperature: " + temperature + "°C")
    print_to_cora("Radiation: " + radiation_level + " cSv")
    print_to_cora("Air Quality: " + air_quality + "%")
    print_to_cora("Temporal Stability: " + temporal_stability + "%")
    
    // Start with default classification
    danger_level = "UNKNOWN"
    primary_hazard = "None detected"
    
    // First check for immediately life-threatening conditions
    if (radiation_level > 50) {
        danger_level = "EXTREME DANGER"
        primary_hazard = "Lethal radiation levels"
    } else if (air_quality < 10) {
        danger_level = "EXTREME DANGER"
        primary_hazard = "Unsurvivable atmosphere"
    } else if (temporal_stability < 15) {
        danger_level = "EXTREME DANGER"
        primary_hazard = "Critical temporal instability"
    } else if (temperature > 60 || temperature < -40) {
        danger_level = "EXTREME DANGER"
        primary_hazard = "Lethal temperature"
    } 
    // Then check for high danger conditions
    else if (radiation_level > 25) {
        danger_level = "HIGH DANGER"
        primary_hazard = "High radiation exposure"
        
        // Nested condition: Even worse if combined with temperature extremes
        if (temperature > 45 || temperature < -20) {
            danger_level = "VERY HIGH DANGER"
            primary_hazard = "Combined radiation and temperature hazards"
        }
    } else if (air_quality < 30) {
        danger_level = "HIGH DANGER"
        primary_hazard = "Severely compromised atmosphere"
        
        // Nested condition: Check for specific hazard combination
        if (temporal_stability < 40) {
            danger_level = "VERY HIGH DANGER"
            primary_hazard = "Atmosphere degradation due to temporal instability"
        }
    } else if (temporal_stability < 30) {
        danger_level = "HIGH DANGER"
        primary_hazard = "Significant temporal anomalies"
    } else if (temperature > 45 || temperature < -20) {
        danger_level = "HIGH DANGER"
        primary_hazard = "Temperature extremes"
    }
    // Then check for moderate danger conditions
    else if (radiation_level > 10) {
        danger_level = "MODERATE DANGER"
        primary_hazard = "Elevated radiation"
        
        // Check air quality as a compounding factor
        if (air_quality < 60) {
            danger_level = "HIGH DANGER"
            primary_hazard = "Radiation particles in compromised atmosphere"
        }
    } else if (air_quality < 60) {
        danger_level = "MODERATE DANGER"
        primary_hazard = "Poor air quality"
    } else if (temporal_stability < 60) {
        danger_level = "MODERATE DANGER"
        primary_hazard = "Occasional temporal shifts"
    } else if (temperature > 35 || temperature < -10) {
        danger_level = "MODERATE DANGER"
        primary_hazard = "Uncomfortable temperature"
    }
    // Finally check for low danger conditions
    else if (radiation_level > 5 || air_quality < 80 || 
             temporal_stability < 80 || temperature > 30 || temperature < 0) {
        danger_level = "LOW DANGER"
        primary_hazard = "Minor environmental hazards"
    } else {
        // If we reach here, all conditions are within safe parameters
        danger_level = "SAFE"
        primary_hazard = "None detected"
    }
    
    // Generate appropriate recommendations based on classification
    if (danger_level == "EXTREME DANGER") {
        recommendation = "ABSOLUTELY NO ENTRY! Fatal conditions present."
        color_code = "RED"
    } else if (danger_level == "VERY HIGH DANGER" || danger_level == "HIGH DANGER") {
        recommendation = "Specialized protective equipment required. Brief exposure only."
        color_code = "ORANGE"
    } else if (danger_level == "MODERATE DANGER") {
        recommendation = "Caution advised. Basic protective gear recommended."
        color_code = "YELLOW"
    } else if (danger_level == "LOW DANGER") {
        recommendation = "Generally safe for exploration with monitoring."
        color_code = "GREEN"
    } else if (danger_level == "SAFE") {
        recommendation = "Safe for extended exploration. No special equipment needed."
        color_code = "BLUE"
    } else {
        recommendation = "Unable to classify. Proceed with extreme caution."
        color_code = "GRAY"
    }
    
    // Output the classification results
    print_to_cora("CLASSIFICATION: " + danger_level)
    print_to_cora("PRIMARY HAZARD: " + primary_hazard)
    print_to_cora("RECOMMENDATION: " + recommendation)
    print_to_cora("COLOR CODE: " + color_code)
    
    // Return the classification details as an object
    return {
        "level": danger_level,
        "hazard": primary_hazard,
        "recommendation": recommendation,
        "color": color_code
    }
}

// Analyze each zone
zone_alpha = classify_zone("Alpha Ridge", 28, 8, 75, 92)
zone_beta = classify_zone("Beta Canyon", 48, 22, 45, 65)
zone_gamma = classify_zone("Gamma Flats", -15, 12, 35, 28)
zone_delta = classify_zone("Delta Crater", 32, 65, 85, 42)

// Find the safest zone for exploration
print_to_cora("-------------------------------------")
print_to_cora("IDENTIFYING SAFEST EXPLORATION ROUTE:")

if (zone_alpha.level == "SAFE" || zone_alpha.level == "LOW DANGER") {
    print_to_cora("Primary recommendation: Explore ALPHA RIDGE")
    print_to_cora("Reasoning: " + zone_alpha.level + " with " + zone_alpha.hazard)
} else if (zone_beta.level == "SAFE" || zone_beta.level == "LOW DANGER") {
    print_to_cora("Primary recommendation: Explore BETA CANYON")
    print_to_cora("Reasoning: " + zone_beta.level + " with " + zone_beta.hazard)
} else if (zone_gamma.level == "SAFE" || zone_gamma.level == "LOW DANGER") {
    print_to_cora("Primary recommendation: Explore GAMMA FLATS")
    print_to_cora("Reasoning: " + zone_gamma.level + " with " + zone_gamma.hazard)
} else if (zone_delta.level == "SAFE" || zone_delta.level == "LOW DANGER") {
    print_to_cora("Primary recommendation: Explore DELTA CRATER")
    print_to_cora("Reasoning: " + zone_delta.level + " with " + zone_delta.hazard)
} else {
    // If no safe or low danger zones, find the least dangerous one
    print_to_cora("CAUTION: No safe exploration routes available.")
    
    // Complex nested conditional to find least dangerous option
    if (zone_alpha.level != "EXTREME DANGER" && 
        (zone_beta.level == "EXTREME DANGER" || zone_alpha.level < zone_beta.level) &&
        (zone_gamma.level == "EXTREME DANGER" || zone_alpha.level < zone_gamma.level) &&
        (zone_delta.level == "EXTREME DANGER" || zone_alpha.level < zone_delta.level)) {
        print_to_cora("If necessary, ALPHA RIDGE presents lowest risk")
    } else if (zone_beta.level != "EXTREME DANGER" && 
               (zone_gamma.level == "EXTREME DANGER" || zone_beta.level < zone_gamma.level) &&
               (zone_delta.level == "EXTREME DANGER" || zone_beta.level < zone_delta.level)) {
        print_to_cora("If necessary, BETA CANYON presents lowest risk")
    } else if (zone_gamma.level != "EXTREME DANGER" && 
               (zone_delta.level == "EXTREME DANGER" || zone_gamma.level < zone_delta.level)) {
        print_to_cora("If necessary, GAMMA FLATS presents lowest risk")
    } else if (zone_delta.level != "EXTREME DANGER") {
        print_to_cora("If necessary, DELTA CRATER presents lowest risk")
    } else {
        print_to_cora("ALL ZONES EXTREMELY DANGEROUS. DO NOT ATTEMPT EXPLORATION.")
    }
}
```

## Learning Outcomes

Through this quest, players learn:
- How to use nested conditional statements for complex decision-making
- How to prioritize conditions in order of importance
- How to build decision trees with multiple branches and outcomes
- How to combine multiple factors in decision-making processes
- How to create reusable classification functions that handle complex logic

## Success Criteria
- The security console illuminates with proper color-coded zone classifications
- Each zone projection shows detailed hazard information and recommendations
- CORA confirms that the classification algorithm aligns with Chronovyan safety standards
- The security system unlocks safe pathways and restricts access to dangerous zones
- A recommended exploration route is highlighted on the holographic map

## Reward Delivery

Upon successful implementation of the classification system:
- The security console fully activates, displaying a detailed environmental analysis of the surrounding areas
- A hidden database compartment opens, revealing a comprehensive Chronovyan safety protocol
- CORA analyzes the protocol: "This is an advanced environmental response system that the Chronovyans used to navigate hazardous regions. It contains complex decision trees for adaptive survival strategies."
- The player receives the **"Advanced Environmental Response"** code example in CORA's database

Additionally, Alex and CORA gain:
- A detailed map of the surrounding regions with safety classifications
- Access codes to secure pathways that bypass the most dangerous zones
- Insights into how Chronovyans adapted to environmental hazards

## Connection to Main Quest

This side quest supports the "Stabilizing CORA's Power Core" main quest by:
1. Teaching nested conditional logic needed for complex power regulation
2. Providing a framework for prioritizing system responses to multiple factors
3. Establishing safe travel routes to locations where power core components might be found
4. Building on the environmental monitoring and threshold skills from previous quests

## CORA Dialogue Samples

**Initial Problem:**
> "Alex, I'm detecting a security classification system at this terminal. It should be categorizing these areas by danger level, but the algorithm appears corrupted. Without proper classification, we risk entering zones with fatal environmental conditions."

**When Accessing Security Console:**
> "This Chronovyan security system is quite sophisticated. It analyzes multiple environmental factors simultaneously and uses nested decision trees to classify danger levels. We need to reconstruct their classification logic to get accurate safety assessments."

**During Analysis:**
> "Interesting... Chronovyans prioritized radiation and temporal stability above other factors. They also considered how different hazards might compound each other, rather than treating them in isolation. This suggests they understood complex environmental interactions."

**After Successful Implementation:**
> "Classification system restored! We now have accurate safety assessments for all surrounding areas. According to this data, the Alpha Ridge appears to be our safest option for exploration, while the Delta Crater should be absolutely avoided due to extreme radiation levels."

**Upon Finding Advanced Protocol:**
> "I've analyzed the Chronovyan environmental response protocol. It's far more sophisticated than Earth systems - it uses adaptive decision trees that account for both individual hazards and their interactions. This will be invaluable for navigating Chronovya's more dangerous regions."

**Suggesting Next Steps:**
> "With this classification system working, we now have a safe route to explore for power core components. The path through Alpha Ridge leads to a structure that might contain the stabilization technology we need. And with our radiation shield operational, we can briefly venture into moderate danger zones if necessary." 