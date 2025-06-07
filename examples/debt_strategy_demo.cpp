#include <iostream>
#include <memory>
#include <string>

#include "../include/resource_manager.h"
#include "../include/temporal_debt_benchmark.h"
#include "../include/temporal_debt_tracker.h"

using namespace chronovyan;

// Utility function to print a strategy name
std::string getStrategyName(RepaymentStrategy strategy) {
    switch (strategy) {
        case RepaymentStrategy::OLDEST_FIRST:
            return "Oldest First";
        case RepaymentStrategy::HIGHEST_INTEREST:
            return "Highest Interest";
        case RepaymentStrategy::CRITICAL_FIRST:
            return "Critical First";
        case RepaymentStrategy::BALANCED:
            return "Balanced";
        case RepaymentStrategy::MINIMUM_PAYMENTS:
            return "Minimum Payments";
        case RepaymentStrategy::SNOWBALL:
            return "Snowball";
        case RepaymentStrategy::AVALANCHE:
            return "Avalanche";
        case RepaymentStrategy::STABILITY_OPTIMIZED:
            return "Stability Optimized";
        case RepaymentStrategy::CHRONOS_PRIORITY:
            return "Chronos Priority";
        case RepaymentStrategy::AETHEL_PRIORITY:
            return "Aethel Priority";
        default:
            return "Unknown Strategy";
    }
}

// Demo function to test a single strategy
void demonstrateStrategy(TemporalDebtTracker& tracker, RepaymentStrategy strategy,
                         ResourceManager& resourceManager) {
    // Show initial state
    std::cout << "==================================================\n";
    std::cout << "DEMONSTRATING STRATEGY: " << getStrategyName(strategy) << "\n";
    std::cout << "==================================================\n";

    std::cout << "Initial Resources: " << resourceManager.getChronons() << " chronons, "
              << resourceManager.getAethel() << " aethel\n";

    std::cout << "Initial Stability: " << tracker.getStability() << "\n\n";

    std::cout << "Initial Debts:\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "ID                 | Type    | Amount | Due Date      | Interest | Critical\n";
    std::cout << "--------------------------------------------------\n";

    for (const auto& debt : tracker.getAllDebts()) {
        std::cout << debt.id << " | " << (debt.debtType == DebtType::CHRONON ? "CHRONON" : "AETHEL")
                  << " | " << debt.amount << " | " << debt.dueDate.toString() << " | "
                  << debt.interestRate << " | " << (debt.isCritical ? "Yes" : "No") << "\n";
    }
    std::cout << "--------------------------------------------------\n\n";

    // Apply the strategy
    std::cout << "Applying " << getStrategyName(strategy) << " strategy...\n";
    double stabilityImprovement = tracker.applyRepaymentStrategy(strategy);

    // Show results
    std::cout << "\nStrategy applied. Stability improved by: " << stabilityImprovement << "\n";
    std::cout << "Final Stability: " << tracker.getStability() << "\n\n";

    std::cout << "Remaining Resources: " << resourceManager.getChronons() << " chronons, "
              << resourceManager.getAethel() << " aethel\n\n";

    std::cout << "Remaining Debts:\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "ID                 | Type    | Amount | Due Date      | Interest | Critical\n";
    std::cout << "--------------------------------------------------\n";

    auto remainingDebts = tracker.getAllDebts();
    if (remainingDebts.empty()) {
        std::cout << "All debts have been paid off!\n";
    } else {
        for (const auto& debt : remainingDebts) {
            std::cout << debt.id << " | "
                      << (debt.debtType == DebtType::CHRONON ? "CHRONON" : "AETHEL") << " | "
                      << debt.amount << " | " << debt.dueDate.toString() << " | "
                      << debt.interestRate << " | " << (debt.isCritical ? "Yes" : "No") << "\n";
        }
    }
    std::cout << "--------------------------------------------------\n\n";
}

// Run the benchmark on all strategies
void runBenchmarkDemo() {
    std::cout << "\n==================================================\n";
    std::cout << "RUNNING TEMPORAL DEBT REPAYMENT BENCHMARK\n";
    std::cout << "==================================================\n\n";

    auto resourceManager = std::make_shared<ResourceManager>();
    auto benchmark = TemporalDebtBenchmark(resourceManager);

    // Set up a standard scenario
    std::cout << "Setting up balanced scenario...\n";
    benchmark.setupStandardScenario("balanced");

    // Run all strategies
    std::cout << "Running all strategies...\n\n";
    auto results = benchmark.runAllStrategies();

    // Generate and display the report
    std::string report = benchmark.generateBenchmarkReport(results);
    std::cout << report << "\n";

    // Find optimal strategy
    RepaymentStrategy optimal = benchmark.findOptimalStrategy();
    std::cout << "The optimal strategy for this scenario is: " << getStrategyName(optimal)
              << "\n\n";

    // Run a few more scenarios
    std::cout << "Setting up critical-heavy scenario...\n";
    benchmark.setupStandardScenario("critical_heavy");
    optimal = benchmark.findOptimalStrategy();
    std::cout << "The optimal strategy for the critical-heavy scenario is: "
              << getStrategyName(optimal) << "\n\n";

    std::cout << "Setting up high-interest scenario...\n";
    benchmark.setupStandardScenario("high_interest");
    optimal = benchmark.findOptimalStrategy();
    std::cout << "The optimal strategy for the high-interest scenario is: "
              << getStrategyName(optimal) << "\n\n";

    std::cout << "Setting up mixed-age scenario...\n";
    benchmark.setupStandardScenario("mixed_age");
    optimal = benchmark.findOptimalStrategy();
    std::cout << "The optimal strategy for the mixed-age scenario is: " << getStrategyName(optimal)
              << "\n\n";

    std::cout << "Setting up resource-limited scenario...\n";
    benchmark.setupStandardScenario("resource_limited");
    optimal = benchmark.findOptimalStrategy();
    std::cout << "The optimal strategy for the resource-limited scenario is: "
              << getStrategyName(optimal) << "\n\n";
}

// Demo for paradox risk projection
void demonstrateParadoxRisk(TemporalDebtTracker& tracker) {
    std::cout << "\n==================================================\n";
    std::cout << "DEMONSTRATING PARADOX RISK PROJECTION\n";
    std::cout << "==================================================\n\n";

    std::cout << "Current Stability: " << tracker.getStability() << "\n\n";

    std::cout << "Projecting paradox risk for different rebel operations:\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << "Operation Type      | Cycles | Count | Risk\n";
    std::cout << "--------------------------------------------------\n";

    std::vector<RebelOperationType> operationTypes = {
        RebelOperationType::TIMELOOP, RebelOperationType::PARADOX_INSERTION,
        RebelOperationType::TIMELINE_FRACTURE, RebelOperationType::CAUSALITY_INVERSION,
        RebelOperationType::ENTROPY_MANIPULATION};

    std::vector<std::string> operationNames = {"TIMELOOP", "PARADOX_INSERTION", "TIMELINE_FRACTURE",
                                               "CAUSALITY_INVERSION", "ENTROPY_MANIPULATION"};

    for (size_t i = 0; i < operationTypes.size(); i++) {
        for (int cycles = 1; cycles <= 3; cycles++) {
            for (int count = 1; count <= 3; count++) {
                double risk = tracker.projectParadoxRisk(operationTypes[i], cycles, count);
                std::cout << operationNames[i] << " | " << cycles << " | " << count << " | " << risk
                          << "\n";
            }
        }
        std::cout << "--------------------------------------------------\n";
    }
}

int main() {
    std::cout << "===============================================\n";
    std::cout << "TEMPORAL DEBT REPAYMENT STRATEGIES DEMONSTRATION\n";
    std::cout << "===============================================\n\n";

    // Initialize the resource manager
    auto resourceManager = std::make_shared<ResourceManager>();
    resourceManager->setChronons(500.0);
    resourceManager->setAethel(500.0);

    // Initialize the debt tracker
    auto debtTracker = std::make_shared<TemporalDebtTracker>(resourceManager);

    // Create a set of test debts
    TemporalDebt debt1{.id = "chronon-small",
                       .debtType = DebtType::CHRONON,
                       .amount = 100.0,
                       .dueDate = Timestamp::now() + 10000,  // Future debt
                       .interestRate = 0.03,
                       .isCritical = false};

    TemporalDebt debt2{.id = "chronon-large",
                       .debtType = DebtType::CHRONON,
                       .amount = 300.0,
                       .dueDate = Timestamp::now() - 5000,  // Past due debt
                       .interestRate = 0.08,
                       .isCritical = true};

    TemporalDebt debt3{.id = "aethel-medium",
                       .debtType = DebtType::AETHEL,
                       .amount = 200.0,
                       .dueDate = Timestamp::now() + 15000,  // Future debt
                       .interestRate = 0.05,
                       .isCritical = false};

    TemporalDebt debt4{.id = "aethel-small",
                       .debtType = DebtType::AETHEL,
                       .amount = 75.0,
                       .dueDate = Timestamp::now() - 2000,  // Past due debt
                       .interestRate = 0.06,
                       .isCritical = true};

    // Add debts to the tracker
    debtTracker->addDebt(debt1);
    debtTracker->addDebt(debt2);
    debtTracker->addDebt(debt3);
    debtTracker->addDebt(debt4);

    // Demonstrate paradox risk projection
    demonstrateParadoxRisk(*debtTracker);

    // Demonstrate a few key strategies individually
    // Make a copy of the initial debts for each demonstration
    auto initialDebts = debtTracker->getAllDebts();
    double initialChronons = resourceManager->getChronons();
    double initialAethel = resourceManager->getAethel();

    // Test OLDEST_FIRST strategy
    demonstrateStrategy(*debtTracker, RepaymentStrategy::OLDEST_FIRST, *resourceManager);

    // Reset for next strategy
    debtTracker->clearAllDebts();
    for (const auto& debt : initialDebts) {
        debtTracker->addDebt(debt);
    }
    resourceManager->setChronons(initialChronons);
    resourceManager->setAethel(initialAethel);

    // Test CRITICAL_FIRST strategy
    demonstrateStrategy(*debtTracker, RepaymentStrategy::CRITICAL_FIRST, *resourceManager);

    // Reset for next strategy
    debtTracker->clearAllDebts();
    for (const auto& debt : initialDebts) {
        debtTracker->addDebt(debt);
    }
    resourceManager->setChronons(initialChronons);
    resourceManager->setAethel(initialAethel);

    // Test STABILITY_OPTIMIZED strategy
    demonstrateStrategy(*debtTracker, RepaymentStrategy::STABILITY_OPTIMIZED, *resourceManager);

    // Run the benchmark demo
    runBenchmarkDemo();

    std::cout << "\n===============================================\n";
    std::cout << "DEMONSTRATION COMPLETE\n";
    std::cout << "===============================================\n";

    return 0;
}