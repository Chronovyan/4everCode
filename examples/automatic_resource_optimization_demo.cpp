#include <chrono>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include "../include/automatic_resource_optimizer.h"
#include "../include/temporal_debt_tracker.h"
#include "../include/temporal_runtime.h"

using namespace chronovyan;

// Helper function to print a timestamp
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::tm* tm = std::localtime(&time_t);

    char buffer[80];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buffer);
}

// Helper function to print dividers in output
void printDivider(const std::string& title = "") {
    std::cout << "\n";
    std::cout << "==========================================================\n";
    if (!title.empty()) {
        std::cout << "= " << title << "\n";
        std::cout << "==========================================================\n";
    }
    std::cout << "\n";
}

// Simulate a series of operations with different patterns
void simulateOperations(std::shared_ptr<TemporalRuntime> runtime,
                        std::shared_ptr<AutomaticResourceOptimizer> optimizer) {
    printDivider("Simulating Operations with Different Patterns");

    // Simulate repetitive pattern
    std::cout << "Simulating repetitive operations (consistent resource usage)...\n";
    for (int i = 0; i < 5; i++) {
        runtime->replenishChronons(100);
        optimizer->optimizeChronons("repetitive_operation");
    }

    // Simulate spike pattern
    std::cout << "Simulating spike operations (occasional high resource usage)...\n";
    for (int i = 0; i < 5; i++) {
        int amount = (i == 2) ? 300 : 100;  // Spike on 3rd operation
        runtime->replenishChronons(amount);
        optimizer->optimizeChronons("spike_operation");
    }

    // Simulate cyclic pattern
    std::cout << "Simulating cyclic operations (alternating resource usage)...\n";
    for (int i = 0; i < 6; i++) {
        int amount = (i % 2 == 0) ? 150 : 50;  // Alternate between high and low
        runtime->replenishChronons(amount);
        optimizer->optimizeChronons("cyclic_operation");
    }

    // Simulate increasing pattern
    std::cout << "Simulating increasing operations (growing resource usage)...\n";
    for (int i = 0; i < 5; i++) {
        int amount = 50 + (i * 25);  // Start at 50, increase by 25 each time
        runtime->replenishChronons(amount);
        optimizer->optimizeChronons("increasing_operation");
    }

    // Simulate decreasing pattern
    std::cout << "Simulating decreasing operations (diminishing resource usage)...\n";
    for (int i = 0; i < 5; i++) {
        int amount = 200 - (i * 25);  // Start at 200, decrease by 25 each time
        runtime->replenishChronons(amount);
        optimizer->optimizeChronons("decreasing_operation");
    }
}

// Demonstrate manual optimizations
void demonstrateManualOptimizations(std::shared_ptr<TemporalRuntime> runtime,
                                    std::shared_ptr<AutomaticResourceOptimizer> optimizer) {
    printDivider("Manual Optimization Demonstration");

    // Demonstrate basic chronon optimization
    std::cout << "Current chronons: " << runtime->getChronons() << "\n";
    std::cout << "Optimizing chronons for operation 'manual_op'...\n";
    double factor = optimizer->optimizeChronons("manual_op");
    std::cout << "Optimization factor: " << factor << "\n";
    std::cout << "Chronons after optimization: " << runtime->getChronons() << "\n\n";

    // Demonstrate advanced chronon optimization with pattern
    std::cout << "Current chronons: " << runtime->getChronons() << "\n";
    std::cout << "Performing advanced optimization with 'repetitive' pattern...\n";
    factor = optimizer->optimizeChrononsAdvanced("manual_advanced_op", "repetitive");
    std::cout << "Optimization factor: " << factor << "\n";
    std::cout << "Chronons after optimization: " << runtime->getChronons() << "\n\n";

    // Demonstrate aethel optimization
    std::cout << "Current aethel: " << runtime->getAethel() << "\n";
    std::cout << "Optimizing aethel for timeline 'manual_timeline'...\n";
    factor = optimizer->optimizeAethel("manual_timeline");
    std::cout << "Optimization factor: " << factor << "\n";
    std::cout << "Aethel after optimization: " << runtime->getAethel() << "\n";
}

// Demonstrate automatic optimization
void demonstrateAutomaticOptimization(std::shared_ptr<TemporalRuntime> runtime,
                                      std::shared_ptr<AutomaticResourceOptimizer> optimizer) {
    printDivider("Automatic Optimization Demonstration");

    // Set up a callback to report optimizations as they happen
    int callback_id = optimizer->registerOptimizationCallback([](const OptimizationResult& result) {
        std::cout << getCurrentTimestamp() << " - Automatic optimization performed:\n";
        std::cout << "  Operation: " << result.operation_id << "\n";
        std::cout << "  Resource: " << result.resource_type << "\n";
        std::cout << "  Pattern: " << result.pattern_detected << "\n";
        std::cout << "  Before: " << result.before_value << ", After: " << result.after_value
                  << "\n";
        std::cout << "  Improvement: "
                  << ((result.resource_type == "chronons")
                          ? (result.before_value - result.after_value)
                          : (result.after_value - result.before_value))
                  << "\n\n";
    });

    // Start automatic optimization
    std::cout << "Starting automatic optimization...\n";
    optimizer->startAutomaticOptimization();

    // Configure optimizer to trigger on resource bottlenecks
    OptimizationConfig config = optimizer->getConfig();
    config.optimize_on_resource_bottleneck = true;
    config.resource_bottleneck_threshold = 0.3;            // Trigger when resources drop below 30%
    config.monitoring_interval = std::chrono::seconds(2);  // Check every 2 seconds
    optimizer->setConfig(config);

    std::cout << "Automatic optimization configured to trigger when resources drop below 30%\n";

    // Simulate operations that will trigger automatic optimization
    std::cout << "Initial chronons: " << runtime->getChronons() << "\n";

    // Create a resource bottleneck to trigger automatic optimization
    int to_consume = static_cast<int>(runtime->getChronons() * 0.8);  // Consume 80% of chronons
    std::cout << "Creating resource bottleneck by consuming " << to_consume << " chronons...\n";
    runtime->consumeChronons(to_consume);
    std::cout << "Chronons after consumption: " << runtime->getChronons() << "\n";

    // Wait for automatic optimization to trigger
    std::cout << "Waiting for automatic optimization to trigger...\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));

    // Stop automatic optimization
    std::cout << "Stopping automatic optimization...\n";
    optimizer->stopAutomaticOptimization();

    // Unregister callback
    optimizer->unregisterOptimizationCallback(callback_id);
}

// Demonstrate optimization reports
void demonstrateOptimizationReports(std::shared_ptr<AutomaticResourceOptimizer> optimizer) {
    printDivider("Optimization Reports Demonstration");

    // Generate a basic report
    std::cout << "Generating basic optimization report...\n\n";
    std::string basic_report = optimizer->generateOptimizationReport(false);
    std::cout << basic_report << "\n";

    // Generate a detailed report
    std::cout << "Generating detailed optimization report...\n\n";
    std::string detailed_report = optimizer->generateOptimizationReport(true);
    std::cout << detailed_report << "\n";
}

// Demonstrate optimization opportunities
void demonstrateOptimizationOpportunities(std::shared_ptr<AutomaticResourceOptimizer> optimizer) {
    printDivider("Optimization Opportunities Demonstration");

    // Identify optimization opportunities
    std::cout << "Identifying optimization opportunities...\n\n";
    auto opportunities = optimizer->identifyOptimizationOpportunities();

    if (opportunities.empty()) {
        std::cout << "No optimization opportunities identified.\n";
        return;
    }

    std::cout << "Identified " << opportunities.size() << " optimization opportunities:\n\n";

    // Print opportunities
    int i = 1;
    for (const auto& [operation, factor] : opportunities) {
        double potential_savings = (1.0 - factor) * 100.0;
        std::cout << i << ". Operation: " << operation << "\n";
        std::cout << "   Potential optimization factor: " << factor << "\n";
        std::cout << "   Potential savings: " << std::fixed << std::setprecision(1)
                  << potential_savings << "%\n\n";
        i++;
    }

    // Demonstrate manual optimization cycle
    std::cout << "Performing a manual optimization cycle...\n";
    int count = optimizer->performOptimizationCycle();
    std::cout << "Performed " << count << " optimizations.\n";
}

// Main function
int main() {
    printDivider("Automatic Resource Optimization Demo");

    // Create the runtime
    auto runtime = std::make_shared<TemporalRuntime>();

    // Create debt tracker
    auto debt_tracker = std::make_shared<TemporalDebtTracker>(runtime);

    // Configure automatic optimization
    OptimizationConfig config;
    config.monitoring_interval = std::chrono::seconds(2);
    config.minimum_improvement_threshold = 0.05;  // 5% minimum improvement
    config.max_optimizations_per_cycle = 3;
    config.learn_from_manual_optimizations = true;

    // Create the automatic optimizer
    auto optimizer = std::make_shared<AutomaticResourceOptimizer>(runtime, debt_tracker, config);

    // Initialize runtime with resources
    runtime->replenishChronons(1000);
    runtime->replenishAethel(500);

    std::cout << "Initialized runtime with 1000 chronons and 500 aethel.\n";

    // Run the demonstrations
    simulateOperations(runtime, optimizer);
    demonstrateManualOptimizations(runtime, optimizer);
    demonstrateOptimizationOpportunities(optimizer);
    demonstrateAutomaticOptimization(runtime, optimizer);
    demonstrateOptimizationReports(optimizer);

    printDivider("Demonstration Complete");
    return 0;
}