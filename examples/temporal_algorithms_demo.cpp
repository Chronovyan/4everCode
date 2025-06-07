/**
 * @file temporal_algorithms_demo.cpp
 * @brief Demonstration of the Chronovyan Temporal Algorithms
 *
 * This example demonstrates the usage of various temporal algorithms
 * provided by the Chronovyan Standard Library.
 *
 * @copyright Chronovyan Project
 */

#include <chrono>
#include <iomanip>
#include <iostream>
#include <thread>

#include "../include/stdlib/chronovyan_stdlib.h"
#include "../include/stdlib/temporal_algorithms.h"
#include "../include/temporal_sequence.h"
#include "../include/value.h"

// Helper function to display a temporal sequence
void display_sequence(const std::string& name, const chronovyan::TemporalSequence& sequence) {
    std::cout << "\n=== " << name << " ===\n";

    const auto& points = sequence.get_all_points();
    if (points.empty()) {
        std::cout << "  [Empty sequence]\n";
        return;
    }

    // Display header
    std::cout << std::setw(10) << "Timestamp" << std::setw(15) << "Value" << "\n";
    std::cout << std::string(25, '-') << "\n";

    // Display each point
    for (const auto& point : points) {
        std::cout << std::setw(10) << point.first << std::setw(15) << point.second.to_string()
                  << "\n";
    }

    std::cout << std::endl;
}

// Helper function to create a test temporal sequence
chronovyan::TemporalSequence create_test_sequence() {
    chronovyan::TemporalSequence sequence;

    // Add some data points
    sequence.add_point("T100", chronovyan::Value(10.0));
    sequence.add_point("T110", chronovyan::Value(15.0));
    sequence.add_point("T120", chronovyan::Value(13.0));
    sequence.add_point("T130", chronovyan::Value(18.0));
    sequence.add_point("T140", chronovyan::Value(21.0));
    sequence.add_point("T150", chronovyan::Value(19.0));
    sequence.add_point("T160", chronovyan::Value(22.0));
    sequence.add_point("T170", chronovyan::Value(25.0));
    sequence.add_point("T180", chronovyan::Value(23.0));
    sequence.add_point("T190", chronovyan::Value(26.0));

    return sequence;
}

// Helper function to create a cyclic test sequence
chronovyan::TemporalSequence create_cyclic_sequence() {
    chronovyan::TemporalSequence sequence;

    // Add some cyclic data points (sine wave pattern)
    for (int i = 0; i < 20; ++i) {
        double angle = i * 0.314159;  // approx PI/10
        double value = 10.0 + 5.0 * sin(angle);
        std::string timestamp = "T" + std::to_string(100 + i * 10);
        sequence.add_point(timestamp, chronovyan::Value(value));
    }

    return sequence;
}

// Helper function to create multiple timelines for merging
std::vector<chronovyan::TemporalSequence> create_timelines() {
    std::vector<chronovyan::TemporalSequence> timelines;

    // Timeline 1: Increasing trend
    chronovyan::TemporalSequence timeline1;
    for (int i = 0; i < 10; ++i) {
        timeline1.add_point("T" + std::to_string(100 + i * 10), chronovyan::Value(10.0 + i * 2.0));
    }
    timelines.push_back(timeline1);

    // Timeline 2: Decreasing trend
    chronovyan::TemporalSequence timeline2;
    for (int i = 0; i < 10; ++i) {
        timeline2.add_point("T" + std::to_string(100 + i * 10), chronovyan::Value(30.0 - i * 1.5));
    }
    timelines.push_back(timeline2);

    // Timeline 3: Cyclic trend
    chronovyan::TemporalSequence timeline3;
    for (int i = 0; i < 10; ++i) {
        double angle = i * 0.628318;  // approx PI/5
        timeline3.add_point("T" + std::to_string(100 + i * 10),
                            chronovyan::Value(20.0 + 5.0 * sin(angle)));
    }
    timelines.push_back(timeline3);

    return timelines;
}

int main() {
    std::cout << "===========================================\n";
    std::cout << "  Chronovyan Temporal Algorithms Demo\n";
    std::cout << "===========================================\n";

    // Initialize the standard library
    chronovyan::stdlib::initialize();

    // Create test sequences
    chronovyan::TemporalSequence sequence = create_test_sequence();
    chronovyan::TemporalSequence cyclic_sequence = create_cyclic_sequence();
    std::vector<chronovyan::TemporalSequence> timelines = create_timelines();

    // Display the original sequence
    display_sequence("Original Sequence", sequence);

    // Demonstrate temporal_map
    std::cout << "\n--- Demonstrating temporal_map ---\n";
    chronovyan::TemporalSequence mapped_sequence =
        chronovyan::stdlib::algorithms::temporal_map(sequence, [](const chronovyan::Value& v) {
            return chronovyan::Value(v.as_number() * 2.0);
        });
    display_sequence("Mapped Sequence (doubled)", mapped_sequence);

    // Demonstrate temporal_filter
    std::cout << "\n--- Demonstrating temporal_filter ---\n";
    chronovyan::TemporalSequence filtered_sequence =
        chronovyan::stdlib::algorithms::temporal_filter(
            sequence, [](const chronovyan::Value& v) { return v.as_number() > 20.0; });
    display_sequence("Filtered Sequence (values > 20)", filtered_sequence);

    // Demonstrate temporal_reduce
    std::cout << "\n--- Demonstrating temporal_reduce ---\n";
    chronovyan::Value sum = chronovyan::stdlib::algorithms::temporal_reduce(
        sequence, chronovyan::Value(0.0),
        [](const chronovyan::Value& acc, const chronovyan::Value& v) {
            return chronovyan::Value(acc.as_number() + v.as_number());
        });
    std::cout << "Sum of all values: " << sum.to_string() << std::endl;

    // Demonstrate temporal_interpolate
    std::cout << "\n--- Demonstrating temporal_interpolate ---\n";
    chronovyan::TemporalSequence interpolated_sequence =
        chronovyan::stdlib::algorithms::temporal_interpolate(sequence, 2, "linear");
    display_sequence("Interpolated Sequence (2 points between each pair, linear)",
                     interpolated_sequence);

    // Demonstrate merge_timelines
    std::cout << "\n--- Demonstrating merge_timelines ---\n";
    std::cout << "Original Timelines:\n";
    for (size_t i = 0; i < timelines.size(); ++i) {
        display_sequence("Timeline " + std::to_string(i + 1), timelines[i]);
    }

    chronovyan::TemporalSequence merged_latest =
        chronovyan::stdlib::algorithms::merge_timelines(timelines, "latest");
    display_sequence("Merged Timeline (latest strategy)", merged_latest);

    chronovyan::TemporalSequence merged_average =
        chronovyan::stdlib::algorithms::merge_timelines(timelines, "average");
    display_sequence("Merged Timeline (average strategy)", merged_average);

    // Demonstrate find_timeline_divergences
    std::cout << "\n--- Demonstrating find_timeline_divergences ---\n";
    std::vector<std::string> divergences =
        chronovyan::stdlib::algorithms::find_timeline_divergences(timelines[0], timelines[1], 0.1);

    std::cout << "Divergence points between Timeline 1 and Timeline 2:\n";
    if (divergences.empty()) {
        std::cout << "  No significant divergences found\n";
    } else {
        for (const auto& point : divergences) {
            std::cout << "  " << point << "\n";
        }
    }
    std::cout << std::endl;

    // Demonstrate create_branch_timeline
    std::cout << "\n--- Demonstrating create_branch_timeline ---\n";
    chronovyan::TemporalSequence branched_timeline =
        chronovyan::stdlib::algorithms::create_branch_timeline(
            sequence, "T130",
            [](const chronovyan::Value& v) { return chronovyan::Value(v.as_number() * 0.5); });
    display_sequence("Branched Timeline (from T130, values halved)", branched_timeline);

    // Demonstrate detect_temporal_pattern
    std::cout << "\n--- Demonstrating detect_temporal_pattern ---\n";
    display_sequence("Cyclic Sequence", cyclic_sequence);

    chronovyan::stdlib::algorithms::PatternDetectionResult pattern =
        chronovyan::stdlib::algorithms::detect_temporal_pattern(cyclic_sequence, 0.7);

    std::cout << "Pattern Detection Results:\n";
    std::cout << "  Pattern Type: ";
    switch (pattern.pattern_type) {
        case chronovyan::stdlib::algorithms::TemporalPatternType::CONSTANT:
            std::cout << "CONSTANT\n";
            break;
        case chronovyan::stdlib::algorithms::TemporalPatternType::INCREASING:
            std::cout << "INCREASING\n";
            break;
        case chronovyan::stdlib::algorithms::TemporalPatternType::DECREASING:
            std::cout << "DECREASING\n";
            break;
        case chronovyan::stdlib::algorithms::TemporalPatternType::CYCLIC:
            std::cout << "CYCLIC\n";
            break;
        case chronovyan::stdlib::algorithms::TemporalPatternType::SPIKE:
            std::cout << "SPIKE\n";
            break;
        case chronovyan::stdlib::algorithms::TemporalPatternType::IRREGULAR:
            std::cout << "IRREGULAR\n";
            break;
    }
    std::cout << "  Confidence: " << pattern.confidence << "\n";

    if (pattern.pattern_type == chronovyan::stdlib::algorithms::TemporalPatternType::CYCLIC) {
        std::cout << "  Period: " << pattern.period << "\n";
    } else if (pattern.pattern_type ==
                   chronovyan::stdlib::algorithms::TemporalPatternType::INCREASING ||
               pattern.pattern_type ==
                   chronovyan::stdlib::algorithms::TemporalPatternType::DECREASING) {
        std::cout << "  Growth Rate: " << pattern.growth_rate << "\n";
    }
    std::cout << std::endl;

    // Demonstrate forecast_temporal_sequence
    std::cout << "\n--- Demonstrating forecast_temporal_sequence ---\n";
    chronovyan::TemporalSequence forecast =
        chronovyan::stdlib::algorithms::forecast_temporal_sequence(cyclic_sequence, 5, "auto");
    display_sequence("Forecasted Sequence (5 periods, auto method)", forecast);

    // Demonstrate find_optimal_timeline_path
    std::cout << "\n--- Demonstrating find_optimal_timeline_path ---\n";
    std::vector<std::string> path = chronovyan::stdlib::algorithms::find_optimal_timeline_path(
        timelines, chronovyan::Value(10.0), chronovyan::Value(25.0),
        [](const chronovyan::Value& current, const chronovyan::Value& goal) {
            return std::abs(current.as_number() - goal.as_number());
        });

    std::cout << "Optimal path from start to goal:\n";
    for (const auto& point : path) {
        std::cout << "  " << point << "\n";
    }
    std::cout << std::endl;

    // Shutdown the standard library
    chronovyan::stdlib::shutdown();

    std::cout << "===========================================\n";
    std::cout << "  Temporal Algorithms Demo Completed\n";
    std::cout << "===========================================\n";

    return 0;
}