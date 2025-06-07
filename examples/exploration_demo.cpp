/**
 * @file exploration_demo.cpp
 * @brief Demonstration of the Exploration Management module
 *
 * // Chronovyan Example: Temporal Exploration Demonstration
 * // Difficulty: Intermediate
 * // Concepts: Temporal Signatures, Exploration Teams, Signature Analysis, Loot Acquisition
 *
 * This example demonstrates the use of the Exploration Management module
 * in the Chronovyan Standard Library. It showcases how to explore temporal regions,
 * analyze signatures, manage exploration teams, and acquire resources from
 * different timelines.
 *
 * @copyright Chronovyan Project
 */

#include <iomanip>
#include <iostream>
#include <map>
#include <string>

#include "stdlib/chronovyan_stdlib.h"
#include "stdlib/exploration.h"
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
 * @brief Prints formatted information about a temporal signature
 * @param label Descriptive label for the signature
 * @param signature The temporal signature to display
 */
void print_signature_info(const std::string& label, const TEMPORAL_SIGNATURE& signature) {
    std::cout << std::left << std::setw(20) << label << ": " << signature << std::endl;

    // Additional signature information could be displayed here
    // For example, signature length, complexity, etc.
    std::cout << std::left << std::setw(20) << "Signature Length" << ": " << signature.length()
              << " units" << std::endl;
}

/**
 * @brief Main function for the Chronovyan Exploration Management Demo
 */
int main() {
    // Application header
    std::cout << std::string(60, '*') << std::endl;
    std::cout << "          Chronovyan Exploration Management Demo" << std::endl;
    std::cout << std::string(60, '*') << std::endl;

    // Initialize the Chronovyan Standard Library
    std::cout << "\nInitializing Chronovyan Standard Library..." << std::endl;
    if (!initialize()) {
        std::cerr << "ERROR: Failed to initialize the standard library. Exiting." << std::endl;
        return 1;
    }
    std::cout << "Initialization successful. Library is ready to use." << std::endl;

    // Display initial resource state
    print_section_header("Resource Initialization", "Preparing Aethel reserves for exploration");
    AETHEL_RESERVE aethel_reserve = 1000;  // Start with 1000 Aethel units
    std::cout << "Initial Aethel reserve: " << aethel_reserve << " units" << std::endl;

    // If initial Aethel is low, replenish some for the demo
    if (aethel_reserve < 500) {
        std::cout << "Replenishing Aethel for exploration..." << std::endl;
        int amount_to_add = 1000 - aethel_reserve;
        replenish_aethel(aethel_reserve, amount_to_add);
        std::cout << "Added " << amount_to_add << " Aethel units" << std::endl;
        std::cout << "Current Aethel reserve: " << aethel_reserve << " units" << std::endl;
    }

    // Basic exploration of a stable timeline
    print_section_header("Basic Exploration", "Exploring a stable temporal region");
    std::cout << "Initiating exploration of Alpha_Timeline (stable region)..." << std::endl;
    // Simulate exploration cost (50 Aethel units)
    const int EXPLORATION_COST = 50;
    if (aethel_reserve >= EXPLORATION_COST) {
        consume_aethel(aethel_reserve, EXPLORATION_COST);
        TEMPORAL_SIGNATURE signature1 = "Alpha_Timeline_Signature";

        print_signature_info("Alpha Signature", signature1);
        std::cout << "Exploration cost: " << EXPLORATION_COST << " Aethel units" << std::endl;
        std::cout << "Remaining Aethel: " << aethel_reserve << " units" << std::endl;
    } else {
        std::cerr << "Error: Not enough Aethel for exploration (need " << EXPLORATION_COST
                  << " units)" << std::endl;
    }

    // Explore an unstable region for comparison
    print_section_header("Advanced Exploration", "Exploring an unstable temporal region");
    std::cout << "WARNING: Temporal_Rift exploration carries increased instability risk"
              << std::endl;
    std::cout << "Initiating exploration of Temporal_Rift (unstable region)..." << std::endl;

    // Simulate exploration of unstable region (higher cost)
    const int UNSTABLE_COST = 100;
    if (aethel_reserve >= UNSTABLE_COST) {
        consume_aethel(aethel_reserve, UNSTABLE_COST);
        TEMPORAL_SIGNATURE signature2 = "Temporal_Rift_Signature";

        print_signature_info("Rift Signature", signature2);
        std::cout << "Exploration cost: " << UNSTABLE_COST << " Aethel units" << std::endl;
        std::cout << "Remaining Aethel: " << aethel_reserve << " units" << std::endl;
    } else {
        std::cerr << "Error: Not enough Aethel for unstable region exploration (need "
                  << UNSTABLE_COST << " units)" << std::endl;
    }

    // Compare the two signatures
    std::cout << "\nComparing signatures from different regions:" << std::endl;

    // Get the signatures from the exploration results
    TEMPORAL_SIGNATURE signature1 = "Alpha_Timeline_Signature";
    TEMPORAL_SIGNATURE signature2 = "Temporal_Rift_Signature";

    std::cout << "  Signature 1: " << signature1 << std::endl;
    std::cout << "  Signature 2: " << signature2 << std::endl;

    if (signature1 == signature2) {
        std::cout << "  Unexpected result: Signatures are identical!" << std::endl;
        std::cout << "  This suggests timeline convergence or measurement error." << std::endl;
    } else {
        std::cout << "  Signatures differ as expected for distinct temporal regions." << std::endl;
        std::cout << "  This confirms the uniqueness of each timeline." << std::endl;
    }

    // Signature analysis
    print_section_header("Signature Analysis", "Extracting properties from temporal signatures");
    Explorer explorer;
    std::cout << "Analyzing temporal signature from Alpha_Timeline..." << std::endl;
    auto properties = explorer.analyze_signature(signature1);

    std::cout << "\nProperties of Alpha_Timeline signature:" << std::endl;
    std::cout << std::left << std::setw(20) << "Property" << "| " << "Value" << std::endl;
    std::cout << std::string(40, '-') << std::endl;
    for (const auto& prop : properties) {
        std::cout << std::left << std::setw(20) << prop.first << "| " << prop.second << std::endl;
    }

    // Create and manage exploration teams
    print_section_header("Exploration Team Management", "Creating and deploying specialized teams");

    // Create a team
    std::cout << "Creating exploration team 'Chronovyan_Seekers'..." << std::endl;
    bool team_created = explorer.create_team("Chronovyan_Seekers", 5);

    if (team_created) {
        std::cout << "Team created successfully with 5 experienced explorers" << std::endl;
        std::cout << "Team specialization: Temporal anomaly detection" << std::endl;

        // Send the team on a mission
        std::cout << "\nDeploying team to Temporal_Rift for deep exploration..." << std::endl;
        TEMPORAL_SIGNATURE mission_signature =
            explorer.send_team("Chronovyan_Seekers", "Temporal_Rift", 10);

        if (!mission_signature.empty()) {
            print_signature_info("Mission Signature", mission_signature);
            std::cout << "Team deployed successfully with mission duration: 10 temporal units"
                      << std::endl;
            std::cout << "Expected return: After completion of temporal stabilization" << std::endl;
        } else {
            std::cout
                << "ERROR: Failed to deploy team - insufficient resources or invalid parameters"
                << std::endl;
        }
    } else {
        std::cout << "ERROR: Failed to create exploration team - insufficient resources"
                  << std::endl;
    }

    // Resource acquisition through exploration
    print_section_header("Resource Acquisition", "Obtaining resources from temporal exploration");

    // Create a loot table with probabilities
    LOOT_TABLE loot_table = {{0.7, LOOT("Common_Chronon_Fragment")},
                             {0.9, LOOT("Rare_Aethel_Crystal")},
                             {1.0, LOOT("Legendary_Temporal_Relic")}};

    // Display the loot table
    std::cout << "Probability-based loot acquisition system:" << std::endl;
    std::cout << std::left << std::setw(10) << "Chance" << "| " << "Item" << std::endl;
    std::cout << std::string(50, '-') << std::endl;
    std::cout << std::left << std::setw(10) << "0-70%" << "| " << "Common_Chronon_Fragment"
              << std::endl;
    std::cout << std::left << std::setw(10) << "71-90%" << "| " << "Rare_Aethel_Crystal"
              << std::endl;
    std::cout << std::left << std::setw(10) << "91-100%" << "| " << "Legendary_Temporal_Relic"
              << std::endl;

    // Perform multiple loot drops to demonstrate probabilities
    std::cout << "\nPerforming 5 sample loot acquisitions:" << std::endl;
    for (int i = 1; i <= 5; i++) {
        CHRONON_STREAM reward = loot_drop(loot_table);
        std::cout << "  Roll #" << i << ": Acquired " << reward << std::endl;
    }

    // Final resource summary
    print_section_header("Exploration Summary", "Final resource status and exploration results");
    AETHEL_RESERVE final_aethel = aethel_reserve;
    AETHEL_RESERVE initial_aethel = 1000;  // Initial value we started with
    AETHEL_RESERVE total_consumed = initial_aethel - final_aethel;

    std::cout << "Initial Aethel reserve: " << initial_aethel << " units" << std::endl;
    std::cout << "Final Aethel reserve:   " << final_aethel << " units" << std::endl;
    std::cout << "Total Aethel consumed:  " << total_consumed << " units" << std::endl;

    // Provide a summary of exploration achievements
    std::cout << "\nExploration achievements:" << std::endl;
    std::cout << "- Explored 2 distinct temporal regions" << std::endl;
    std::cout << "- Acquired temporal signature data from both regions" << std::endl;
    std::cout << "- Deployed 1 exploration team on a deep exploration mission" << std::endl;
    std::cout << "- Acquired multiple resources through loot drops" << std::endl;

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