/**
 * @file resource_demo.cpp
 * @brief Example program demonstrating usage of the resource management functions
 *
 * // Chronovyan Example: Resource Management Demonstration
 * // Difficulty: Beginner to Intermediate
 * // Concepts: Aethel Harvesting, Pattern Infusion, Resource Analysis, Chronovyan Initialization
 *
 * This program shows how to use the resource management functions in the Chronovyan
 * Standard Library, including harvesting Aethel, infusing patterns, analyzing resource
 * stability, and refining resources using the ResourceManager class.
 *
 * @copyright Chronovyan Project
 */

#include <iomanip>
#include <iostream>
#include <string>

#include "stdlib/chronovyan_stdlib.h"
#include "stdlib/pattern_management.h"
#include "stdlib/resource_management.h"

using namespace chronovyan::stdlib;

// Forward declarations for functions that will be defined later
AETHEL_RESERVE simulate_harvest(const std::string& source);
WEAVE_PATTERN simulate_infusion(const WEAVE_PATTERN& pattern, const std::string& source,
                                const std::string& intent);

/**
 * @brief Prints a section header to improve output readability
 * @param title The title of the section
 * @param subtitle Optional subtitle for additional context
 */
void print_section_header(const std::string& title, const std::string& subtitle = "") {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    if (!subtitle.empty()) {
        std::cout << std::string(60, '-') << std::endl;
        std::cout << "  " << subtitle << std::endl;
    }
    std::cout << std::string(60, '-') << std::endl;
}

/**
 * @brief Prints the result of a resource operation with formatted output
 * @param operation The name of the operation
 * @param value The numeric result value
 * @param details Additional information about the operation
 */
void print_resource_result(const std::string& operation, AETHEL_RESERVE value,
                           const std::string& details = "") {
    std::cout << "  " << std::left << std::setw(25) << operation << ": " << std::setw(8)
              << std::fixed << std::setprecision(2) << value;
    if (!details.empty()) {
        std::cout << "  (" << details << ")";
    }
    std::cout << std::endl;
}

/**
 * @brief Creates a simple pattern for demonstration purposes
 * @param name The name to assign to the pattern
 * @return A WEAVE_PATTERN object with the specified name
 */
WEAVE_PATTERN create_demo_pattern(const std::string& name) {
    // Create a basic pattern with the given name
    WEAVE_PATTERN pattern;
    // In a real application, you would define the pattern structure
    // For demo purposes, we'll just set the name
    pattern.set_metadata("name", name);
    return pattern;
}

/**
 * @brief Demonstrates harvesting Aethel from different sources
 *
 * This function shows:
 * - Harvesting from stable and unstable sources
 * - Error handling for invalid sources
 * - Resource depletion with multiple harvests
 */
void demonstrate_harvesting() {
    print_section_header("Aethel Harvesting Demonstration",
                         "Collecting energy from various temporal sources");

    // Harvest from a stable source (Time Crystals)
    // Time Crystals provide consistent but moderate yields
    std::cout << "Harvesting from stable sources:" << std::endl;
    AETHEL_RESERVE crystal_energy = simulate_harvest("Time_Crystals");
    print_resource_result("Time_Crystals", crystal_energy, "Stable source");

    // Harvest from an unstable source (Temporal Rift)
    // Temporal Rifts provide unpredictable but potentially high yields
    std::cout << "\nHarvesting from unstable sources:" << std::endl;
    AETHEL_RESERVE rift_energy = simulate_harvest("Temporal_Rift");
    print_resource_result("Temporal_Rift", rift_energy, "Unstable source");

    // Demonstrate error handling with invalid source
    std::cout << "\nError handling for invalid sources:" << std::endl;
    AETHEL_RESERVE invalid_energy = simulate_harvest("NonexistentSource");
    print_resource_result("NonexistentSource", invalid_energy, "Invalid source - expected to fail");

    // Demonstrate resource depletion with multiple harvests
    // Each harvest reduces the available energy until the source is depleted
    std::cout << "\nResource depletion demonstration:" << std::endl;
    for (int i = 0; i < 3; i++) {
        AETHEL_RESERVE shard_energy = simulate_harvest("Aethel_Shard");
        print_resource_result("Aethel_Shard: Harvest #" + std::to_string(i + 1), shard_energy,
                              shard_energy == 0 ? "Source depleted" : "Source diminishing");
    }
}

/**
 * @brief Demonstrates pattern infusion with Aethel resources
 *
 * This function shows:
 * - Creating and infusing patterns with different intents
 * - Using different energy sources for infusion
 * - Error handling for invalid parameters
 */
void demonstrate_infusion() {
    print_section_header("Pattern Infusion Demonstration", "Enhancing patterns with Aethel energy");

    // Create demo patterns for different purposes
    WEAVE_PATTERN stabilization = create_demo_pattern("Stabilization");
    WEAVE_PATTERN exploration = create_demo_pattern("Exploration");

    std::cout << "Created base patterns:" << std::endl;
    std::cout << "  - Stabilization (Used for timeline stability)" << std::endl;
    std::cout << "  - Exploration (Used for timeline exploration)" << std::endl;

    // Infuse a pattern with Time Crystals for stability
    std::cout << "\nInfusing pattern with Time Crystals:" << std::endl;
    WEAVE_PATTERN enhanced_stability =
        simulate_infusion(stabilization, "Time_Crystals", "BoostStability");
    print_resource_result("Infusion cost", 100, "Aethel consumed");

    // Infuse a pattern with Aethel Shards for cost efficiency
    std::cout << "\nInfusing pattern with Aethel Shards:" << std::endl;
    WEAVE_PATTERN cost_efficient = simulate_infusion(exploration, "Aethel_Shard", "ReduceCost");
    print_resource_result("Infusion cost", 75, "Aethel consumed");

    // Demonstrate error handling with invalid intent
    std::cout << "\nError handling for invalid intent:" << std::endl;
    try {
        WEAVE_PATTERN invalid_infusion =
            simulate_infusion(stabilization, "Time_Crystals", "InvalidIntent");
    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << std::endl;
    }

    // Demonstrate error handling with invalid source
    std::cout << "\nError handling for invalid source:" << std::endl;
    try {
        WEAVE_PATTERN invalid_source =
            simulate_infusion(stabilization, "NonexistentSource", "BoostStability");
    } catch (const std::exception& e) {
        std::cout << "  Error: " << e.what() << std::endl;
    }
}

/**
 * @brief Demonstrates advanced features of the ResourceManager class
 *
 * This function shows:
 * - Analyzing resource stability
 * - Refining Aethel at different quality levels
 * - Resource usage suggestions
 */
void demonstrate_resource_manager() {
    print_section_header("ResourceManager Demonstration",
                         "Advanced resource analysis and refinement");

    ResourceManager manager;

    // Analyze stability of different Aethel sources
    // Higher stability values indicate more reliable sources
    std::cout << "Analyzing source stability:" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    double tc_stability = manager.analyze_source("Time_Crystals");
    std::cout << "  Time_Crystals     : " << tc_stability << " stability"
              << (tc_stability > 0.8   ? " (Highly stable)"
                  : tc_stability > 0.5 ? " (Moderately stable)"
                                       : " (Unstable)")
              << std::endl;

    double tr_stability = manager.analyze_source("Temporal_Rift");
    std::cout << "  Temporal_Rift     : " << tr_stability << " stability"
              << (tr_stability > 0.8   ? " (Highly stable)"
                  : tr_stability > 0.5 ? " (Moderately stable)"
                                       : " (Unstable)")
              << std::endl;

    double nx_stability = manager.analyze_source("NonexistentSource");
    std::cout << "  NonexistentSource : " << nx_stability << " stability (Invalid source)"
              << std::endl;

    // Demonstrate Aethel refinement at different quality levels
    // Higher quality targets yield less refined Aethel but of better quality
    std::cout << "\nRefining Aethel at different quality levels:" << std::endl;
    AETHEL_RESERVE raw_aethel = 1000;
    std::cout << "  Starting with " << raw_aethel << " units of raw Aethel" << std::endl;

    std::cout << "\n  " << std::left << std::setw(15) << "Quality Target" << std::setw(15)
              << "Yield" << std::setw(15) << "Efficiency"
              << "Use Case" << std::endl;
    std::cout << "  " << std::string(60, '-') << std::endl;

    for (double quality = 0.2; quality <= 1.0; quality += 0.2) {
        AETHEL_RESERVE refined = manager.refine_aethel(raw_aethel, quality);
        double efficiency = refined * 100.0 / raw_aethel;

        std::cout << "  " << std::left << std::setw(15) << quality << std::setw(15) << refined
                  << std::setw(15) << efficiency << "% ";

        // Suggest use cases based on quality level
        if (quality <= 0.2) {
            std::cout << "Basic operations";
        } else if (quality <= 0.4) {
            std::cout << "Standard weaving";
        } else if (quality <= 0.6) {
            std::cout << "Advanced patterns";
        } else if (quality <= 0.8) {
            std::cout << "Precision operations";
        } else {
            std::cout << "Critical stability tasks";
        }
        std::cout << std::endl;
    }

    // Provide resource usage suggestions
    std::cout << "\nResource usage suggestions:" << std::endl;
    std::cout << "  - For critical operations, use high quality (0.8-1.0) refined Aethel"
              << std::endl;
    std::cout << "  - For routine tasks, use standard quality (0.4-0.6) for best efficiency"
              << std::endl;
    std::cout << "  - Harvest from Time_Crystals when stability is important" << std::endl;
    std::cout << "  - Harvest from Temporal_Rift when maximum yield is needed" << std::endl;
}

/**
 * @brief Main function for the Chronovyan Resource Management Demo
 */
int main() {
    // Application header
    std::cout << std::string(60, '*') << std::endl;
    std::cout << "          Chronovyan Resource Management Demo" << std::endl;
    std::cout << std::string(60, '*') << std::endl;

    // Initialize the standard library
    std::cout << "\nInitializing Chronovyan Standard Library..." << std::endl;
    if (!initialize()) {
        std::cerr << "ERROR: Failed to initialize the standard library. Exiting." << std::endl;
        return 1;
    }
    std::cout << "Initialization successful. Library is ready to use." << std::endl;

    // Run the demonstrations
    demonstrate_harvesting();
    demonstrate_infusion();
    demonstrate_resource_manager();

    // Shutdown the standard library
    std::cout << "\nShutting down Chronovyan Standard Library..." << std::endl;
    if (!shutdown()) {
        std::cerr << "WARNING: Failed to properly shut down the standard library." << std::endl;
    } else {
        std::cout << "Shutdown successful. All resources released." << std::endl;
    }

    std::cout << "\n" << std::string(60, '*') << std::endl;
    std::cout << "          Demo completed successfully" << std::endl;
    std::cout << std::string(60, '*') << std::endl;

    return 0;
}

/**
 * @brief Simulates harvesting Aethel from a source
 * @param source The source to harvest from
 * @return Amount of Aethel harvested
 */
AETHEL_RESERVE simulate_harvest(const std::string& source) {
    // Simple simulation of different sources
    if (source == "Time_Crystals")
        return 150;
    if (source == "Temporal_Rift")
        return 50 + (std::rand() % 200);  // Random between 50-250
    if (source == "Aethel_Shard")
        return 80;
    return 0;  // Invalid source
}

/**
 * @brief Simulates infusing a pattern with Aethel
 * @param pattern The pattern to infuse
 * @param source The source of Aethel
 * @param intent The intended effect of the infusion
 * @return The infused pattern
 */
WEAVE_PATTERN simulate_infusion(const WEAVE_PATTERN& pattern, const std::string& source,
                                const std::string& intent) {
    // Simple validation
    if (source != "Time_Crystals" && source != "Aethel_Shard") {
        throw std::runtime_error("Invalid Aethel source: " + source);
    }
    if (intent != "BoostStability" && intent != "ReduceCost") {
        throw std::runtime_error("Invalid infusion intent: " + intent);
    }

    // Return a copy of the pattern with updated metadata
    WEAVE_PATTERN result = pattern;
    result.set_metadata("infused_with", source);
    result.set_metadata("infusion_intent", intent);
    return result;
}