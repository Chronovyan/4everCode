#include <chrono>
#include <cmath>
#include <iostream>
#include <random>
#include <thread>

#include "../resource_management/resource_processor.h"

using namespace chronovyan;

// Function to simulate fluctuating resource usage
void simulateResourceUsage(ResourceProcessor& processor, int iterations) {
    // Create a random number generator
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> random_factor(0.8, 1.2);

    // Base values for resources
    double chronon_base = 20.0;
    double aethel_base = 10.0;
    double debt_base = 1.0;
    double risk_base = 0.2;

    // Simulate resource usage over time with some randomness and oscillation
    for (int i = 0; i < iterations; ++i) {
        // Calculate oscillating factor based on time (sine wave pattern)
        double time_factor = 1.0 + 0.5 * std::sin(i * 0.2);

        // Add some randomness
        double random = random_factor(gen);

        // Calculate current resource values with oscillation and randomness
        double chronon = chronon_base * time_factor * random;
        double aethel = aethel_base * time_factor * random;
        double debt = debt_base * time_factor * random;
        double risk = risk_base * time_factor * random;

        // Every 10th iteration, simulate a resource spike
        if (i > 0 && i % 10 == 0) {
            chronon *= 2.0;
            aethel *= 2.0;
            debt *= 1.5;
            risk *= 1.5;
        }

        // Process the current resource usage
        bool healthy = processor.processCurrentUsage(chronon, aethel, debt, risk);

        // Print status
        std::cout << "Iteration " << i + 1 << "/" << iterations << ": ";
        std::cout << "Resource state is " << (healthy ? "HEALTHY" : "CRITICAL") << std::endl;

        // Every 5 iterations, print a detailed report
        if (i > 0 && i % 5 == 0) {
            std::cout << "\n--- Current Resource Summary ---\n";
            std::cout << processor.getSummary() << std::endl;
        }

        // Every 15 iterations, print a visualization
        if (i > 0 && i % 15 == 0) {
            std::cout << "\n--- Resource Visualization ---\n";
            std::cout << processor.getVisualization() << std::endl;
        }

        // Pause briefly to simulate time passing
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    std::cout << "==================================" << std::endl;
    std::cout << "Chronovyan Resource Management Demo" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << std::endl;

    // Create a resource processor with a history size of 50 entries
    ResourceProcessor processor(50);

    // Run the simulation for 30 iterations
    std::cout << "Starting resource usage simulation...\n" << std::endl;
    simulateResourceUsage(processor, 30);

    // At the end, print a detailed report
    std::cout << "\n==================================" << std::endl;
    std::cout << "Final Resource Status:" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << processor.getDetailedReport() << std::endl;

    // Save the results to a log file
    std::string logFile = "resource_simulation_results.log";
    if (processor.saveToLog(logFile)) {
        std::cout << "Results saved to " << logFile << std::endl;
    }

    return 0;
}