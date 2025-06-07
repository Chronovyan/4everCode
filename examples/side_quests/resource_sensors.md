# Side Quest: Resource Sensors

## Overview

**Quest Title:** Resource Sensors  
**Chapter:** Act I, Chapter 3: Sustainable Survival  
**Difficulty:** Intermediate  
**Concepts Taught:** Event listeners, event handlers, event triggering, reactive programming  
**Prerequisites:** Basic Chronovyan syntax, functions (from "Signal Boosters" side quest), arrays (from "Data Patterns" side quest)

## Quest Description

With the Resource Harvester's location pinpointed thanks to the signal boosters, Alex and CORA prepare for their approach. However, as they study Professor Thorne's detailed notes about the Harvester, they discover concerning information: the area immediately surrounding the Harvester experiences unpredictable "temporal resource blooms" and dangerous "energy discharges" that occur at random intervals.

Professor Thorne's notes indicate that these phenomena are key to understanding and safely activating the Harvester. The resource blooms represent rare, ephemeral materials that spontaneously manifest due to the Harvester's dormant energy field and are crucial for optimal operation. Meanwhile, the energy discharges pose serious dangers that could damage CORA's systems or harm Alex if they're caught unaware.

To safely navigate this environment and gather the necessary resources, they must establish a network of programmable Chronovyan sensor pylons around the Harvester's perimeter. These pylons can be programmed to continuously monitor for specific events and react appropriately when they occur – a programming paradigm Professor Thorne described as "event-driven" in his notes.

## Gameplay Elements

### Environmental Features
- A series of clearings around the Resource Harvester where sensor pylons can be placed
- Visible but unpredictable energy discharges that pulse from the Harvester at random intervals
- Shimmering air distortions that indicate potential resource bloom locations
- Remnants of Professor Thorne's previous sensor attempts, partially damaged but salvageable
- A central control station where all sensor data can be monitored

### CORA's Role
- CORA explains the concept of event-driven programming based on Professor Thorne's notes
- She can detect energy discharges and resource blooms but only within a limited range
- She provides technical specifications for programming the sensor pylons
- She simulates different environmental events for testing sensor programming
- She analyzes collected data to identify patterns in the Harvester's behavior

### Alex's Tasks
1. Recover and repair damaged sensor pylons from Professor Thorne's camp
2. Place sensors at strategic locations around the Harvester's perimeter
3. Program each sensor to listen for specific environmental events
4. Create appropriate handler functions that respond to detected events
5. Develop a central monitoring system that aggregates data from all sensors
6. Test the sensor network by gathering resources and avoiding hazards

## Initial Discovery

As Alex and CORA approach the coordinates identified by the signal boosters, they begin to notice strange phenomena in the surrounding environment. Occasional flashes of light burst from seemingly random locations, and shimmering distortions appear and disappear in the air.

**CORA:** "Alex, I'm detecting unusual energy patterns all around us. These match Professor Thorne's descriptions of the area surrounding the Resource Harvester. My sensors can detect these events, but only after they've already occurred, which isn't very helpful for avoiding hazards or gathering resources."

Alex finds a small camp that Professor Thorne had established on a ridge overlooking the Harvester site. Among the abandoned equipment is a data terminal with a partial research log:

> *"Day 47: The Harvester's dormant state is causing unpredictable effects in the surrounding area. Energy discharges occur without warning – nearly lost my hand to one yesterday. More fascinating are the resource blooms: rare materials temporarily manifesting due to temporal distortion. These ephemeral resources are key to optimal Harvester function.*
> 
> *Day 48: Standard monitoring ineffective. Developing network of programmable Chronovyan sensors using event-driven approach. Each sensor continuously watches for specific conditions and executes response routines when triggered. First tests promising – managed to collect three crystal blooms before they dissipated.*
> 
> *Day 49: Expanding sensor network. Key insight: Chronovyan coding allows for 'event listeners' that wait for specific conditions, then trigger 'handler functions' in response. Much more efficient than continuous polling. Six sensors now operational, two more needed for complete coverage.*
> 
> *Day 52: Temporal storm damaged half the sensor network. Must repair before approaching Harvester core. Critical safety protocols depend on advance warning system...*"

The log ends abruptly. Nearby, Alex discovers several damaged sensor pylons.

**CORA:** "This is exactly what we need! Professor Thorne was using event-driven programming to monitor the Harvester's effects. Instead of continuously checking for conditions, the sensors were programmed to 'listen' for specific events and react when they occurred. If we can repair these sensors and complete the network, we can safely gather the resources we need to activate the Harvester."

## Coding Challenge

### Phase 1: Basic Event Listeners and Handlers

**CORA:** "Let's start by understanding event listeners. In Chronovyan, you can create a function that will be called automatically when a specific event occurs."

```chronovyan
// Define an event handler function
function handle_energy_discharge(event_data) {
    print_to_cora("Energy discharge detected!")
    print_to_cora("Intensity: " + event_data.intensity)
    print_to_cora("Location: " + event_data.location)
    
    // Respond based on intensity
    if (event_data.intensity > 75) {
        print_to_cora("DANGER! Seek shelter immediately!")
    } else {
        print_to_cora("Moderate discharge. Maintain safe distance.")
    }
}

// Register the event listener
register_event_listener("energy_discharge", handle_energy_discharge)

print_to_cora("Energy discharge monitoring active...")
```

**Task for Alex:** Program the first sensor pylon to listen for energy discharges and provide appropriate warnings based on intensity.

### Phase 2: Multiple Event Types

**CORA:** "Now let's expand our sensor to listen for different types of events. Each event type needs its own handler function."

```chronovyan
// Resource bloom event handler
function handle_resource_bloom(event_data) {
    print_to_cora("Resource bloom detected!")
    print_to_cora("Resource type: " + event_data.resource_type)
    print_to_cora("Duration: " + event_data.duration + " seconds")
    print_to_cora("Coordinates: " + event_data.coordinates)
    
    if (event_data.resource_type == "chronocrystal") {
        print_to_cora("High priority resource! Collecting immediately.")
        initiate_collection(event_data.coordinates)
    } else {
        print_to_cora("Standard resource logged for later collection.")
        log_resource_location(event_data)
    }
}

// Register multiple event listeners
register_event_listener("energy_discharge", handle_energy_discharge)
register_event_listener("resource_bloom", handle_resource_bloom)

print_to_cora("Multiple event monitoring active...")
```

**Task for Alex:** Program the second sensor pylon to listen for both energy discharges and resource blooms, with appropriate responses for each.

### Phase 3: Custom Event Filtering

**CORA:** "We need to be more selective about which events trigger our handlers. Let's add filtering criteria to our event listeners."

```chronovyan
// Register event listener with filtering criteria
register_event_listener("resource_bloom", handle_resource_bloom, {
    "minimum_duration": 30,
    "resource_types": ["chronocrystal", "temporal_alloy", "flux_matter"],
    "minimum_quality": 70
})

// More advanced filtering with a custom filter function
function temperature_spike_filter(event_data) {
    // Only handle dangerous temperature spikes
    return event_data.temperature > 80 && event_data.rate_of_change > 5
}

register_event_listener("temperature_change", handle_temperature_spike, 
                       temperature_spike_filter)

print_to_cora("Filtered event monitoring active...")
```

**Task for Alex:** Program the third sensor pylon with custom filtering to focus on high-priority events and ignore background noise.

### Phase 4: Event Priorities and Conflict Resolution

**CORA:** "Sometimes multiple events might occur simultaneously. We need to establish priorities for handling these conflicts."

```chronovyan
// Register event listener with priority level
register_event_listener("energy_discharge", handle_energy_discharge, null, 10)
register_event_listener("resource_bloom", handle_resource_bloom, null, 5)
register_event_listener("temperature_change", handle_temperature_spike, null, 8)

// Event queue processor
function process_event_queue(event_queue) {
    // Sort events by priority (higher numbers = higher priority)
    sorted_events = sort_by_priority(event_queue)
    
    // Process events in priority order
    for (i = 0; i < sorted_events.length; i++) {
        current_event = sorted_events[i]
        trigger_event_handler(current_event.type, current_event.data)
        
        // Check if we need to abort lower priority processing
        if (current_event.requires_immediate_attention) {
            print_to_cora("Critical event requires immediate attention!")
            print_to_cora("Pausing processing of lower priority events.")
            break
        }
    }
}

print_to_cora("Prioritized event handling active...")
```

**Task for Alex:** Implement a priority system for the sensor network to properly handle situations when multiple events occur simultaneously.

### Phase 5: Creating Custom Events

**CORA:** "We can also create our own custom events based on combinations of sensor readings or analysis of patterns."

```chronovyan
// Function to check for specific conditions and trigger a custom event
function check_harvester_activation_pattern() {
    // Check if a specific pattern of events has occurred
    if (resource_blooms_detected.length >= 3 &&
        last_energy_discharge_time > (current_time - 300) &&
        temperature_rising_steadily) {
        
        // Create custom event data
        activation_event_data = {
            "pattern_complete": true,
            "resource_types": resource_blooms_detected,
            "timestamp": current_time,
            "confidence": 85
        }
        
        // Trigger custom event
        trigger_event("harvester_activation_imminent", activation_event_data)
    }
}

// Register handler for our custom event
register_event_listener("harvester_activation_imminent", 
                       handle_harvester_activation, null, 15)

// Schedule regular checks
set_interval(check_harvester_activation_pattern, 60)  // Check every 60 seconds

print_to_cora("Custom event detection active...")
```

**Task for Alex:** Create a system that monitors for specific patterns of events and triggers custom notifications when important combinations are detected.

### Final Challenge: Complete Sensor Network Integration

**CORA:** "Now we need to integrate all our sensors into a unified network that can provide comprehensive monitoring and coordinated responses."

```chronovyan
// Create the centralized sensor network manager
function initialize_sensor_network() {
    // Initialize each sensor
    sensors = []
    
    // North sensor
    sensors.push(initialize_sensor("north", {
        "position": {"x": 100, "y": 0},
        "events": ["energy_discharge", "resource_bloom"],
        "range": 50
    }))
    
    // East sensor
    sensors.push(initialize_sensor("east", {
        "position": {"x": 0, "y": 100},
        "events": ["temperature_change", "radiation_spike"],
        "range": 50
    }))
    
    // South sensor
    sensors.push(initialize_sensor("south", {
        "position": {"x": -100, "y": 0},
        "events": ["resource_bloom", "temporal_distortion"],
        "range": 50
    }))
    
    // West sensor
    sensors.push(initialize_sensor("west", {
        "position": {"x": 0, "y": -100},
        "events": ["energy_discharge", "atmospheric_change"],
        "range": 50
    }))
    
    // Register the network-wide event handler
    register_network_event_handler(process_network_event)
    
    return sensors
}

// Central event handler for the entire network
function process_network_event(event_data) {
    // Log all events
    log_event(event_data)
    
    // Update the map with event location
    update_event_map(event_data)
    
    // Notify CORA of critical events
    if (is_critical_event(event_data)) {
        alert_cora(event_data)
    }
    
    // Trigger appropriate response based on event type
    execute_response_protocol(event_data)
}

// Initialize and activate the network
sensor_network = initialize_sensor_network()
activate_sensor_network(sensor_network)

print_to_cora("Complete sensor network activated.")
print_to_cora("Monitoring for all event types around the Harvester perimeter.")
```

**Task for Alex:** Implement the complete sensor network that integrates all individual pylons into a coordinated system, providing comprehensive monitoring of the Harvester's surroundings.

## Complete Solution

```chronovyan
// Resource Sensors - Comprehensive Monitoring System
// This program establishes a network of event-driven sensors around the Resource Harvester

// Step 1: Define event handler functions for different event types

// Energy discharge handler
function handle_energy_discharge(event_data) {
    print_to_cora("ALERT: Energy discharge detected")
    print_to_cora("Location: " + format_coordinates(event_data.coordinates))
    print_to_cora("Intensity: " + event_data.intensity + "%")
    print_to_cora("Type: " + event_data.discharge_type)
    
    // Determine appropriate response based on intensity
    if (event_data.intensity > 85) {
        print_to_cora("CRITICAL DANGER - Seek immediate shelter!")
        activate_emergency_shields()
        sound_alarm("critical")
    } else if (event_data.intensity > 60) {
        print_to_cora("HIGH DANGER - Evacuate affected sector")
        sound_alarm("high")
    } else {
        print_to_cora("CAUTION - Maintain safe distance of " + 
                     (event_data.intensity * 0.5) + " meters")
    }
    
    // Log the event for pattern analysis
    log_discharge_event(event_data)
}

// Resource bloom handler
function handle_resource_bloom(event_data) {
    print_to_cora("OPPORTUNITY: Resource bloom detected")
    print_to_cora("Resource type: " + event_data.resource_type)
    print_to_cora("Quality: " + event_data.quality + "%")
    print_to_cora("Location: " + format_coordinates(event_data.coordinates))
    print_to_cora("Estimated duration: " + event_data.duration + " seconds")
    
    // Check if this is a high-priority resource
    is_priority = high_priority_resources.includes(event_data.resource_type)
    
    if (is_priority && event_data.quality > 75) {
        print_to_cora("HIGH PRIORITY RESOURCE - Immediate collection recommended")
        mark_on_map(event_data.coordinates, "high_priority")
        
        if (event_data.duration < 60) {
            print_to_cora("WARNING: Rapid dissipation imminent!")
        }
    } else {
        print_to_cora("Standard resource logged for collection")
        mark_on_map(event_data.coordinates, "standard")
    }
    
    // Log the bloom for pattern analysis
    log_bloom_event(event_data)
}

// Temperature change handler
function handle_temperature_change(event_data) {
    print_to_cora("INFORMATION: Temperature anomaly detected")
    print_to_cora("Current temperature: " + event_data.temperature + "°C")
    print_to_cora("Rate of change: " + event_data.rate_of_change + "°C/min")
    print_to_cora("Location: " + format_coordinates(event_data.coordinates))
    
    // Check for dangerous conditions
    if (event_data.temperature > 75) {
        print_to_cora("WARNING: Extreme heat detected")
        
        if (event_data.rate_of_change > 5) {
            print_to_cora("DANGER: Rapid temperature increase!")
            sound_alarm("moderate")
        }
    } else if (event_data.temperature < -10) {
        print_to_cora("WARNING: Extreme cold detected")
    }
    
    // Log temperature data
    log_temperature_event(event_data)
}

// Temporal distortion handler
function handle_temporal_distortion(event_data) {
    print_to_cora("CAUTION: Temporal distortion detected")
    print_to_cora("Magnitude: " + event_data.magnitude)
    print_to_cora("Radius: " + event_data.radius + " meters")
    print_to_cora("Location: " + format_coordinates(event_data.coordinates))
    
    if (event_data.magnitude > 7) {
        print_to_cora("DANGER: Severe temporal distortion!")
        print_to_cora("Time flow differential: " + event_data.time_dilation_factor + "x")
        sound_alarm("high")
        mark_on_map(event_data.coordinates, "danger_zone", event_data.radius)
    }
    
    // Log temporal data
    log_temporal_event(event_data)
}

// Step 2: Define event filters for more precise monitoring

// Filter for significant energy discharges only
function energy_discharge_filter(event_data) {
    return event_data.intensity > 30 || 
           event_data.discharge_type == "chronoelectric";
}

// Filter for valuable resource blooms only
function resource_bloom_filter(event_data) {
    return event_data.quality > 50 && 
           event_data.duration > 45;
}

// Filter for rapid temperature changes
function temperature_change_filter(event_data) {
    return Math.abs(event_data.rate_of_change) > 2;
}

// Step 3: Register event listeners with appropriate filters and priorities

function setup_event_listeners() {
    // Clear any existing listeners
    reset_event_listeners()
    
    // Register with appropriate filters and priorities
    // Higher priority number = more important
    register_event_listener("energy_discharge", 
                          handle_energy_discharge, 
                          energy_discharge_filter, 
                          10)
    
    register_event_listener("resource_bloom", 
                          handle_resource_bloom, 
                          resource_bloom_filter, 
                          8)
    
    register_event_listener("temperature_change", 
                          handle_temperature_change, 
                          temperature_change_filter, 
                          5)
    
    register_event_listener("temporal_distortion", 
                          handle_temporal_distortion, 
                          null,  // No filter, handle all distortions
                          9)
}

// Step 4: Set up custom event detection for pattern recognition

// Track recent events to look for patterns
recent_energy_discharges = []
recent_resource_blooms = []
recent_temperature_changes = []

// Function to detect patterns in recent events
function analyze_event_patterns() {
    // Check for harvester activation pattern
    // (Energy discharge followed by specific resource blooms in sequence)
    if (recent_energy_discharges.length > 0 && 
        recent_resource_blooms.length >= 3) {
        
        latest_discharge = recent_energy_discharges[recent_energy_discharges.length - 1]
        
        // Check if we have the right sequence of resource types
        has_chronocrystal = false
        has_temporal_alloy = false
        has_flux_matter = false
        
        for (i = 0; i < recent_resource_blooms.length; i++) {
            bloom = recent_resource_blooms[i]
            if (bloom.resource_type == "chronocrystal") has_chronocrystal = true
            if (bloom.resource_type == "temporal_alloy") has_temporal_alloy = true
            if (bloom.resource_type == "flux_matter") has_flux_matter = true
        }
        
        // If we have all required resources and discharge was recent
        if (has_chronocrystal && has_temporal_alloy && has_flux_matter &&
            (current_time - latest_discharge.timestamp) < 600) {
            
            // Create activation pattern event
            pattern_event = {
                "pattern_name": "harvester_activation_sequence",
                "confidence": 85,
                "timestamp": current_time,
                "description": "Resource Harvester activation sequence detected",
                "required_action": "Prepare for Harvester activation protocol"
            }
            
            // Trigger the custom event
            trigger_event("pattern_detected", pattern_event)
        }
    }
    
    // Check for dangerous energy buildup pattern
    // (Increasing temperature followed by multiple energy discharges)
    if (recent_temperature_changes.length >= 3 && 
        recent_energy_discharges.length >= 2) {
        
        // Check if temperature has been steadily increasing
        temp_increasing = true
        for (i = 1; i < recent_temperature_changes.length; i++) {
            if (recent_temperature_changes[i].temperature <= 
                recent_temperature_changes[i-1].temperature) {
                temp_increasing = false
                break
            }
        }
        
        // Check if discharge intensity is increasing
        if (temp_increasing && 
            recent_energy_discharges.length >= 2 &&
            recent_energy_discharges[recent_energy_discharges.length - 1].intensity >
            recent_energy_discharges[recent_energy_discharges.length - 2].intensity) {
            
            // Create danger pattern event
            pattern_event = {
                "pattern_name": "cascading_energy_buildup",
                "confidence": 75,
                "timestamp": current_time,
                "description": "Dangerous energy buildup detected - cascading discharge possible",
                "required_action": "Immediate evacuation recommended"
            }
            
            // Trigger the custom event
            trigger_event("pattern_detected", pattern_event)
        }
    }
}

// Handler for pattern detection events
function handle_pattern_detection(event_data) {
    print_to_cora("PATTERN DETECTED: " + event_data.pattern_name)
    print_to_cora("Description: " + event_data.description)
    print_to_cora("Confidence: " + event_data.confidence + "%")
    print_to_cora("Recommended action: " + event_data.required_action)
    
    // Log the pattern for later analysis
    log_pattern_event(event_data)
    
    // Mark on the map
    mark_pattern_on_map(event_data)
    
    // Alert based on pattern type
    if (event_data.pattern_name == "harvester_activation_sequence") {
        sound_alarm("notification")
    } else if (event_data.pattern_name == "cascading_energy_buildup") {
        sound_alarm("critical")
    }
}

// Step 5: Initialize and activate the sensor network

function initialize_sensor_network() {
    // Define high priority resources
    high_priority_resources = ["chronocrystal", "temporal_alloy", "flux_matter"]
    
    // Set up event listeners
    setup_event_listeners()
    
    // Register pattern detection handler
    register_event_listener("pattern_detected", handle_pattern_detection, null, 15)
    
    // Schedule regular pattern analysis
    set_interval(analyze_event_patterns, 30)  // Check every 30 seconds
    
    // Clear event history
    recent_energy_discharges = []
    recent_resource_blooms = []
    recent_temperature_changes = []
    
    // Set up event history maintenance (keep only recent events)
    set_interval(function() {
        // Keep only events from the last 10 minutes
        cutoff_time = current_time - 600
        
        recent_energy_discharges = recent_energy_discharges.filter(
            function(event) { return event.timestamp > cutoff_time; }
        )
        
        recent_resource_blooms = recent_resource_blooms.filter(
            function(event) { return event.timestamp > cutoff_time; }
        )
        
        recent_temperature_changes = recent_temperature_changes.filter(
            function(event) { return event.timestamp > cutoff_time; }
        )
    }, 60)  // Run cleanup every 60 seconds
    
    print_to_cora("Sensor network initialization complete")
}

// Step 6: Event logging functions to maintain history

function log_discharge_event(event_data) {
    event_data.timestamp = current_time
    recent_energy_discharges.push(event_data)
}

function log_bloom_event(event_data) {
    event_data.timestamp = current_time
    recent_resource_blooms.push(event_data)
}

function log_temperature_event(event_data) {
    event_data.timestamp = current_time
    recent_temperature_changes.push(event_data)
}

function log_temporal_event(event_data) {
    // Just log to the general event log
    log_event(event_data)
}

// Step 7: Activate the network

initialize_sensor_network()
print_to_cora("Resource sensor network active")
print_to_cora("Monitoring for events around the Harvester perimeter...")
print_to_cora("Ready to detect resource opportunities and potential hazards")
```

## Learning Application

This side quest teaches several important concepts about event-driven programming:

1. **Event Listeners and Handlers:**
   - Registering functions to respond to specific events
   - Creating handler functions that process event data
   - Understanding the event-driven execution model

2. **Event Filtering:**
   - Filtering events based on criteria
   - Creating custom filter functions
   - Preventing handler overload with selective processing

3. **Event Priorities:**
   - Assigning priorities to different event types
   - Handling conflicts when multiple events occur
   - Implementing interrupt-style processing for critical events

4. **Custom Events:**
   - Creating and triggering custom events
   - Building higher-level abstractions from low-level events
   - Implementing pattern detection systems

5. **Event History and Analysis:**
   - Maintaining logs of past events
   - Analyzing event sequences for patterns
   - Making predictions based on historical data

6. **Comprehensive Event Systems:**
   - Building a coordinated network of event listeners
   - Centralizing event processing
   - Creating modular, responsive systems

## Success Criteria
- A network of at least four sensor pylons is successfully deployed around the Harvester
- Each sensor is programmed to detect specific event types with appropriate handlers
- The system successfully filters out insignificant events while detecting important ones
- The pattern detection system identifies at least two significant event sequences
- Alex successfully collects at least three high-priority resources using the sensor network
- The early warning system prevents injury from at least one dangerous energy discharge

## Reward Delivery

Upon successful completion of the side quest:

**Code Example Reward:**
CORA adds the "Event-Driven Programming Toolkit" to her database, providing Alex with reusable code for:
- Event listener registration
- Event handler implementation
- Event filtering techniques
- Event priority management
- Pattern detection algorithms
- Comprehensive event system architecture

**Physical Rewards:**
- A portable Chronovyan sensor module that can be attached to CORA
- A collection of rare resources gathered using the sensor network
- Professor Thorne's notes on Chronovyan event-driven systems
- A detailed map of the Harvester perimeter with marked resource bloom locations

**Knowledge Reward:**
CORA's analysis of the sensor data reveals:
- The Resource Harvester operates on an event-driven cycle
- Resource blooms follow predictable patterns related to the Harvester's energy state
- Energy discharges serve as a protective mechanism for the Harvester
- The specific sequence of events needed to safely initiate the Harvester's activation

## Connection to Main Quest

This side quest directly prepares Alex for the "Resource Harvester Activation" main quest in several ways:

1. **Technical Preparation:**
   - The event-driven programming model is essential for interacting with the Harvester's control systems
   - The Harvester's collection arrays operate on an event-based model, responding to resource availability
   - The Harvester's safety systems use event listeners to monitor for dangerous conditions

2. **Narrative Connection:**
   - The rare resources collected using the sensor network are needed for the Harvester activation
   - The pattern detection system reveals clues about the Harvester's operational cycle
   - Professor Thorne's notes about event-driven systems explain key aspects of the Harvester's design

3. **Specific Main Quest Applications:**
   - The Harvester's diagnostic system uses event listeners to monitor subsystem status
   - The resource classification system triggers events when specific materials are detected
   - The power distribution system responds to events from collection arrays and storage systems
   - The central control interface uses an event-driven model for user interaction

**CORA's Connection Dialogue:**
> "Alex, these sensor pylons have given us crucial insights into how the Resource Harvester functions. The event-driven approach we've used here mirrors exactly how the Harvester itself operates! Each of its collection arrays continuously listens for specific resource types, and its power systems respond to events from its various components. The control systems Professor Thorne was trying to activate use the same event handler pattern we've implemented in our sensors. And now we have a supply of the rare resources we'll need to initiate the activation sequence. Plus, by detecting these patterns of energy discharges and resource blooms, we've essentially mapped out the Harvester's operational rhythm. We're now fully prepared to approach the central facility and begin the activation process." 