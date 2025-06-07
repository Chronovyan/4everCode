/**
 * @file stdlib_demo.cpp
 * @brief Example program demonstrating usage of the Chronovyan Standard Library
 *
 * // Chronovyan Example: Temporal Manipulation Demonstration
 * // Difficulty: Beginner
 * // Concepts: Temporal Operations, Branch Management, Chronovyan Initialization
 *
 * This program shows how to initialize the standard library, use core temporal
 * manipulation functions, manage timeline branches, and properly shut down the library.
 * It demonstrates both basic operations and more advanced features through the
 * TemporalManipulator interface.
 *
 * @copyright Chronovyan Project
 */

#include <iomanip>
#include <iostream>
#include <string>

#include "stdlib/chronovyan_stdlib.h"
#include "stdlib/temporal_manipulation.h"

using namespace chronovyan::stdlib;

/**
 * @brief Prints a section header to improve output readability
 * @param title The title of the section
 */
void print_section_header(const std::string& title) {
    std::cout << "\n" << std::string(60, '=') << std::endl;
    std::cout << "  " << title << std::endl;
    std::cout << std::string(60, '=') << std::endl;
}

/**
 * @brief Prints the result of an operation
 * @param operation The name of the operation
 * @param result The result (true for success, false for failure)
 * @param details Additional details about the operation (optional)
 */
void print_result(const std::string& operation, bool result, const std::string& details = "") {
    std::cout << "  " << std::left << std::setw(30) << operation << ": "
              << (result ? "SUCCESS" : "FAILURE");
    if (!details.empty()) {
        std::cout << " (" << details << ")";
    }
    std::cout << std::endl;
}

/**
 * @brief Demonstrates basic temporal manipulation functions
 *
 * This function shows how to use the fundamental temporal operations:
 * - Rewinding to specific anchors
 * - Fast forwarding by a number of ticks
 * - Handling invalid operations gracefully
 */
void demonstrate_basic_temporal_operations() {
    print_section_header("Basic Temporal Operations");

    // Demonstrate rewind_to function
    // The @Origin anchor is a special anchor that represents the beginning of the timeline
    std::cout << "Rewinding to timeline anchors:" << std::endl;
    bool rewind_result = rewind_to("@Origin");
    print_result("Rewind to @Origin", rewind_result);

    // Demonstrate fast_forward function
    // Fast forwarding advances the timeline by a specified number of ticks
    std::cout << "\nAdvancing the timeline:" << std::endl;
    bool ff_result = fast_forward(10);
    print_result("Fast forward 10 ticks", ff_result);

    // Demonstrate invalid inputs and error handling
    std::cout << "\nError handling with invalid operations:" << std::endl;
    bool invalid_rewind = rewind_to("@NonexistentAnchor");
    print_result("Rewind to nonexistent anchor", invalid_rewind, "Expected to fail");

    bool invalid_ff = fast_forward(-5);
    print_result("Fast forward negative ticks", invalid_ff, "Expected to fail");
}

/**
 * @brief Demonstrates advanced temporal manipulation with the TemporalManipulator class
 *
 * This function shows how to use the TemporalManipulator for more complex operations:
 * - Creating timeline branches for parallel development
 * - Merging branches to consolidate changes
 * - Error handling for branch operations
 */
void demonstrate_advanced_temporal_manipulation() {
    print_section_header("Advanced Temporal Manipulation");

    // Create a temporal manipulator instance
    // This provides a higher-level interface for managing timeline branches
    TemporalManipulator manipulator;

    // Branch management
    std::cout << "Timeline Branch Management:" << std::endl;

    // Create branches for parallel development
    bool branch_result = manipulator.create_branch("alternative_timeline");
    print_result("Create 'alternative_timeline'", branch_result);

    bool branch2_result = manipulator.create_branch("experiment_42");
    print_result("Create 'experiment_42'", branch2_result);

    // Merge operations
    std::cout << "\nBranch Merging:" << std::endl;
    bool merge_result = manipulator.merge_branch("alternative_timeline");
    print_result("Merge 'alternative_timeline'", merge_result);

    // Error handling with merges
    bool invalid_merge = manipulator.merge_branch("nonexistent_branch");
    print_result("Merge nonexistent branch", invalid_merge, "Expected to fail");
}

/**
 * @brief Main function that demonstrates the Chronovyan Standard Library
 */
int main() {
    // Application header
    std::cout << std::string(60, '*') << std::endl;
    std::cout << "          Chronovyan Standard Library Demo" << std::endl;
    std::cout << std::string(60, '*') << std::endl;

    // Initialize the standard library
    // This is required before using any Chronovyan functions
    std::cout << "\nInitializing Chronovyan Standard Library..." << std::endl;
    if (!initialize()) {
        std::cerr << "ERROR: Failed to initialize the standard library. Exiting." << std::endl;
        return 1;
    }
    std::cout << "Initialization successful. Library is ready to use." << std::endl;

    // Run the demonstrations
    demonstrate_basic_temporal_operations();
    demonstrate_advanced_temporal_manipulation();

    // Shutdown the standard library
    // This ensures all resources are properly released
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