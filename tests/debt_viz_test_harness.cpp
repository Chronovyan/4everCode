#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../include/resource_optimizer.h"
#include "../include/resource_visualization.h"
#include "../include/temporal_debt_tracker.h"
#include "../include/temporal_runtime.h"

using namespace chronovyan;

// Helper function to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

// Function to set up different debt levels
void setupDebtLevel(TemporalDebtTracker& tracker, const std::string& level) {
    // Reset any existing debts
    // Since there's no clear() method, we'll just create a baseline

    if (level == "high") {
        tracker.borrowChronons(400, "test_chronon_debt_high", true);
        tracker.borrowAethel(200, "test_aethel_debt_high", true);
    } else if (level == "medium") {
        tracker.borrowChronons(200, "test_chronon_debt_medium", false);
        tracker.borrowAethel(100, "test_aethel_debt_medium", false);
    } else if (level == "low") {
        tracker.borrowChronons(50, "test_chronon_debt_low", false);
        tracker.borrowAethel(20, "test_aethel_debt_low", false);
    } else if (level == "very_low") {
        tracker.borrowChronons(10, "test_chronon_debt_very_low", false);
        tracker.borrowAethel(5, "test_aethel_debt_very_low", false);
    }
}

int main() {
    std::cout << "Starting Debt Visualization Test Harness" << std::endl;

    // Initialize core components
    auto runtime = std::make_shared<TemporalRuntime>();
    runtime->replenishChronons(1000);
    std::cout << "Replenished 1000 chronons. New level: " << runtime->getChrononsLevel()
              << std::endl;

    runtime->replenishAethel(1000);
    std::cout << "Replenished 1000 aethel. New level: " << runtime->getAethelLevel() << std::endl;

    auto debtTracker = std::make_shared<TemporalDebtTracker>(runtime);
    auto optimizer = std::make_shared<ResourceOptimizer>(runtime, debtTracker);

    std::cout << "Creating test debts..." << std::endl;

    // Create some test debts
    setupDebtLevel(*debtTracker, "high");

    // Create visualization
    ResourceVisualization visualization(runtime, debtTracker, optimizer, nullptr);

    std::cout << "Testing visualizeDebtImpact..." << std::endl;

    // Get the debt impact visualization
    std::string output = visualization.visualizeDebtImpact(10, VisualizationFormat::TEXT);

    std::cout << "\n\n---- VISUALIZATION OUTPUT ----\n\n" << output << "\n\n";

    // Check for the required sections
    std::vector<std::string> requiredSections = {
        "TEMPORAL DEBT IMPACT ANALYSIS", "CURRENT STATUS",  "DEBT PROJECTION",
        "STABILITY PROJECTION",          "IMPACT ANALYSIS", "RECOMMENDATION"};

    std::cout << "---- VERIFICATION ----\n\n";
    bool allPassed = true;

    for (const auto& section : requiredSections) {
        bool found = contains(output, section);
        std::cout << (found ? "PASSED: " : "FAILED: ") << "Section '" << section << "' "
                  << (found ? "found" : "not found") << std::endl;
        allPassed = allPassed && found;
    }

    if (allPassed) {
        std::cout << "\nALL CHECKS PASSED!" << std::endl;
        return 0;
    } else {
        std::cout << "\nSOME CHECKS FAILED!" << std::endl;
        return 1;
    }
}