---
title: Signal Boosters
description: Documentation for signal boosters
date_created: '2025-06-07'
status: draft
last_updated: '2025-06-07'
---

# Side Quest: Signal Boosters

## Overview

**Quest Title:** Signal Boosters  
**Chapter:** Act I, Chapter 3: Sustainable Survival  
**Difficulty:** Intermediate  
**Concepts Taught:** Function composition, return values, problem decomposition  
**Prerequisites:** Basic Chronovyan syntax, variables, functions, and arrays (from "Data Patterns" side quest)

## Quest Description

Following their success in analyzing the crystal field patterns, Alex and CORA continue their journey toward the highlands. CORA's sensors detect a faint, intermittent signal that matches the energy signature Professor Thorne described for the Resource Harvester, but it's too weak and distorted to pinpoint its exact location.

Among Professor Thorne's belongings found near the crystal field, Alex discovers schematics for Chronovyan signal amplifier modules. These devices were designed to boost and clarify faint energy signatures by processing them through a series of specialized filters and amplifiers. The schematics indicate that three interconnected signal boosters, when properly programmed and positioned, could triangulate the Resource Harvester's location with remarkable precision.

However, the signal processing requires a sophisticated approach to Chronovyan programming that Professor Thorne called "function composition" - creating specialized functions for each processing step and then chaining them together to form a complete signal processing pipeline.

## Gameplay Elements

### Environmental Features
- A series of small hills surrounding a valley where the faint Harvester signal originates
- Three damaged signal booster pedestals positioned at strategic points
- Various debris containing components needed to repair the boosters
- A makeshift workbench left by Professor Thorne with partial code samples

### CORA's Role
- CORA can detect the weak Harvester signal but cannot process it effectively
- She explains the principles of function composition in Chronovyan programming
- She translates the technical schematics for the signal boosters
- She helps identify which components are needed for repairs

### Alex's Tasks
1. Gather components to repair the three signal booster pedestals
2. Program each booster with specialized signal processing functions
3. Create a signal processing pipeline by composing functions together
4. Position and calibrate the boosters to triangulate the Harvester's location
5. Test the complete system to get precise coordinates for the Resource Harvester

## Initial Discovery

As Alex and CORA crest a ridge with a panoramic view of the highlands, CORA's sensors begin picking up an unusual energy signature.

**CORA:** "Alex, I'm detecting a faint energy pattern that matches Professor Thorne's description of the Resource Harvester. It's definitely coming from somewhere in that valley ahead, but the signal is too weak and distorted for me to determine its exact location."

While scanning the area, Alex notices several unusual structures on the surrounding hillsides.

**CORA:** "Those appear to be Chronovyan signal amplifiers! Professor Thorne must have been trying to locate the Harvester too. Let's check if his equipment is still functional."

Upon investigating the nearest signal booster pedestal, Alex finds it damaged but repairable. A data tablet nearby contains Professor Thorne's notes:

> *"Signal from Harvester too weak for direct detection. Using three-point triangulation with signal boosters. Each booster requires specific signal processing functions - noise filtering, frequency isolation, amplitude enhancement. Chronovyan approach of function composition ideal for this task. First booster operational, second partially programmed, third damaged in temporal storm. Must complete the signal processing chain to pinpoint Harvester coordinates."*

**CORA:** "Alex, these signal boosters are ingenious! Each one handles a different aspect of signal processing, and then combines the results. This approach breaks down the complex problem of signal analysis into smaller, manageable functions that work together. We need to repair these boosters and complete Professor Thorne's programming."

## Coding Challenge

### Phase 1: Basic Functions with Return Values

**CORA:** "Let's start by understanding how functions can return values in Chronovyan. We'll need this concept to create our signal processing pipeline."

```chronovyan
// Function that returns a value
function filter_noise(raw_signal) {
    // Filter out noise from the signal
    filtered_signal = raw_signal - (raw_signal * 0.2)
    
    // Return the filtered signal
    return filtered_signal
}

// Using the returned value
raw_data = 100
cleaned_data = filter_noise(raw_data)
print_to_cora("Original signal: " + raw_data)
print_to_cora("Filtered signal: " + cleaned_data)
```

**Task for Alex:** Create basic signal processing functions for the first booster, each returning a processed version of its input.

### Phase 2: Using Function Return Values as Inputs

**CORA:** "Now that we have our basic processing functions, we need to connect them together. The output from one function becomes the input to the next."

```chronovyan
// First signal processing step
function remove_background_noise(signal) {
    // Algorithm to filter background noise
    result = signal * 0.85
    print_to_cora("Removing background noise...")
    return result
}

// Second signal processing step
function enhance_frequency_band(signal) {
    // Algorithm to enhance specific frequencies
    result = signal * 1.5
    print_to_cora("Enhancing target frequency band...")
    return result
}

// Process the signal through multiple steps
original_signal = 100
filtered_signal = remove_background_noise(original_signal)
enhanced_signal = enhance_frequency_band(filtered_signal)

print_to_cora("Original signal strength: " + original_signal)
print_to_cora("After noise removal: " + filtered_signal)
print_to_cora("After frequency enhancement: " + enhanced_signal)
```

**Task for Alex:** Program the second signal booster to process data through multiple sequential functions.

### Phase 3: Function Composition (Chaining)

**CORA:** "For the third booster, we need to create a more efficient signal processing pipeline. Instead of storing intermediate results, we can directly chain function calls together."

```chronovyan
// First processing function
function filter_signal(raw_signal) {
    return raw_signal * 0.85
}

// Second processing function
function amplify_signal(filtered_signal) {
    return filtered_signal * 2.5
}

// Third processing function
function focus_signal(amplified_signal) {
    return amplified_signal + 10
}

// Using function composition (chaining function calls)
original_signal = 100
processed_signal = focus_signal(amplify_signal(filter_signal(original_signal)))

print_to_cora("Original signal: " + original_signal)
print_to_cora("Fully processed signal: " + processed_signal)
```

**Task for Alex:** Create a complete signal processing pipeline for the third booster using function composition.

### Phase 4: Functions with Multiple Parameters

**CORA:** "Now we need to calibrate our signal boosters to focus on specific coordinates. Let's create functions that accept multiple parameters."

```chronovyan
// Function with multiple parameters
function calibrate_booster(signal_strength, direction, frequency) {
    // Calculate calibration based on all parameters
    calibration_value = (signal_strength * 0.5) + (direction * 0.3) + (frequency * 0.2)
    
    print_to_cora("Calibrating booster...")
    print_to_cora("Signal strength factor: " + (signal_strength * 0.5))
    print_to_cora("Direction factor: " + (direction * 0.3))
    print_to_cora("Frequency factor: " + (frequency * 0.2))
    
    return calibration_value
}

// Call function with multiple arguments
booster_setting = calibrate_booster(75, 120, 42)
print_to_cora("Final calibration setting: " + booster_setting)
```

**Task for Alex:** Calibrate each signal booster by creating and using functions with multiple parameters.

### Phase 5: Higher-Order Functions

**CORA:** "These signal boosters are more sophisticated than I initially thought. They use what Professor Thorne called 'higher-order functions' - functions that return other functions. This allows for customized signal processors."

```chronovyan
// Higher-order function (returns another function)
function create_signal_processor(sensitivity) {
    // Return a customized function based on the sensitivity parameter
    return function(signal) {
        if (sensitivity == "high") {
            return signal * 3
        } else if (sensitivity == "medium") {
            return signal * 2
        } else {
            return signal * 1.5
        }
    }
}

// Create specialized signal processors
high_sensitivity_processor = create_signal_processor("high")
medium_sensitivity_processor = create_signal_processor("medium")

// Use the returned functions
test_signal = 50
high_result = high_sensitivity_processor(test_signal)
medium_result = medium_sensitivity_processor(test_signal)

print_to_cora("Original signal: " + test_signal)
print_to_cora("High sensitivity result: " + high_result)
print_to_cora("Medium sensitivity result: " + medium_result)
```

**Task for Alex:** Create a customizable signal processor for each booster using higher-order functions.

### Final Challenge: Complete Signal Processing System

**CORA:** "Now we need to combine everything into a complete signal processing system that uses all three boosters to triangulate the Harvester's location."

```chronovyan
// Create the complete triangulation system using function composition
function triangulate_harvester_location(signal_data) {
    // Process data from all three boosters
    booster1_result = process_booster1_data(signal_data.booster1)
    booster2_result = process_booster2_data(signal_data.booster2)
    booster3_result = process_booster3_data(signal_data.booster3)
    
    // Combine results to determine coordinates
    coordinates = calculate_coordinates(booster1_result, booster2_result, booster3_result)
    
    return coordinates
}

// Function to log the coordinates in a readable format
function log_coordinates(location_data) {
    print_to_cora("Harvester located at:")
    print_to_cora("X: " + location_data.x)
    print_to_cora("Y: " + location_data.y)
    print_to_cora("Z: " + location_data.z)
    print_to_cora("Confidence: " + location_data.confidence + "%")
}

// Execute the full process in one composed operation
log_coordinates(triangulate_harvester_location(collect_booster_data()))
```

**Task for Alex:** Implement the complete triangulation system that processes signals from all three boosters and returns the Harvester's precise coordinates.

## Complete Solution

```chronovyan
// Signal Boosters - Harvester Triangulation System
// This program processes signals from three boosters to locate the Resource Harvester

// Step 1: Define basic signal processing functions for each booster

// Booster 1: Noise Filtering Functions
function remove_background_noise(signal) {
    filtered = signal * 0.85
    print_to_cora("Filtering background noise...")
    return filtered
}

function remove_temporal_distortion(signal) {
    stabilized = signal + 15
    print_to_cora("Removing temporal distortion...")
    return stabilized
}

function isolate_harvester_frequency(signal) {
    // The Harvester operates at a specific frequency range
    isolated = signal * 1.2
    print_to_cora("Isolating Harvester frequency...")
    return isolated
}

// Booster 2: Signal Amplification Functions
function amplify_weak_signal(signal) {
    amplified = signal * 3
    print_to_cora("Amplifying signal...")
    return amplified
}

function enhance_clarity(signal) {
    enhanced = signal + 25
    print_to_cora("Enhancing signal clarity...")
    return enhanced
}

function focus_beam(signal, direction) {
    // Direction is in degrees (0-359)
    focused = signal * (1 + (direction / 1000))
    print_to_cora("Focusing signal beam at " + direction + " degrees...")
    return focused
}

// Booster 3: Signal Analysis Functions
function analyze_signal_strength(signal) {
    strength_factor = signal / 100
    print_to_cora("Analyzing signal strength...")
    return strength_factor
}

function calculate_distance(signal_strength) {
    // Convert signal strength to approximate distance
    distance = 1000 / signal_strength
    print_to_cora("Calculating approximate distance: " + distance + " meters")
    return distance
}

function determine_confidence(signal, noise_ratio) {
    confidence = (signal / (signal + noise_ratio)) * 100
    print_to_cora("Determining confidence level: " + confidence + "%")
    return confidence
}

// Step 2: Create higher-order functions for customizable processing

function create_custom_processor(sensitivity, focus_direction) {
    // Return a specialized processing function based on parameters
    return function(raw_signal) {
        // First remove noise
        filtered_signal = remove_background_noise(raw_signal)
        
        // Then remove temporal distortion
        stabilized_signal = remove_temporal_distortion(filtered_signal)
        
        // Isolate the frequency we care about
        isolated_signal = isolate_harvester_frequency(stabilized_signal)
        
        // Apply sensitivity-based amplification
        if (sensitivity == "high") {
            amplified_signal = isolated_signal * 3
        } else if (sensitivity == "medium") {
            amplified_signal = isolated_signal * 2
        } else {
            amplified_signal = isolated_signal * 1.5
        }
        
        // Focus the beam in the specified direction
        focused_signal = focus_beam(amplified_signal, focus_direction)
        
        return focused_signal
    }
}

// Step 3: Define functions to process data from each booster

function process_booster1_data(signal_data) {
    // Create a custom processor for booster 1
    processor = create_custom_processor("high", signal_data.direction)
    
    // Process the raw signal
    processed_signal = processor(signal_data.strength)
    
    // Calculate distance based on processed signal
    distance = calculate_distance(processed_signal)
    
    return {
        "direction": signal_data.direction,
        "distance": distance,
        "signal_quality": processed_signal
    }
}

function process_booster2_data(signal_data) {
    // Similar processing for booster 2, with medium sensitivity
    processor = create_custom_processor("medium", signal_data.direction)
    processed_signal = processor(signal_data.strength)
    distance = calculate_distance(processed_signal)
    
    return {
        "direction": signal_data.direction,
        "distance": distance,
        "signal_quality": processed_signal
    }
}

function process_booster3_data(signal_data) {
    // Similar processing for booster 3, with low sensitivity but high precision
    processor = create_custom_processor("low", signal_data.direction)
    processed_signal = processor(signal_data.strength)
    distance = calculate_distance(processed_signal * 1.1) // Adjust for precision
    
    return {
        "direction": signal_data.direction,
        "distance": distance,
        "signal_quality": processed_signal
    }
}

// Step 4: Define the triangulation function

function calculate_coordinates(booster1_result, booster2_result, booster3_result) {
    // Triangulation algorithm using the results from all three boosters
    // This is a simplified version for demonstration
    
    // Average the distances for a rough estimate
    avg_distance = (booster1_result.distance + 
                   booster2_result.distance + 
                   booster3_result.distance) / 3
    
    // Calculate a confidence level based on signal quality
    avg_quality = (booster1_result.signal_quality + 
                  booster2_result.signal_quality + 
                  booster3_result.signal_quality) / 3
    
    confidence = determine_confidence(avg_quality, 20)
    
    // For demonstration, generate coordinates based on the data
    // In a real system, this would use trigonometry with the angles and distances
    x_coord = 500 + (Math.cos(booster1_result.direction) * avg_distance / 2)
    y_coord = 500 + (Math.sin(booster2_result.direction) * avg_distance / 2)
    z_coord = 100 + (avg_quality / 10)
    
    return {
        "x": Math.round(x_coord),
        "y": Math.round(y_coord),
        "z": Math.round(z_coord),
        "confidence": Math.round(confidence)
    }
}

// Step 5: Define the main triangulation function that brings everything together

function triangulate_harvester_location() {
    // In a real scenario, this would collect actual signal data
    // For this example, we'll use test data
    test_data = {
        "booster1": {
            "strength": 42,
            "direction": 120
        },
        "booster2": {
            "strength": 38,
            "direction": 240
        },
        "booster3": {
            "strength": 45,
            "direction": 360
        }
    }
    
    // Process the data from each booster
    booster1_result = process_booster1_data(test_data.booster1)
    booster2_result = process_booster2_data(test_data.booster2)
    booster3_result = process_booster3_data(test_data.booster3)
    
    // Calculate the final coordinates using the processed results
    coordinates = calculate_coordinates(
        booster1_result,
        booster2_result,
        booster3_result
    )
    
    return coordinates
}

// Step 6: Execute the triangulation and display the results

function display_harvester_location(location_data) {
    print_to_cora("=== HARVESTER LOCATION IDENTIFIED ===")
    print_to_cora("Coordinates:")
    print_to_cora("X: " + location_data.x)
    print_to_cora("Y: " + location_data.y)
    print_to_cora("Z: " + location_data.z)
    print_to_cora("Confidence level: " + location_data.confidence + "%")
    print_to_cora("=====================================")
}

// Final execution using function composition
display_harvester_location(triangulate_harvester_location())
```

## Learning Application

This side quest teaches several important concepts about function composition and return values:

1. **Basic Function Returns:**
   - Creating functions that process data and return results
   - Using return values in subsequent operations

2. **Function Chaining:**
   - Passing the output of one function directly as input to another
   - Building processing pipelines through function composition

3. **Higher-Order Functions:**
   - Creating functions that return other functions
   - Customizing behavior through function parameters

4. **Problem Decomposition:**
   - Breaking complex problems into smaller, manageable functions
   - Organizing code into logical, reusable components

5. **Multi-Parameter Functions:**
   - Working with functions that accept multiple inputs
   - Combining multiple data points to calculate results

6. **Complete Processing Pipelines:**
   - Designing end-to-end data processing systems
   - Creating clear, modular code structures

## Success Criteria
- All three signal boosters are repaired and programmed correctly
- Each booster successfully processes different aspects of the Harvester signal
- The triangulation system accurately determines the Harvester's coordinates
- The confidence level of the location is at least 85%
- The signal processing code demonstrates proper function composition

## Reward Delivery

Upon successful completion of the side quest:

**Code Example Reward:**
CORA adds the "Function Composition Toolkit" to her database, providing Alex with reusable code for:
- Signal processing functions
- Higher-order function templates
- Data transformation pipelines
- Modular function organization patterns
- Function chaining techniques

**Physical Rewards:**
- A Chronovyan Signal Amplifier that can be integrated into CORA's systems
- Precise coordinates for the Resource Harvester location
- A set of portable signal boosters that can be deployed for future exploration
- Component schematics for building additional signal processing devices

**Knowledge Reward:**
CORA's analysis of the signal boosters reveals:
- Chronovyan engineers used function composition extensively in their technology
- The signal boosters employ a sophisticated form of temporal filtering
- Professor Thorne was developing a communication system based on these principles
- Signal patterns suggest the Resource Harvester has multiple operational modes

## Connection to Main Quest

This side quest directly prepares Alex for the "Resource Harvester Activation" main quest in several ways:

1. **Technical Preparation:**
   - The function composition techniques will be essential for diagnosing and activating Harvester subsystems
   - The modular programming approach mirrors how the Harvester's control systems are structured
   - The signal processing functions will help interpret the Harvester's status indicators

2. **Narrative Connection:**
   - The triangulation system provides the exact location of the Resource Harvester
   - The signal analysis reveals information about the Harvester's operational state
   - Professor Thorne's notes suggest connections between the signal boosters and Harvester control systems

3. **Specific Main Quest Applications:**
   - The diagnostic function in the main quest requires a chain of function calls similar to the signal processing pipeline
   - The power distribution algorithms use function composition to manage complex resource flows
   - The collection array calibration involves creating specialized functions for each array type

**CORA's Connection Dialogue:**
> "Alex, these signal boosters have given us more than just the Harvester's location. The way Chronovyan technology breaks complex operations into chains of simpler functions is fascinating! I can see from the signal patterns that the Resource Harvester itself uses similar principles in its control systems. Each collection array appears to process resources through a series of specialized functions, and the power distribution system uses function composition to optimize energy flows. The function composition techniques you've learned here will be crucial when we attempt to reactivate these systems. Plus, now we have exact coordinates! The Resource Harvester is approximately 3.2 kilometers northeast of our current position, at coordinates we can now follow precisely." 