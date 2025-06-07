#include <cmath>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "../include/resource_visualization.h"
#include "../include/temporal_runtime.h"
#include "../include/temporal_sequence.h"

using namespace chronovyan;

// Helper function to print a separator line
void printSeparator() { std::cout << "\n" << std::string(70, '-') << "\n" << std::endl; }

// Helper function to print the sequence
void printSequence(const TemporalSequence& sequence, const std::string& name) {
    std::cout << name << " (size: " << sequence.size() << "):" << std::endl;
    std::cout << sequence.toString() << std::endl;
}

// Helper function to visualize a numeric sequence
void visualizeSequence(const TemporalSequence& sequence, const std::string& name) {
    std::cout << name << " visualization:" << std::endl;

    // Find min and max values for scaling
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();

    for (int cycle : sequence.getCycles()) {
        const Value& value = sequence.at(cycle);
        double numValue = 0.0;

        if (value.getType() == Value::Type::INTEGER) {
            numValue = static_cast<double>(value.asInteger());
        } else if (value.getType() == Value::Type::FLOAT) {
            numValue = value.asFloat();
        } else {
            std::cout << "  Cannot visualize non-numeric sequence" << std::endl;
            return;
        }

        minValue = std::min(minValue, numValue);
        maxValue = std::max(maxValue, numValue);
    }

    // Width of the visualization
    const int width = 50;
    const double range = maxValue - minValue;

    // Display each point
    for (int cycle : sequence.getCycles()) {
        const Value& value = sequence.at(cycle);
        double numValue = 0.0;

        if (value.getType() == Value::Type::INTEGER) {
            numValue = static_cast<double>(value.asInteger());
        } else {
            numValue = value.asFloat();
        }

        // Calculate bar length
        int barLength =
            range > 0 ? static_cast<int>((numValue - minValue) / range * width) : width / 2;

        std::cout << "  Cycle " << std::setw(3) << cycle << " | " << std::setw(8) << std::fixed
                  << std::setprecision(2) << numValue << " | " << std::string(barLength, '*')
                  << std::endl;
    }
}

// Helper function to print statistics
void printStatistics(const std::map<std::string, Value>& stats) {
    std::cout << "Statistics:" << std::endl;

    for (const auto& [key, value] : stats) {
        std::cout << "  " << std::setw(12) << key << ": " << value.toString() << std::endl;
    }
}

int main() {
    // Initialize the temporal runtime
    auto runtime = std::make_shared<TemporalRuntime>();
    runtime->setChrononsLimit(10000);  // Set a high limit for the demo

    std::cout << "Chronovyan TEMPORAL_SEQUENCE Type Demonstration" << std::endl;
    printSeparator();

    // 1. Basic Operations
    std::cout << "1. Basic Operations" << std::endl;
    TemporalSequence basicSequence(runtime);

    std::cout << "Creating a sequence with time points..." << std::endl;
    basicSequence.set(1, Value(static_cast<int64_t>(100)));
    basicSequence.set(3, Value(static_cast<int64_t>(300)));
    basicSequence.set(5, Value(static_cast<int64_t>(500)));

    printSequence(basicSequence, "Basic Sequence");

    // Accessing values
    std::cout << "\nAccessing values:" << std::endl;
    std::cout << "Value at cycle 1: " << basicSequence.at(1).toString() << std::endl;
    std::cout << "Value at cycle 3: " << basicSequence.at(3).toString() << std::endl;
    std::cout << "Value at cycle 5: " << basicSequence.at(5).toString() << std::endl;

    // Check cycle existence
    std::cout << "\nChecking cycle existence:" << std::endl;
    std::cout << "Has cycle 1: " << (basicSequence.hasCycle(1) ? "Yes" : "No") << std::endl;
    std::cout << "Has cycle 2: " << (basicSequence.hasCycle(2) ? "Yes" : "No") << std::endl;

    // Get all cycles
    std::cout << "\nAll cycles in sequence: ";
    for (int cycle : basicSequence.getCycles()) {
        std::cout << cycle << " ";
    }
    std::cout << std::endl;

    // Modifying values
    std::cout << "\nModifying values:" << std::endl;
    basicSequence.set(3, Value(static_cast<int64_t>(350)));
    std::cout << "Updated value at cycle 3: " << basicSequence.at(3).toString() << std::endl;

    // Adding a new cycle
    basicSequence.set(2, Value(static_cast<int64_t>(200)));
    std::cout << "Added value at cycle 2: " << basicSequence.at(2).toString() << std::endl;

    printSequence(basicSequence, "Updated Sequence");

    // Removing cycles
    std::cout << "\nRemoving cycle 3..." << std::endl;
    basicSequence.remove(3);
    printSequence(basicSequence, "After Removal");

    printSeparator();

    // 2. Advanced Temporal Operations
    std::cout << "2. Advanced Temporal Operations" << std::endl;

    // Create a sequence for temporal operations
    TemporalSequence temporalSequence(runtime);

    // Add points at regular intervals
    for (int i = 0; i < 10; i++) {
        double value = 50.0 + 25.0 * std::sin(i * 0.6);
        temporalSequence.set(i, Value(value));
    }

    printSequence(temporalSequence, "Sine Wave Sequence");
    visualizeSequence(temporalSequence, "Sine Wave");

    // Get cycle range
    std::cout << "\nCycle range:" << std::endl;
    std::cout << "Earliest cycle: " << temporalSequence.getEarliestCycle() << std::endl;
    std::cout << "Latest cycle: " << temporalSequence.getLatestCycle() << std::endl;

    // Slice the sequence
    std::cout << "\nSlicing sequence from cycle 3 to 7:" << std::endl;
    auto sliced = temporalSequence.slice(3, 7);
    printSequence(*sliced, "Sliced Sequence");
    visualizeSequence(*sliced, "Sliced Sequence");

    // Interpolation
    std::cout << "\nInterpolation:" << std::endl;
    std::cout << "Value at cycle 2.5 (linear): "
              << temporalSequence.interpolate(2.5, "linear").toString() << std::endl;
    std::cout << "Value at cycle 5.7 (linear): "
              << temporalSequence.interpolate(5.7, "linear").toString() << std::endl;
    std::cout << "Value at cycle 2.5 (step): "
              << temporalSequence.interpolate(2.5, "step").toString() << std::endl;

    // Resampling
    std::cout << "\nResampling to intervals of 2 cycles:" << std::endl;
    auto resampled = temporalSequence.resample(2, "linear");
    printSequence(*resampled, "Resampled Sequence");
    visualizeSequence(*resampled, "Resampled Sequence");

    printSeparator();

    // 3. Functional Operations
    std::cout << "3. Functional Operations" << std::endl;

    // Create a sequence for functional operations
    TemporalSequence functionalSequence(runtime);

    // Add points
    for (int i = 1; i <= 5; i++) {
        functionalSequence.set(i, Value(static_cast<int64_t>(i * 10)));
    }

    printSequence(functionalSequence, "Original Sequence");

    // Map operation - double each value
    std::cout << "\nMapping: Double each value" << std::endl;
    auto doubled = functionalSequence.map([](int cycle, const Value& value) {
        return Value(static_cast<int64_t>(value.asInteger() * 2));
    });

    printSequence(*doubled, "Doubled Sequence");

    // Filter operation - keep only even cycles
    std::cout << "\nFiltering: Keep only even cycles" << std::endl;
    auto filtered =
        functionalSequence.filter([](int cycle, const Value& value) { return cycle % 2 == 0; });

    printSequence(*filtered, "Filtered Sequence");

    // Reduce operation - sum all values
    std::cout << "\nReducing: Sum all values" << std::endl;
    Value sum = functionalSequence.reduce(
        [](const Value& acc, int cycle, const Value& value) {
            return Value(static_cast<int64_t>(acc.asInteger() + value.asInteger()));
        },
        Value(static_cast<int64_t>(0)));

    std::cout << "Sum of all values: " << sum.toString() << std::endl;

    printSeparator();

    // 4. Statistical Analysis and Pattern Recognition
    std::cout << "4. Statistical Analysis and Pattern Recognition" << std::endl;

    // Create a sequence for statistical analysis
    TemporalSequence statsSequence(runtime);

    // Add points with a linear trend and some noise
    for (int i = 1; i <= 10; i++) {
        double value = i * 5.0 + (std::rand() % 10 - 5);
        statsSequence.set(i, Value(value));
    }

    printSequence(statsSequence, "Statistical Sequence");
    visualizeSequence(statsSequence, "Statistical Sequence");

    // Calculate statistics
    std::cout << "\nCalculating statistics:" << std::endl;
    auto stats = statsSequence.statistics();
    printStatistics(stats);

    // Pattern detection and projection
    std::cout << "\nDetecting patterns and projecting forward:" << std::endl;
    auto patterns = statsSequence.detectPatterns();
    std::cout << "Detected " << patterns.size() << " pattern(s)" << std::endl;

    auto projected = statsSequence.project(3);
    std::cout << "Projected 3 cycles forward:" << std::endl;
    printSequence(*projected, "Projected Sequence");
    visualizeSequence(*projected, "Projected Sequence");

    printSeparator();

    // 5. Merging Sequences
    std::cout << "5. Merging Sequences" << std::endl;

    // Create two sequences to merge
    TemporalSequence sequence1(runtime);
    TemporalSequence sequence2(runtime);

    // First sequence: odd cycles
    for (int i = 1; i <= 9; i += 2) {
        sequence1.set(i, Value(static_cast<int64_t>(i * 10)));
    }

    // Second sequence: even cycles
    for (int i = 2; i <= 10; i += 2) {
        sequence2.set(i, Value(static_cast<int64_t>(i * 10)));
    }

    // Add some overlapping cycles to demonstrate merge strategies
    sequence1.set(4, Value(static_cast<int64_t>(35)));
    sequence2.set(5, Value(static_cast<int64_t>(60)));

    printSequence(sequence1, "Sequence 1");
    printSequence(sequence2, "Sequence 2");

    // Merge with different strategies
    std::cout << "\nMerging with 'replace' strategy:" << std::endl;
    auto mergedReplace = sequence1.merge(sequence2, "replace");
    printSequence(*mergedReplace, "Merged (Replace)");

    std::cout << "\nMerging with 'keep' strategy:" << std::endl;
    auto mergedKeep = sequence1.merge(sequence2, "keep");
    printSequence(*mergedKeep, "Merged (Keep)");

    std::cout << "\nMerging with 'combine' strategy:" << std::endl;
    auto mergedCombine = sequence1.merge(sequence2, "combine");
    printSequence(*mergedCombine, "Merged (Combine)");

    printSeparator();

    // 6. Resource Usage Tracking
    std::cout << "6. Resource Usage Tracking" << std::endl;

    // Get chronons consumed
    int chrononsUsed = runtime->getChrononsUsed();
    std::cout << "Total Chronons used during this demonstration: " << chrononsUsed << std::endl;

    // Visualize resource usage if available
    try {
        ResourceVisualization resourceViz;
        std::cout << "\nResource usage visualization:" << std::endl;
        resourceViz.generateResourceUsageReport(*runtime);
    } catch (const std::exception& e) {
        std::cout << "Resource visualization not available: " << e.what() << std::endl;
    }

    printSeparator();
    std::cout << "TEMPORAL_SEQUENCE Type Demonstration Completed" << std::endl;

    return 0;
}