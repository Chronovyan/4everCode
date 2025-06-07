#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "advanced_optimization_algorithms.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

using namespace chronovyan;

// Helper function to simulate resource usage patterns
void simulateOperationWithPattern(const std::shared_ptr<TemporalRuntime>& runtime,
                                  const std::string& operation_id, const std::string& pattern,
                                  int num_executions, double base_chronons, double base_aethel) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> noise(1.0, 0.1);  // Mean 1.0, stddev 0.1

    std::cout << "Simulating operation '" << operation_id << "' with " << pattern << " pattern..."
              << std::endl;

    for (int i = 0; i < num_executions; ++i) {
        double chronon_usage = base_chronons;
        double aethel_usage = base_aethel;

        if (pattern == "repetitive") {
            // Consistent usage with small random noise
            chronon_usage *= noise(gen);
            aethel_usage *= noise(gen);
        } else if (pattern == "increasing") {
            // Linear increase over time
            double factor = 1.0 + (0.1 * i / num_executions);
            chronon_usage *= factor * noise(gen);
            aethel_usage *= factor * noise(gen);
        } else if (pattern == "decreasing") {
            // Linear decrease over time
            double factor = 1.0 - (0.3 * i / num_executions);
            chronon_usage *= factor * noise(gen);
            aethel_usage *= factor * noise(gen);
        } else if (pattern == "cyclic") {
            // Sinusoidal pattern
            double cycle_factor = 1.0 + 0.5 * std::sin(2 * M_PI * i / (num_executions / 4.0));
            chronon_usage *= cycle_factor * noise(gen);
            aethel_usage *= cycle_factor * noise(gen);
        } else if (pattern == "spike") {
            // Occasional spikes
            if (i % (num_executions / 3) == 0) {
                chronon_usage *= 3.0 * noise(gen);
                aethel_usage *= 2.5 * noise(gen);
            } else {
                chronon_usage *= noise(gen);
                aethel_usage *= noise(gen);
            }
        } else {
            // Random pattern
            chronon_usage *= std::uniform_real_distribution<double>(0.5, 1.5)(gen);
            aethel_usage *= std::uniform_real_distribution<double>(0.5, 1.5)(gen);
        }

        // Record resource usage
        runtime->recordResourceUsage(operation_id, chronon_usage, aethel_usage);

        // Sleep a bit to simulate time passing
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Simulation complete for '" << operation_id << "'" << std::endl;
}

// Helper function to print optimization results
void printOptimizationResults(const std::string& title,
                              const std::map<OptimizationStrategy, double>& results) {
    std::cout << "\n" << title << "\n";
    std::cout << std::string(title.length(), '=') << "\n";

    for (const auto& [strategy, improvement] : results) {
        std::string strategy_name;

        switch (strategy) {
            case OptimizationStrategy::CONSERVATIVE:
                strategy_name = "Conservative";
                break;
            case OptimizationStrategy::BALANCED:
                strategy_name = "Balanced";
                break;
            case OptimizationStrategy::AGGRESSIVE:
                strategy_name = "Aggressive";
                break;
            case OptimizationStrategy::ADAPTIVE:
                strategy_name = "Adaptive";
                break;
            case OptimizationStrategy::PREDICTIVE:
                strategy_name = "Predictive";
                break;
            case OptimizationStrategy::EXPERIMENTAL:
                strategy_name = "Experimental";
                break;
            default:
                strategy_name = "Unknown";
        }

        std::cout << std::left << std::setw(15) << strategy_name << ": " << std::fixed
                  << std::setprecision(2) << (improvement * 100.0) << "% improvement" << std::endl;
    }

    std::cout << std::endl;
}

// Helper function to print pattern recognition results
void printPatternRecognitionResults(const std::string& operation_id,
                                    const PatternRecognitionResult& result) {
    std::cout << "\nPattern Recognition Results for '" << operation_id << "'\n";
    std::cout << "===============================================\n";
    std::cout << "Primary Pattern: " << result.primary_pattern << "\n";
    std::cout << "Confidence: " << std::fixed << std::setprecision(2) << (result.confidence * 100.0)
              << "%\n";

    if (result.is_seasonal) {
        std::cout << "Seasonal Pattern Detected with Period: " << result.seasonality_period << "\n";
    }

    std::cout << "\nPattern Probabilities:\n";
    for (const auto& [pattern, probability] : result.pattern_probabilities) {
        std::cout << "  " << std::left << std::setw(12) << pattern << ": " << std::fixed
                  << std::setprecision(2) << (probability * 100.0) << "%\n";
    }

    if (!result.trend_coefficients.empty()) {
        std::cout << "\nTrend Coefficients:\n";
        for (size_t i = 0; i < result.trend_coefficients.size(); ++i) {
            std::cout << "  Degree " << i << ": " << result.trend_coefficients[i] << "\n";
        }
    }

    std::cout << std::endl;
}

int main() {
    std::cout << "Advanced Optimization Algorithms Demo\n";
    std::cout << "====================================\n\n";

    // Create runtime and debt tracker
    auto runtime = std::make_shared<TemporalRuntime>();
    auto debt_tracker = std::make_shared<TemporalDebtTracker>(runtime);

    // Create the advanced optimizer
    AdvancedOptimizationAlgorithms optimizer(runtime, debt_tracker);

    std::cout << "Simulating different operation patterns...\n\n";

    // Simulate operations with different patterns
    simulateOperationWithPattern(runtime, "repetitive_op", "repetitive", 20, 100.0, 50.0);
    simulateOperationWithPattern(runtime, "increasing_op", "increasing", 20, 100.0, 50.0);
    simulateOperationWithPattern(runtime, "decreasing_op", "decreasing", 20, 100.0, 50.0);
    simulateOperationWithPattern(runtime, "cyclic_op", "cyclic", 20, 100.0, 50.0);
    simulateOperationWithPattern(runtime, "spike_op", "spike", 20, 100.0, 50.0);
    simulateOperationWithPattern(runtime, "random_op", "random", 20, 100.0, 50.0);

    std::cout << "\nPerforming pattern detection with different algorithms...\n";

    // Test pattern detection with different algorithms
    std::vector<std::pair<std::string, std::string>> operations_to_test = {
        {"repetitive_op", "Repetitive Pattern"},
        {"increasing_op", "Increasing Pattern"},
        {"cyclic_op", "Cyclic Pattern"},
        {"spike_op", "Spike Pattern"}};

    std::vector<std::pair<PatternMatchingAlgorithm, std::string>> algorithms_to_test = {
        {PatternMatchingAlgorithm::BASIC_STATISTICAL, "Basic Statistical"},
        {PatternMatchingAlgorithm::SLIDING_WINDOW, "Sliding Window"},
        {PatternMatchingAlgorithm::FOURIER_TRANSFORM, "Fourier Transform"},
        {PatternMatchingAlgorithm::REGRESSION_ANALYSIS, "Regression Analysis"}};

    for (const auto& [operation_id, description] : operations_to_test) {
        std::cout << "\nTesting pattern detection for: " << description << " (" << operation_id
                  << ")\n";
        std::cout << std::string(60, '-') << "\n";

        for (const auto& [algorithm, algo_name] : algorithms_to_test) {
            std::cout << "Using " << algo_name << " algorithm:\n";
            auto result = optimizer.detectPatternAdvanced(operation_id, algorithm);
            std::cout << "  Detected Pattern: " << result.primary_pattern
                      << " (Confidence: " << std::fixed << std::setprecision(2)
                      << (result.confidence * 100.0) << "%)\n";
        }
    }

    // Print detailed pattern recognition results for one operation
    auto detailed_result = optimizer.detectPatternAdvanced("cyclic_op");
    printPatternRecognitionResults("cyclic_op", detailed_result);

    std::cout << "\nComparing optimization strategies...\n";

    // Compare different optimization strategies
    std::vector<OptimizationStrategy> strategies = {
        OptimizationStrategy::CONSERVATIVE, OptimizationStrategy::BALANCED,
        OptimizationStrategy::AGGRESSIVE,   OptimizationStrategy::ADAPTIVE,
        OptimizationStrategy::PREDICTIVE,   OptimizationStrategy::EXPERIMENTAL};

    for (const auto& [operation_id, description] : operations_to_test) {
        auto results = optimizer.compareOptimizationStrategies(operation_id, strategies);
        printOptimizationResults("Strategy Comparison for " + description, results);
    }

    // Demonstrate comprehensive optimization
    std::cout << "\nPerforming comprehensive optimization...\n";
    std::cout << "--------------------------------------\n";

    for (const auto& [operation_id, description] : operations_to_test) {
        double improvement = optimizer.optimizeComprehensive(operation_id);
        std::cout << "Comprehensive optimization for " << description << " achieved " << std::fixed
                  << std::setprecision(2) << (improvement * 100.0) << "% improvement\n";
    }

    // Generate and display efficiency report
    std::cout << "\nGenerating efficiency report...\n";
    std::cout << std::string(30, '-') << "\n";
    std::string report = optimizer.generateEfficiencyReport(true);
    std::cout << report << std::endl;

    // Visualize optimization opportunities
    std::cout << "\nVisualizing optimization opportunities...\n";
    std::cout << std::string(40, '-') << "\n";
    std::string visualization = optimizer.visualizeOptimizationOpportunities();
    std::cout << visualization << std::endl;

    std::cout << "\nAdvanced Optimization Demo complete!\n";

    return 0;
}