/**
 * @file paradox_demo.cpp
 * @brief Demonstrates the use of paradox management functions in the Chronovyan Standard Library
 *
 * // Chronovyan Example: Paradox Management Demonstration
 * // Difficulty: Intermediate to Advanced
 * // Concepts: Paradox Detection, Timeline Stability, Conflict Resolution, Temporal Dependencies
 *
 * This example shows how to detect, handle, and manage temporal paradoxes
 * using the Paradox Management subsystem. It demonstrates how to monitor
 * timeline stability, respond to dangerous paradox levels, and recover from
 * potential causality violations.
 *
 * @copyright Chronovyan Project
 */

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// Include the Chronovyan Standard Library headers
#include "stdlib/chronovyan_stdlib.h"
#include "stdlib/paradox_management.h"
#include "stdlib/resource_management.h"

// Using directives for simplicity in the demo
using namespace chronovyan::stdlib;

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
 * @brief Utility function to print paradox levels with a visual indicator
 * @param label Description of the paradox measurement
 * @param level The current paradox level (0.0 to 1.0)
 */
void print_paradox_level(const std::string& label, double level) {
    std::cout << std::left << std::setw(25) << label << ": " << std::fixed << std::setprecision(2)
              << std::setw(4) << level;

    std::cout << " [";
    int bars = static_cast<int>(level * 20);
    for (int i = 0; i < 20; ++i) {
        if (i < bars) {
            if (level < 0.25) {
                std::cout << "-";  // Stable - use dash
            } else if (level < 0.5) {
                std::cout << "~";  // Caution - use tilde
            } else if (level < 0.75) {
                std::cout << "!";  // Warning - use exclamation
            } else {
                std::cout << "#";  // Critical - use hash
            }
        } else {
            std::cout << " ";
        }
    }
    std::cout << "] ";

    // Display status label with appropriate emphasis
    if (level < 0.25) {
        std::cout << "STABLE";
    } else if (level < 0.5) {
        std::cout << "CAUTION";
    } else if (level < 0.75) {
        std::cout << "\033[33mWARNING\033[0m";  // Yellow text for warning
    } else {
        std::cout << "\033[31mCRITICAL\033[0m";  // Red text for critical
    }

    std::cout << std::endl;
}

/**
 * @brief Simulates a timeline operation that increases paradox risk
 *
 * This function demonstrates how different operations can affect
 * the paradox level of the timeline, potentially causing instability.
 *
 * @param operation_name Name of the operation being performed
 * @param paradox_contribution Amount by which this operation increases paradox level
 * @return true if operation succeeded, false if it failed due to high paradox
 */
bool perform_timeline_operation(const std::string& operation_name, double paradox_contribution) {
    std::cout << "⚙️ Performing operation: " << operation_name << std::endl;
    std::cout << "  Paradox contribution: " << std::fixed << std::setprecision(2)
              << paradox_contribution << std::endl;

    // Check for paradoxes with temporal dependencies
    // Dependencies track how states at different times affect each other
    std::cout << "  Checking temporal dependencies..." << std::endl;
    std::vector<std::string> dependencies = {"future_state", "past_state"};
    bool is_paradox = !paradox_check(dependencies);

    if (is_paradox) {
        std::cout << "  ⚠️  Potential paradox detected between dependencies" << std::endl;
    }

    // Get current paradox level
    double current = get_paradox_level();

    // Add more paradox contribution if a potential paradox was detected
    if (is_paradox) {
        paradox_contribution *= 1.5;  // Increase risk if paradox detected
    }
    std::cout << "  Current paradox level: " << current << std::endl;

    // Calculate new paradox level
    double new_level = current + paradox_contribution;
    std::cout << "  Projected paradox level: " << new_level << std::endl;

    // Update the paradox level
    // In a real application, this would happen automatically as a result of the operation
    trigger_paradox_level(new_level);

    // Simulate a slight delay in the operation
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // Check if operation succeeded (paradox level below critical)
    bool success = new_level < 0.8;

    if (success) {
        std::cout << "  ✅ Operation completed successfully" << std::endl;
    } else {
        std::cout << "  ❌ Operation failed - critical paradox level exceeded" << std::endl;
        std::cout << "     Timeline stability compromised" << std::endl;
    }

    return success;
}

/**
 * @brief Attempts to reduce the paradox level and stabilize the timeline
 *
 * This function demonstrates how to recover from dangerous paradox levels
 * by using the stabilize_timeline function.
 *
 * @return true if stabilization succeeded, false if it failed
 */
bool attempt_timeline_stabilization() {
    std::cout << "🔄 Initiating timeline stabilization sequence..." << std::endl;

    // Get current paradox level before stabilization
    double before_level = get_paradox_level();
    std::cout << "  Initial paradox level: " << std::fixed << std::setprecision(2) << before_level
              << std::endl;

    // Attempt to stabilize the timeline using Aethel
    std::cout << "  Applying stabilization fields..." << std::endl;
    AETHEL_RESERVE reserve = 1000;  // Example reserve
    PARADOX_LEVEL new_level = stabilize_timeline(reserve, 100);
    bool success = new_level < 0.8;

    // Get new paradox level after stabilization
    double after_level = get_paradox_level();
    double reduction = before_level - after_level;

    if (success) {
        std::cout << "  ✅ Stabilization successful" << std::endl;
        std::cout << "  Paradox reduction: " << std::fixed << std::setprecision(2) << reduction
                  << " (" << (reduction / before_level * 100) << "%)" << std::endl;
    } else {
        std::cout << "  ❌ Stabilization failed" << std::endl;
        std::cout << "  Paradox reduction: " << std::fixed << std::setprecision(2) << reduction
                  << " (insufficient)" << std::endl;
    }

    return success;
}

/**
 * @brief Demonstrates how to detect and resolve temporal conflicts
 *
 * This function shows how to check for conflicts among temporal dependencies
 * and resolve them to prevent paradoxes.
 *
 * @param dependencies List of temporal dependencies to check
 * @return true if conflicts were successfully resolved, false otherwise
 */
bool resolve_temporal_conflicts(const std::vector<std::string>& dependencies) {
    std::cout << "🔍 Analyzing temporal dependencies for conflicts..." << std::endl;

    // Print all dependencies being checked
    std::cout << "  Dependencies under analysis:" << std::endl;
    for (const auto& dep : dependencies) {
        std::cout << "  - " << dep << std::endl;
    }

    // Check for conflicts among dependencies
    bool has_conflicts = !paradox_check(dependencies);

    if (has_conflicts) {
        std::cout << "  ⚠️ Detected temporal conflicts among dependencies" << std::endl;

        // In a real application, we would identify specific conflicts
        std::cout << "  Attempting automated conflict resolution..." << std::endl;

        // Simulate conflict resolution
        double resolution_chance = 0.7;  // 70% chance of successful resolution
        bool resolved = (static_cast<double>(rand()) / RAND_MAX) < resolution_chance;

        if (resolved) {
            std::cout << "  ✅ Conflicts successfully resolved" << std::endl;
            // Reduce paradox level slightly to represent successful resolution
            double current = get_paradox_level();
            trigger_paradox_level(current - 0.1);
            return true;
        } else {
            std::cout << "  ❌ Conflict resolution failed" << std::endl;
            std::cout << "     Manual intervention required" << std::endl;
            return false;
        }
    } else {
        std::cout << "  ✅ No temporal conflicts detected" << std::endl;
        return true;
    }
}

/**
 * @brief Main function for the Chronovyan Paradox Management Demo
 */
int main() {
    // Seed random number generator for conflict resolution simulation
    srand(static_cast<unsigned int>(time(nullptr)));

    // Application header
    std::cout << std::string(60, '*') << std::endl;
    std::cout << "          Chronovyan Paradox Management Demo" << std::endl;
    std::cout << std::string(60, '*') << std::endl;

    // Initialize the Chronovyan Standard Library
    std::cout << "\nInitializing Chronovyan Standard Library..." << std::endl;
    if (!initialize()) {
        std::cerr << "ERROR: Failed to initialize the standard library. Exiting." << std::endl;
        return 1;
    }
    std::cout << "Initialization successful. Library is ready to use." << std::endl;

    // Start with baseline paradox level
    print_section_header("Baseline Measurement", "Establishing initial timeline stability");
    print_paradox_level("Initial paradox level", get_paradox_level());

    // Register alert handlers for dangerous paradox levels
    std::cout << "\nRegistering paradox alert handlers..." << std::endl;
    set_paradox_alert(0.5, [](double level) {
        std::cout << "\n🚨 \033[33m[PARADOX ALERT]\033[0m Level: " << std::fixed
                  << std::setprecision(2) << level << " - Timeline instability detected!"
                  << std::endl;
        std::cout << "   Recommended action: Stabilize timeline" << std::endl;
    });

    set_paradox_alert(0.75, [](double level) {
        std::cout << "\n🚨 \033[31m[CRITICAL PARADOX ALERT]\033[0m Level: " << std::fixed
                  << std::setprecision(2) << level << " - Severe timeline instability!"
                  << std::endl;
        std::cout << "   Recommended action: Immediate stabilization required" << std::endl;
        std::cout << "   Warning: Potential causality breakdown imminent" << std::endl;
    });

    std::cout << "Alert handlers registered at thresholds: 0.50, 0.75" << std::endl;

    // Perform a series of timeline operations with increasing risk
    print_section_header("Timeline Operations Sequence",
                         "Demonstrating paradox accumulation from operations");

    // First operation - low risk
    perform_timeline_operation("Minor temporal adjustment", 0.15);
    print_paradox_level("Current paradox level", get_paradox_level());

    // Second operation - moderate risk
    perform_timeline_operation("Moderate timeline manipulation", 0.25);
    print_paradox_level("Current paradox level", get_paradox_level());

    // First stabilization attempt
    print_section_header("Intermediate Stabilization", "Reducing paradox level before continuing");
    attempt_timeline_stabilization();
    print_paradox_level("Updated paradox level", get_paradox_level());

    // Additional operations with higher risk
    print_section_header("High-Risk Operations", "Testing system response to dangerous levels");

    // Third operation - higher risk
    perform_timeline_operation("Major timeline divergence", 0.35);
    print_paradox_level("Current paradox level", get_paradox_level());

    // Fourth operation - critical risk
    perform_timeline_operation("Critical causality violation", 0.45);
    print_paradox_level("Current paradox level", get_paradox_level());

    // Check for temporal conflicts
    print_section_header("Conflict Analysis", "Detecting and resolving temporal conflicts");

    std::vector<std::string> dependencies = {"Stability_Field", "Temporal_Rift", "Void_Essence",
                                             "Causality_Chain"};

    resolve_temporal_conflicts(dependencies);
    print_paradox_level("Post-resolution level", get_paradox_level());

    // Final stabilization attempt
    print_section_header("Emergency Stabilization", "Final attempt to recover timeline stability");
    attempt_timeline_stabilization();
    print_paradox_level("Final paradox level", get_paradox_level());

    // Final status report
    print_section_header("Mission Summary", "Analysis of timeline stability management");

    double final_level = get_paradox_level();
    std::cout << "Timeline stability assessment:" << std::endl;

    if (final_level < 0.25) {
        std::cout << "✅ Mission successful: Timeline stabilized" << std::endl;
        std::cout << "   All paradoxes successfully managed" << std::endl;
    } else if (final_level < 0.5) {
        std::cout << "⚠️ Mission partially successful: Timeline moderately stable" << std::endl;
        std::cout << "   Minor paradoxes remain but under control" << std::endl;
    } else {
        std::cout << "❌ Mission failed: Timeline remains unstable" << std::endl;
        std::cout << "   Significant paradoxes persist - further intervention required"
                  << std::endl;
    }

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