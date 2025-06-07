/**
 * @file pattern_demo.cpp
 * @brief Example program demonstrating usage of the pattern management functions
 *
 * // Chronovyan Example: Pattern Management Demonstration
 * // Difficulty: Intermediate
 * // Concepts: Pattern Creation, Composition, Validation, Application, Temporal Weaving
 *
 * This program shows how to use the pattern management functions in the Chronovyan
 * Standard Library, including creating, composing, validating, and applying patterns
 * to manipulate the temporal fabric.
 *
 * @copyright Chronovyan Project
 */

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "stdlib/chronovyan_stdlib.h"
#include "stdlib/pattern_management.h"
#include "stdlib/resource_management.h"

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
 * @brief Prints pattern properties in a formatted way
 * @param pattern The pattern to display information about
 * @param detailed Whether to show detailed metadata
 */
void print_pattern_info(const WEAVE_PATTERN& pattern, bool detailed = false) {
    std::cout << std::left << std::setw(20) << "Name:" << pattern.get_name() << std::endl;
    std::cout << std::left << std::setw(20) << "Instability Index:" << std::fixed
              << std::setprecision(2) << pattern.get_instability_index() << std::endl;
    std::cout << std::left << std::setw(20) << "Aethel Cost:" << pattern.get_aethel_cost()
              << std::endl;

    if (detailed) {
        std::cout << "\nMetadata:" << std::endl;
        std::cout << "  " << std::left << std::setw(15)
                  << "Author:" << pattern.get_metadata("author") << std::endl;
        std::cout << "  " << std::left << std::setw(15)
                  << "Version:" << pattern.get_metadata("version") << std::endl;
        std::cout << "  " << std::left << std::setw(15)
                  << "Description:" << pattern.get_metadata("description") << std::endl;
        std::cout << "  " << std::left << std::setw(15)
                  << "Category:" << pattern.get_metadata("category") << std::endl;
    }
}

/**
 * @brief Demonstrates basic pattern creation and management
 */
void demonstrate_basic_patterns() {
    print_section_header("Basic Pattern Management",
                         "Creating and examining simple temporal patterns");

    // Create a basic pattern
    std::cout << "Creating a basic stabilization pattern:" << std::endl;
    WEAVE_PATTERN basic_pattern("StabilizationPattern");

    // Get pattern properties
    std::cout << "\nExamining pattern properties:" << std::endl;
    print_pattern_info(basic_pattern);

    // Set and get metadata
    std::cout << "\nEnriching pattern with metadata:" << std::endl;
    basic_pattern.set_metadata("author", "Chronovyan Project");
    basic_pattern.set_metadata("version", "1.0");
    basic_pattern.set_metadata("description", "A simple pattern for timeline stabilization");
    basic_pattern.set_metadata("category", "Stabilization");

    // Display the enriched pattern
    print_pattern_info(basic_pattern, true);

    // Validate pattern
    std::cout << "\nValidating pattern safety:" << std::endl;
    double risk = validate_pattern(basic_pattern);
    std::cout << "Pattern risk level: " << risk;
    if (risk < 0.3) {
        std::cout << " (Safe for general use)";
    } else if (risk < 0.7) {
        std::cout << " (Use with caution)";
    } else {
        std::cout << " (Potentially dangerous)";
    }
    std::cout << std::endl;

    // Apply pattern
    std::cout << "\nApplying pattern to default temporal context:" << std::endl;
    WEAVE_PATTERN context("DefaultContext");
    bool result = apply_pattern(basic_pattern, context);
    std::cout << "Application result: " << (result ? "Success" : "Failure") << std::endl;
    if (result) {
        std::cout << "Timeline stability increased by: "
                  << (1.0 - basic_pattern.get_instability_index()) << std::endl;
    }
}

/**
 * @brief Demonstrates pattern combination techniques
 */
void demonstrate_pattern_composition() {
    print_section_header("Pattern Combination",
                         "Combining patterns to create more complex effects");

    // Create component patterns
    std::cout << "Creating component patterns:" << std::endl;
    WEAVE_PATTERN stability_pattern("StabilityCore");
    stability_pattern.set_metadata("category", "Stabilization");
    stability_pattern.set_metadata("description", "Provides base stability to combined patterns");

    WEAVE_PATTERN accelerator_pattern("TemporalAccelerator");
    accelerator_pattern.set_metadata("category", "Acceleration");
    accelerator_pattern.set_metadata("description", "Speeds up temporal flow in the target region");

    std::cout << "\nComponent 1: " << stability_pattern.get_name() << std::endl;
    std::cout << "Component 2: " << accelerator_pattern.get_name() << std::endl;

    // Combine patterns
    std::cout << "\nCombining patterns using different methods:" << std::endl;

    // Method 1: Sequential application (one pattern after another)
    std::cout << "1. Sequential Application (First stability, then acceleration):" << std::endl;
    WEAVE_PATTERN sequential_combined("SequentialCombined");
    sequential_combined.set_metadata("description",
                                     "Sequential application of stability and acceleration");
    sequential_combined.set_metadata("author", "Chronovyan Pattern Composer");

    // Apply stability pattern first, then accelerator pattern
    apply_pattern(stability_pattern, sequential_combined);
    apply_pattern(accelerator_pattern, sequential_combined);

    print_pattern_info(sequential_combined);

    // Method 2: Layered application
    std::cout << "\n2. Layered Application (Apply to a copy of another pattern):" << std::endl;
    // First create a copy of the accelerator
    WEAVE_PATTERN layered("LayeredPattern");
    // Then apply the stability pattern to it
    apply_pattern(accelerator_pattern, layered);
    apply_pattern(stability_pattern, layered);
    layered.set_metadata("description", "Layered stability over acceleration");
    layered.set_metadata("author", "Chronovyan Pattern Composer");
    print_pattern_info(layered);

    // Method 3: Pattern enhancement using resources
    std::cout << "\n3. Enhanced Pattern (Using Aethel infusion):" << std::endl;
    WEAVE_PATTERN buffer_pattern("TemporalBuffer");
    buffer_pattern.set_metadata("category", "Protection");
    buffer_pattern.set_metadata("description", "Buffers against temporal disruptions");

    // Enhance the buffer pattern with Aethel
    WEAVE_PATTERN enhanced_buffer =
        infuse_pattern(buffer_pattern, "Time_Crystals", "BoostStability");
    enhanced_buffer.set_metadata("description", "Enhanced temporal buffer");
    enhanced_buffer.set_metadata("author", "Chronovyan Pattern Composer");
    print_pattern_info(enhanced_buffer);

    // Apply the enhanced buffer to the combined pattern
    WEAVE_PATTERN protected_combined("ProtectedCombined");
    apply_pattern(sequential_combined, protected_combined);
    apply_pattern(enhanced_buffer, protected_combined);
    protected_combined.set_metadata("description", "Protected combined pattern");
    print_pattern_info(protected_combined);

    // Compare risks
    std::cout << "\nComparing risk levels of different pattern combinations:" << std::endl;
    std::cout << "Sequential combined risk: " << validate_pattern(sequential_combined) << std::endl;
    std::cout << "Layered pattern risk:     " << validate_pattern(layered) << std::endl;
    std::cout << "Enhanced buffer risk:     " << validate_pattern(enhanced_buffer) << std::endl;
    std::cout << "Protected combined risk:  " << validate_pattern(protected_combined) << std::endl;
}

/**
 * @brief Demonstrates pattern applications in different contexts
 */
void demonstrate_pattern_applications() {
    print_section_header("Pattern Applications", "Applying patterns to various temporal contexts");

    // Create a pattern
    WEAVE_PATTERN healing_pattern("TemporalHealing");
    healing_pattern.set_metadata("category", "Restoration");
    healing_pattern.set_metadata("description", "Repairs damaged timelines");

    std::cout << "Created " << healing_pattern.get_name() << " pattern:" << std::endl;
    print_pattern_info(healing_pattern);

    // Create different contexts
    std::vector<WEAVE_PATTERN> contexts = {WEAVE_PATTERN("StableTimeline"),
                                           WEAVE_PATTERN("DamagedTimeline"),
                                           WEAVE_PATTERN("CriticalTimeline")};

    contexts[0].set_metadata("condition", "stable");
    contexts[1].set_metadata("condition", "damaged");
    contexts[2].set_metadata("condition", "critical");

    // Apply pattern to different contexts
    std::cout << "\nApplying pattern to different temporal contexts:" << std::endl;

    for (const auto& context : contexts) {
        std::cout << "\nContext: " << context.get_name() << " ("
                  << context.get_metadata("condition") << ")" << std::endl;

        bool result = apply_pattern(healing_pattern, context);

        std::cout << "Application result: " << (result ? "Success" : "Failure") << std::endl;

        // Calculate and display the effect based on context
        if (result) {
            double effect_magnitude = 0.0;
            std::string effect_description;

            if (context.get_metadata("condition") == "stable") {
                effect_magnitude = 0.1;
                effect_description = "Minor improvements (already stable)";
            } else if (context.get_metadata("condition") == "damaged") {
                effect_magnitude = 0.6;
                effect_description = "Significant repairs completed";
            } else if (context.get_metadata("condition") == "critical") {
                effect_magnitude = 0.9;
                effect_description = "Critical timeline stabilized";
            }

            std::cout << "Healing effect: " << (effect_magnitude * 100) << "% ("
                      << effect_description << ")" << std::endl;
        }
    }
}

/**
 * @brief Main function for the Chronovyan Pattern Management Demo
 */
int main() {
    // Application header
    std::cout << std::string(60, '*') << std::endl;
    std::cout << "          Chronovyan Pattern Management Demo" << std::endl;
    std::cout << std::string(60, '*') << std::endl;

    // Initialize the standard library
    std::cout << "\nInitializing Chronovyan Standard Library..." << std::endl;
    if (!initialize()) {
        std::cerr << "ERROR: Failed to initialize the standard library. Exiting." << std::endl;
        return 1;
    }
    std::cout << "Initialization successful. Library is ready to use." << std::endl;

    // Run the demonstrations
    demonstrate_basic_patterns();
    demonstrate_pattern_composition();
    demonstrate_pattern_applications();

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