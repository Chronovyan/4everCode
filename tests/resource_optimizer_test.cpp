#include <gtest/gtest.h>

#include <ctime>
#include <iostream>
#include <memory>
#include <string>

#include "resource_optimizer.h"
#include "resource_tracker.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

using namespace chronovyan;

// Helper function to populate a tracker with test data
void populateTrackerWithTestData(ResourceTracker& tracker, int numPoints = 12) {
    // Add some test data with varying patterns
    for (int i = 0; i < numPoints; ++i) {
        // Create a pattern with some variation
        double chronon = 10.0 + (i % 5) * 5.0;
        double aethel = chronon * 0.5;
        double debt = (i % 3) * 0.5;
        double risk = (i % 4) * 0.1;

        // Add some spikes
        if (i % 7 == 0) {
            chronon *= 2.0;
            aethel *= 1.5;
        }

        tracker.recordCurrentUsage(chronon, aethel, debt, risk);
    }
}

// Tests for the resource_management version of ResourceOptimizer
TEST(ResourceOptimizerTest, CalculatesEfficiencyMetrics) {
    ResourceTracker tracker;
    populateTrackerWithTestData(tracker);

    ResourceOptimizer optimizer(tracker);
    auto metrics = optimizer.getEfficiencyMetrics();

    // Verify that all expected metrics are present
    EXPECT_TRUE(metrics.find("chronon_efficiency") != metrics.end());
    EXPECT_TRUE(metrics.find("aethel_efficiency") != metrics.end());
    EXPECT_TRUE(metrics.find("temporal_stability") != metrics.end());
    EXPECT_TRUE(metrics.find("utilization_variance") != metrics.end());
    EXPECT_TRUE(metrics.find("overall_efficiency") != metrics.end());

    // Verify that metrics are within valid range (0.0 to 1.0)
    EXPECT_GE(metrics["chronon_efficiency"], 0.0);
    EXPECT_LE(metrics["chronon_efficiency"], 1.0);
    EXPECT_GE(metrics["aethel_efficiency"], 0.0);
    EXPECT_LE(metrics["aethel_efficiency"], 1.0);
    EXPECT_GE(metrics["temporal_stability"], 0.0);
    EXPECT_LE(metrics["temporal_stability"], 1.0);
    EXPECT_GE(metrics["overall_efficiency"], 0.0);
    EXPECT_LE(metrics["overall_efficiency"], 1.0);
}

TEST(ResourceOptimizerTest, GeneratesRecommendations) {
    ResourceTracker tracker;
    populateTrackerWithTestData(tracker);

    ResourceOptimizer optimizer(tracker);
    auto recommendations = optimizer.generateRecommendations();

    // We should have at least one recommendation with our test data
    EXPECT_FALSE(recommendations.empty());

    // Verify that recommendations have valid improvement and confidence values
    for (const auto& rec : recommendations) {
        EXPECT_GE(rec.estimated_improvement, 0.0);
        EXPECT_LE(rec.estimated_improvement, 1.0);
        EXPECT_GE(rec.confidence, 0.0);
        EXPECT_LE(rec.confidence, 1.0);
        EXPECT_FALSE(rec.description.empty());
    }
}

TEST(ResourceOptimizerTest, GeneratesOptimizationReport) {
    ResourceTracker tracker;
    populateTrackerWithTestData(tracker);

    ResourceOptimizer optimizer(tracker);
    std::string report = optimizer.generateOptimizationReport();

    // Print the report for debugging
    std::cout << "Optimization Report:\n" << report << std::endl;

    // Verify that the report contains key sections
    EXPECT_TRUE(report.find("Chronovyan Resource Optimization Analysis") != std::string::npos);
    EXPECT_TRUE(report.find("EFFICIENCY METRICS:") != std::string::npos);
    EXPECT_TRUE(report.find("OVERALL EFFICIENCY:") != std::string::npos);
    EXPECT_TRUE(report.find("OPTIMIZATION RECOMMENDATIONS:") != std::string::npos);
}

TEST(ResourceOptimizerTest, HandlesInsufficientData) {
    ResourceTracker tracker;
    // Add only a few data points (less than the default minimum)
    tracker.recordCurrentUsage(10.0, 5.0);
    tracker.recordCurrentUsage(15.0, 7.5);

    ResourceOptimizer optimizer(tracker);

    // Set a higher minimum data points requirement
    optimizer.setMinimumDataPoints(5);

    // Check that recommendations indicate insufficient data
    auto recommendations = optimizer.generateRecommendations();
    EXPECT_EQ(recommendations.size(), 1);
    EXPECT_TRUE(recommendations[0].description.find("Insufficient data") != std::string::npos);

    // Check that metrics indicate insufficient data
    auto metrics = optimizer.getEfficiencyMetrics();
    EXPECT_DOUBLE_EQ(metrics["status"], 0.0);

    // Check that the report mentions insufficient data
    std::string report = optimizer.generateOptimizationReport();
    EXPECT_TRUE(report.find("Insufficient data") != std::string::npos);
}

TEST(ResourceOptimizerTest, DetectsResourceSpikes) {
    ResourceTracker tracker;

    // Create a pattern with clear spikes
    for (int i = 0; i < 15; ++i) {
        double chronon = 20.0;
        double aethel = 10.0;

        // Add spikes at specific points
        if (i == 5 || i == 10) {
            chronon = 60.0;  // 3x normal
            aethel = 30.0;   // 3x normal
        }

        tracker.recordCurrentUsage(chronon, aethel);
    }

    ResourceOptimizer optimizer(tracker);
    optimizer.setMinimumDataPoints(10);

    auto recommendations = optimizer.generateRecommendations();

    // Find spike-related recommendation
    bool has_spike_recommendation = false;
    for (const auto& rec : recommendations) {
        if (rec.description.find("spikes") != std::string::npos) {
            has_spike_recommendation = true;
            break;
        }
    }

    EXPECT_TRUE(has_spike_recommendation);
}

// Main function for running all tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}