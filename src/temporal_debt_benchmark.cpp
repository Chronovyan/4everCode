#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "../include/temporal_debt_benchmark.h"

namespace chronovyan {

TemporalDebtBenchmark::TemporalDebtBenchmark(std::shared_ptr<ResourceManager> resourceManager)
    : m_resourceManager(resourceManager)
    , m_debtTracker(std::make_shared<TemporalDebtTracker>(resourceManager->getTemporalRuntime())) {}

bool TemporalDebtBenchmark::setupStandardScenario(const std::string& scenario) {
    clearDebts();

    if (scenario == "balanced") {
        setupBalancedScenario();
        return true;
    } else if (scenario == "critical") {
        setupCriticalHeavyScenario();
        return true;
    } else if (scenario == "high_interest") {
        setupHighInterestScenario();
        return true;
    } else if (scenario == "mixed_age") {
        setupMixedAgeScenario();
        return true;
    } else if (scenario == "resource_limited") {
        setupResourceLimitedScenario();
        return true;
    }

    return false;
}

void TemporalDebtBenchmark::addDebt(const TemporalDebt& debt) {
    m_debtTracker->addDebtForTesting(debt.id, debt.chronon_component, debt.aethel_component,
                                     debt.interest_rate, debt.age_in_cycles, debt.is_critical,
                                     debt.type, 1);
}

void TemporalDebtBenchmark::clearDebts() {
    // Clear all debts from the tracker
    auto debts = m_debtTracker->getAllDebts();
    for (const auto& debt : debts) {
        m_debtTracker->repayChronons(debt.chronon_debt, debt.operation_id);
        m_debtTracker->repayAethel(debt.aethel_debt, debt.operation_id);
    }
}

void TemporalDebtBenchmark::setAvailableResources(double chronons, double aethel) {
    m_resourceManager->setChronons(chronons);
    m_resourceManager->setAethel(aethel);
}

TemporalDebtBenchmark::BenchmarkResult TemporalDebtBenchmark::runStrategyBenchmark(
    RepaymentStrategyType strategy) {
    BenchmarkResult result;
    result.strategy = strategy;

    // Record initial state
    result.initialStability = m_debtTracker->getStability();
    double initialChronons = m_resourceManager->getChronons();
    double initialAethel = m_resourceManager->getAethel();
    int initialDebtCount = m_debtTracker->getAllDebts().size();

    // Execute the strategy with timing
    auto startTime = std::chrono::high_resolution_clock::now();

    RepaymentResult repaymentResult =
        m_debtTracker->applyRepaymentStrategy(strategy, initialChronons, initialAethel);

    auto endTime = std::chrono::high_resolution_clock::now();
    result.executionTime =
        std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    // Record final state
    result.finalStability = m_debtTracker->getStability();
    result.stabilityImprovement = result.finalStability - result.initialStability;
    result.resourcesUsed = repaymentResult.chronons_repaid + repaymentResult.aethel_repaid;

    if (result.resourcesUsed > 0) {
        result.stabilityPerResource = result.stabilityImprovement / result.resourcesUsed;
    } else {
        result.stabilityPerResource = 0.0;
    }

    // Count debts paid
    int remainingDebtCount = m_debtTracker->getAllDebts().size();
    result.debtsFullyPaid = initialDebtCount - remainingDebtCount;

    // Count partially paid debts (those with reduced but not eliminated debt)
    int partiallyPaid = 0;
    for (const auto& debt : m_debtTracker->getAllDebts()) {
        if (debt.chronon_debt < initialChronons || debt.aethel_debt < initialAethel) {
            partiallyPaid++;
        }
    }
    result.debtsPartiallyPaid = partiallyPaid;
    result.debtsUnpaid = remainingDebtCount - partiallyPaid;

    return result;
}

std::vector<TemporalDebtBenchmark::BenchmarkResult> TemporalDebtBenchmark::runAllStrategies() {
    std::vector<BenchmarkResult> results;

    // Save initial state
    double initialChronons = m_resourceManager->getChronons();
    double initialAethel = m_resourceManager->getAethel();
    auto initialDebts = m_debtTracker->getAllDebts();

    // Test each strategy
    std::vector<RepaymentStrategyType> strategies = {
        RepaymentStrategyType::OLDEST_FIRST,     RepaymentStrategyType::HIGHEST_INTEREST,
        RepaymentStrategyType::CRITICAL_FIRST,   RepaymentStrategyType::BALANCED,
        RepaymentStrategyType::MINIMUM_PAYMENTS, RepaymentStrategyType::SNOWBALL,
        RepaymentStrategyType::AVALANCHE,        RepaymentStrategyType::STABILITY_OPTIMIZED,
        RepaymentStrategyType::CHRONON_PRIORITY, RepaymentStrategyType::AETHEL_PRIORITY};

    for (const auto& strategy : strategies) {
        // Reset state before each test
        clearDebts();
        for (const auto& debt : initialDebts) {
            addDebt(debt);
        }
        m_resourceManager->setChronons(initialChronons);
        m_resourceManager->setAethel(initialAethel);

        // Run the benchmark
        results.push_back(runStrategyBenchmark(strategy));
    }

    // Restore initial state
    clearDebts();
    for (const auto& debt : initialDebts) {
        addDebt(debt);
    }
    m_resourceManager->setChronons(initialChronons);
    m_resourceManager->setAethel(initialAethel);

    return results;
}

double TemporalDebtBenchmark::compareStrategies(RepaymentStrategyType strategy1,
                                                RepaymentStrategyType strategy2) {
    // Save initial state
    double initialChronons = m_resourceManager->getChronons();
    double initialAethel = m_resourceManager->getAethel();
    auto initialDebts = m_debtTracker->getAllDebts();

    // Run first strategy
    clearDebts();
    for (const auto& debt : initialDebts) {
        addDebt(debt);
    }
    m_resourceManager->setChronons(initialChronons);
    m_resourceManager->setAethel(initialAethel);
    auto result1 = runStrategyBenchmark(strategy1);

    // Run second strategy
    clearDebts();
    for (const auto& debt : initialDebts) {
        addDebt(debt);
    }
    m_resourceManager->setChronons(initialChronons);
    m_resourceManager->setAethel(initialAethel);
    auto result2 = runStrategyBenchmark(strategy2);

    // Restore initial state
    clearDebts();
    for (const auto& debt : initialDebts) {
        addDebt(debt);
    }
    m_resourceManager->setChronons(initialChronons);
    m_resourceManager->setAethel(initialAethel);

    // Compare based on weighted factors:
    // - 50% stability improvement
    // - 30% stability per resource (efficiency)
    // - 20% debts fully paid

    double score1 = (0.5 * result1.stabilityImprovement) + (0.3 * result1.stabilityPerResource) +
                    (0.2 * result1.debtsFullyPaid);

    double score2 = (0.5 * result2.stabilityImprovement) + (0.3 * result2.stabilityPerResource) +
                    (0.2 * result2.debtsFullyPaid);

    return score1 - score2;
}

RepaymentStrategyType TemporalDebtBenchmark::findOptimalStrategy() {
    auto results = runAllStrategies();

    // Find the strategy with the highest weighted score
    RepaymentStrategyType optimalStrategy = RepaymentStrategyType::BALANCED;  // Default
    double highestScore = -1.0;

    for (const auto& result : results) {
        double score = (0.5 * result.stabilityImprovement) + (0.3 * result.stabilityPerResource) +
                       (0.2 * result.debtsFullyPaid);

        if (score > highestScore) {
            highestScore = score;
            optimalStrategy = result.strategy;
        }
    }

    return optimalStrategy;
}

std::string TemporalDebtBenchmark::formatBenchmarkResult(const BenchmarkResult& result) {
    std::stringstream ss;

    // Convert strategy enum to string
    std::string strategyName;
    switch (result.strategy) {
        case RepaymentStrategyType::OLDEST_FIRST:
            strategyName = "Oldest First";
            break;
        case RepaymentStrategyType::HIGHEST_INTEREST:
            strategyName = "Highest Interest";
            break;
        case RepaymentStrategyType::CRITICAL_FIRST:
            strategyName = "Critical First";
            break;
        case RepaymentStrategyType::BALANCED:
            strategyName = "Balanced";
            break;
        case RepaymentStrategyType::MINIMUM_PAYMENTS:
            strategyName = "Minimum Payments";
            break;
        case RepaymentStrategyType::SNOWBALL:
            strategyName = "Snowball";
            break;
        case RepaymentStrategyType::AVALANCHE:
            strategyName = "Avalanche";
            break;
        case RepaymentStrategyType::STABILITY_OPTIMIZED:
            strategyName = "Stability Optimized";
            break;
        case RepaymentStrategyType::CHRONON_PRIORITY:
            strategyName = "Chronon Priority";
            break;
        case RepaymentStrategyType::AETHEL_PRIORITY:
            strategyName = "Aethel Priority";
            break;
        default:
            strategyName = "Unknown Strategy";
            break;
    }

    ss << "Strategy: " << strategyName << "\n";
    ss << "Stability: " << std::fixed << std::setprecision(2) << result.initialStability << " → "
       << result.finalStability << " (+" << result.stabilityImprovement << ")\n";
    ss << "Resources Used: " << result.resourcesUsed << "\n";
    ss << "Efficiency: " << result.stabilityPerResource << " stability per resource\n";
    ss << "Execution Time: " << result.executionTime.count() << " μs\n";
    ss << "Debts: " << result.debtsFullyPaid << " fully paid, " << result.debtsPartiallyPaid
       << " partially paid, " << result.debtsUnpaid << " unpaid\n";

    return ss.str();
}

std::string TemporalDebtBenchmark::generateBenchmarkReport(
    const std::vector<BenchmarkResult>& results) {
    std::stringstream report;

    report << "===============================================\n";
    report << "       TEMPORAL DEBT REPAYMENT BENCHMARK       \n";
    report << "===============================================\n\n";

    // Sort results by stability improvement (descending)
    auto sortedResults = results;
    std::sort(sortedResults.begin(), sortedResults.end(),
              [](const BenchmarkResult& a, const BenchmarkResult& b) {
                  return a.stabilityImprovement > b.stabilityImprovement;
              });

    // Report summary table
    report << "SUMMARY (sorted by stability improvement):\n";
    report << "-----------------------------------------------\n";
    report << std::left << std::setw(25) << "Strategy" << std::right << std::setw(15)
           << "Stability +" << std::setw(15) << "Efficiency" << std::setw(10) << "Paid Off" << "\n";
    report << "-----------------------------------------------\n";

    for (const auto& result : sortedResults) {
        std::string strategyName;
        switch (result.strategy) {
            case RepaymentStrategyType::OLDEST_FIRST:
                strategyName = "Oldest First";
                break;
            case RepaymentStrategyType::HIGHEST_INTEREST:
                strategyName = "Highest Interest";
                break;
            case RepaymentStrategyType::CRITICAL_FIRST:
                strategyName = "Critical First";
                break;
            case RepaymentStrategyType::BALANCED:
                strategyName = "Balanced";
                break;
            case RepaymentStrategyType::MINIMUM_PAYMENTS:
                strategyName = "Minimum Payments";
                break;
            case RepaymentStrategyType::SNOWBALL:
                strategyName = "Snowball";
                break;
            case RepaymentStrategyType::AVALANCHE:
                strategyName = "Avalanche";
                break;
            case RepaymentStrategyType::STABILITY_OPTIMIZED:
                strategyName = "Stability Optimized";
                break;
            case RepaymentStrategyType::CHRONON_PRIORITY:
                strategyName = "Chronon Priority";
                break;
            case RepaymentStrategyType::AETHEL_PRIORITY:
                strategyName = "Aethel Priority";
                break;
            default:
                strategyName = "Unknown Strategy";
                break;
        }

        report << std::left << std::setw(25) << strategyName << std::right << std::fixed
               << std::setprecision(2) << std::setw(15) << result.stabilityImprovement
               << std::setw(15) << result.stabilityPerResource << std::setw(10)
               << result.debtsFullyPaid << "\n";
    }

    report << "-----------------------------------------------\n\n";

    // Detailed results
    report << "DETAILED RESULTS:\n";
    report << "===============================================\n\n";

    for (const auto& result : sortedResults) {
        report << formatBenchmarkResult(result) << "\n";
        report << "-----------------------------------------------\n\n";
    }

    // Find and report the optimal strategy
    RepaymentStrategyType optimalStrategy = RepaymentStrategyType::BALANCED;  // Default
    double highestScore = -1.0;

    for (const auto& result : results) {
        double score = (0.5 * result.stabilityImprovement) + (0.3 * result.stabilityPerResource) +
                       (0.2 * result.debtsFullyPaid);

        if (score > highestScore) {
            highestScore = score;
            optimalStrategy = result.strategy;
        }
    }

    std::string optimalStrategyName;
    switch (optimalStrategy) {
        case RepaymentStrategyType::OLDEST_FIRST:
            optimalStrategyName = "Oldest First";
            break;
        case RepaymentStrategyType::HIGHEST_INTEREST:
            optimalStrategyName = "Highest Interest";
            break;
        case RepaymentStrategyType::CRITICAL_FIRST:
            optimalStrategyName = "Critical First";
            break;
        case RepaymentStrategyType::BALANCED:
            optimalStrategyName = "Balanced";
            break;
        case RepaymentStrategyType::MINIMUM_PAYMENTS:
            optimalStrategyName = "Minimum Payments";
            break;
        case RepaymentStrategyType::SNOWBALL:
            optimalStrategyName = "Snowball";
            break;
        case RepaymentStrategyType::AVALANCHE:
            optimalStrategyName = "Avalanche";
            break;
        case RepaymentStrategyType::STABILITY_OPTIMIZED:
            optimalStrategyName = "Stability Optimized";
            break;
        case RepaymentStrategyType::CHRONON_PRIORITY:
            optimalStrategyName = "Chronon Priority";
            break;
        case RepaymentStrategyType::AETHEL_PRIORITY:
            optimalStrategyName = "Aethel Priority";
            break;
        default:
            optimalStrategyName = "Unknown Strategy";
            break;
    }

    report << "OPTIMAL STRATEGY: " << optimalStrategyName << "\n";
    report << "===============================================\n";

    return report.str();
}

void TemporalDebtBenchmark::setupBalancedScenario() {
    // Mix of different debts with varied characteristics
    addDebt(TemporalDebt("op1", 100.0, 80.0, 20.0, 0.05, 5, false));
    addDebt(TemporalDebt("op2", 200.0, 150.0, 50.0, 0.07, 3, true));
    addDebt(TemporalDebt("op3", 150.0, 100.0, 50.0, 0.03, 8, false));
    addDebt(TemporalDebt("op4", 300.0, 200.0, 100.0, 0.09, 2, true));
    addDebt(TemporalDebt("op5", 250.0, 120.0, 130.0, 0.06, 4, false));

    // Set moderate resource availability
    m_resourceManager->setChronons(400.0);
    m_resourceManager->setAethel(200.0);
}

void TemporalDebtBenchmark::setupCriticalHeavyScenario() {
    // More critical debts with higher interest rates
    addDebt(TemporalDebt("op1", 150.0, 100.0, 50.0, 0.08, 3, true));
    addDebt(TemporalDebt("op2", 200.0, 150.0, 50.0, 0.09, 2, true));
    addDebt(TemporalDebt("op3", 250.0, 200.0, 50.0, 0.07, 4, true));
    addDebt(TemporalDebt("op4", 100.0, 80.0, 20.0, 0.04, 7, false));
    addDebt(TemporalDebt("op5", 120.0, 90.0, 30.0, 0.05, 6, false));

    // Set moderate resource availability
    m_resourceManager->setChronons(350.0);
    m_resourceManager->setAethel(120.0);
}

void TemporalDebtBenchmark::setupHighInterestScenario() {
    // Debts with varying interest rates
    addDebt(TemporalDebt("op1", 100.0, 80.0, 20.0, 0.03, 6, false));
    addDebt(TemporalDebt("op2", 150.0, 100.0, 50.0, 0.12, 4, false));
    addDebt(TemporalDebt("op3", 200.0, 150.0, 50.0, 0.08, 5, true));
    addDebt(TemporalDebt("op4", 120.0, 90.0, 30.0, 0.15, 3, false));
    addDebt(TemporalDebt("op5", 180.0, 120.0, 60.0, 0.06, 7, true));

    // Set moderate resource availability
    m_resourceManager->setChronons(300.0);
    m_resourceManager->setAethel(150.0);
}

void TemporalDebtBenchmark::setupMixedAgeScenario() {
    // Debts with varying ages
    addDebt(TemporalDebt("op1", 100.0, 80.0, 20.0, 0.05, 10, false));
    addDebt(TemporalDebt("op2", 150.0, 100.0, 50.0, 0.07, 7, false));
    addDebt(TemporalDebt("op3", 200.0, 150.0, 50.0, 0.06, 4, false));
    addDebt(TemporalDebt("op4", 120.0, 90.0, 30.0, 0.08, 2, true));
    addDebt(TemporalDebt("op5", 180.0, 120.0, 60.0, 0.09, 1, true));

    // Set moderate resource availability
    m_resourceManager->setChronons(320.0);
    m_resourceManager->setAethel(130.0);
}

void TemporalDebtBenchmark::setupResourceLimitedScenario() {
    // More debts than can be fully repaid with available resources
    addDebt(TemporalDebt("op1", 150.0, 100.0, 50.0, 0.06, 5, false));
    addDebt(TemporalDebt("op2", 200.0, 150.0, 50.0, 0.08, 4, true));
    addDebt(TemporalDebt("op3", 250.0, 200.0, 50.0, 0.05, 6, false));
    addDebt(TemporalDebt("op4", 300.0, 200.0, 100.0, 0.09, 3, true));
    addDebt(TemporalDebt("op5", 180.0, 120.0, 60.0, 0.07, 5, false));
    addDebt(TemporalDebt("op6", 220.0, 160.0, 60.0, 0.08, 4, true));
    addDebt(TemporalDebt("op7", 270.0, 180.0, 90.0, 0.06, 5, false));

    // Set limited resource availability
    m_resourceManager->setChronons(500.0);
    m_resourceManager->setAethel(200.0);
}

}  // namespace chronovyan