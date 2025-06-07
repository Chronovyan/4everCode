#include <chrono>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "../include/resource_optimizer.h"
#include "../include/temporal_debt_tracker.h"
#include "../include/temporal_runtime.h"
#include "gtest/gtest.h"

namespace chronovyan {
// Simplified ResourceTracker for testing purposes
class TestResourceTracker {
public:
    TestResourceTracker() {}

    void setResourceLevel(const std::string& resource, double level) {
        resources[resource] = level;
    }

    double getResourceLevel(const std::string& resource) const {
        auto it = resources.find(resource);
        return (it != resources.end()) ? it->second : 0.0;
    }

private:
    std::map<std::string, double> resources;
};
}  // namespace chronovyan

// Test fixture class for the core ResourceOptimizer
class CoreResourceOptimizerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create runtime and optimizer
        runtime = std::make_shared<chronovyan::TemporalRuntime>();
        debt_tracker = std::make_shared<chronovyan::TemporalDebtTracker>(runtime);
        resourceOptimizer = std::make_shared<chronovyan::ResourceOptimizer>(runtime, debt_tracker);

        // Create test resource tracker
        resourceTracker = std::make_shared<chronovyan::TestResourceTracker>();

        // Initialize runtime with some resources
        runtime->replenishChronons(1000);
        runtime->replenishAethel(500);
    }

    void TearDown() override {
        // Clean up resources
        resourceOptimizer.reset();
        debt_tracker.reset();
        runtime.reset();
        resourceTracker.reset();
    }

    // Shared resources for tests
    std::shared_ptr<chronovyan::TemporalRuntime> runtime;
    std::shared_ptr<chronovyan::TemporalDebtTracker> debt_tracker;
    std::shared_ptr<chronovyan::ResourceOptimizer> resourceOptimizer;
    std::shared_ptr<chronovyan::TestResourceTracker> resourceTracker;
};

TEST_F(CoreResourceOptimizerTest, CalculatesEfficiencyMetrics) {
    // Run an operation to generate metrics
    resourceOptimizer->analyzeResourceUsage();

    // Use public methods to verify the optimizer is working
    EXPECT_GE(resourceOptimizer->getEfficiencyMetric("chronon_baseline"), 0.0);
    EXPECT_LE(resourceOptimizer->getEfficiencyMetric("chronon_baseline"), 1.0);
    EXPECT_GE(resourceOptimizer->getEfficiencyMetric("aethel_baseline"), 0.0);
    EXPECT_LE(resourceOptimizer->getEfficiencyMetric("aethel_baseline"), 1.0);
}

TEST_F(CoreResourceOptimizerTest, OptimizesChrononsBasic) {
    // Test basic chronons optimization
    double factor = resourceOptimizer->optimizeChronons("test_timeline");

    // Check that optimization factor is reasonable
    EXPECT_GT(factor, 0.0);
    EXPECT_LT(factor, 2.0);  // Should not be too extreme
}

TEST_F(CoreResourceOptimizerTest, OptimizesChrononsAdvanced) {
    // Test advanced chronons optimization with different patterns
    double factor1 = resourceOptimizer->optimizeChrononsAdvanced("timeline_1", "repetitive");
    EXPECT_GT(factor1, 0.0);

    double factor2 = resourceOptimizer->optimizeChrononsAdvanced("timeline_2", "spike");
    EXPECT_GT(factor2, 0.0);

    double factor3 = resourceOptimizer->optimizeChrononsAdvanced("timeline_3", "steady");
    EXPECT_GT(factor3, 0.0);

    // Test auto-detection
    double factor4 = resourceOptimizer->optimizeChrononsAdvanced("timeline_1", "auto-detect");
    EXPECT_GT(factor4, 0.0);
}

TEST_F(CoreResourceOptimizerTest, IntegratesWithDebtTracker) {
    // Test optimization with debt tracker

    // First test without debt
    double factorNormal = resourceOptimizer->optimizeChrononsAdvanced("test_timeline", "normal");

    // Add some debt using the borrowChronons method
    debt_tracker->borrowChronons(300, "debt_test", true);

    // Test optimization with debt
    double factorWithDebt = resourceOptimizer->optimizeChrononsAdvanced("debt_test", "with-debt");

    // Add critical debt
    debt_tracker->borrowChronons(100, "critical_debt", true);

    // Force the debt to be past due by advancing the cycle
    for (int i = 0; i < 3; i++) {
        debt_tracker->advanceCycle();
    }

    // Test with critical past-due debt
    double factorWithCriticalDebt =
        resourceOptimizer->optimizeChrononsAdvanced("critical_debt", "critical");

    // In this implementation, higher factor means more optimization,
    // so with more debt, we should see higher optimization factors to save resources
    EXPECT_GE(factorWithDebt, factorNormal);
    EXPECT_GE(factorWithCriticalDebt, factorWithDebt);
}

TEST_F(CoreResourceOptimizerTest, AdaptsToChangingConditions) {
    // Test that optimizer adapts to changing conditions over time

    // Run several operations to build history with a specific pattern
    for (int i = 0; i < 5; i++) {
        resourceOptimizer->optimizeChrononsAdvanced("timeline_1", "standard");
    }

    // Record the factor from the established pattern
    double factor5 = resourceOptimizer->optimizeChrononsAdvanced("timeline_1", "standard");

    // Now change the pattern dramatically with a spike in resource usage
    runtime->replenishChronons(2000);  // Add a large number of chronons
    resourceOptimizer->optimizeChrononsAdvanced("timeline_2", "spike");

    // Record the factor from the new pattern
    double factor7 = resourceOptimizer->optimizeChrononsAdvanced("timeline_2", "spike");

    // The optimizer should adapt to the new pattern with a different factor
    EXPECT_NE(factor7, factor5);
}

TEST_F(CoreResourceOptimizerTest, CalculatesAethelEfficiencyComprehensively) {
    // First initialize the metrics
    resourceOptimizer->analyzeResourceUsage();

    // Set an initial baseline
    [[maybe_unused]] double initialBaseline =
        resourceOptimizer->getEfficiencyMetric("aethel_baseline");

    // Setup multiple operations to build history
    resourceOptimizer->optimizeAethel("timeline_1");
    resourceOptimizer->optimizeAethel("timeline_2");
    resourceOptimizer->optimizeAethel("timeline_1");

    // Run analysis to calculate metrics
    resourceOptimizer->analyzeResourceUsage();

    // Check aethel efficiency through public method
    double efficiency = resourceOptimizer->getEfficiencyMetric("aethel_baseline");

    // Check that efficiency is in valid range
    EXPECT_GE(efficiency, 0.0);
    EXPECT_LE(efficiency, 1.0);

    // Efficiency should consider multiple factors and not be zero
    EXPECT_NE(efficiency, 0.0) << "Efficiency should not be zero after operations";
}

TEST_F(CoreResourceOptimizerTest, OptimizesAethelAdvanced) {
    // Test with explicit pattern: repetitive
    double factor1 = resourceOptimizer->optimizeAethelAdvanced("timeline_1", "repetitive");
    EXPECT_GT(factor1, 1.0) << "Repetitive pattern should enhance optimization";

    // Test with explicit pattern: spike
    double factor2 = resourceOptimizer->optimizeAethelAdvanced("timeline_2", "spike");
    EXPECT_GT(factor2, factor1) << "Spike pattern should provide greater optimization";

    // Test with explicit pattern: steady
    double factor3 = resourceOptimizer->optimizeAethelAdvanced("timeline_3", "steady");
    EXPECT_GT(factor3, 1.0) << "Steady pattern should provide optimization";
    EXPECT_LT(factor3, factor2) << "Steady pattern should optimize less than spike pattern";

    // Test auto-detection of pattern
    double factor4 = resourceOptimizer->optimizeAethelAdvanced("timeline_1", "auto");
    EXPECT_GT(factor4, 1.0) << "Auto-detected pattern should provide optimization";
}

TEST_F(CoreResourceOptimizerTest, AethelOptimizationAdaptsToTemporalDebt) {
    // First optimize without debt
    double factorWithoutDebt = resourceOptimizer->optimizeAethelAdvanced("timeline_debt", "spike");

    // Add some debt using the borrowChronons method
    debt_tracker->borrowChronons(500, "debt_test", true);

    // Test optimization with debt
    double factorWithDebt = resourceOptimizer->optimizeAethelAdvanced("timeline_debt", "spike");

    // Optimization should be more conservative with debt
    EXPECT_LT(factorWithDebt, factorWithoutDebt)
        << "Optimization should be more conservative with debt";
}

TEST_F(CoreResourceOptimizerTest, OptimizesChrononsWithPatternAnalysis) {
    // Create different operation patterns for testing

    // Create a repetitive pattern (similar values)
    for (int i = 0; i < 10; ++i) {
        runtime->replenishChronons(100);
        resourceOptimizer->optimizeChronons("repetitive_op");
    }

    // Create a spike pattern (occasional high values)
    for (int i = 0; i < 5; ++i) {
        runtime->replenishChronons(100);
        resourceOptimizer->optimizeChronons("spike_op");

        if (i % 2 == 0) {
            runtime->replenishChronons(500);  // Add a spike
            resourceOptimizer->optimizeChronons("spike_op");
        }
    }

    // Create a cyclic pattern (alternating values)
    for (int i = 0; i < 6; ++i) {
        if (i % 2 == 0) {
            runtime->replenishChronons(200);
        } else {
            runtime->replenishChronons(100);
        }
        resourceOptimizer->optimizeChronons("cyclic_op");
    }

    // Create an increasing pattern
    for (int i = 0; i < 5; ++i) {
        runtime->replenishChronons(100 + i * 50);
        resourceOptimizer->optimizeChronons("increasing_op");
    }

    // Create a decreasing pattern
    for (int i = 0; i < 5; ++i) {
        runtime->replenishChronons(300 - i * 50);
        resourceOptimizer->optimizeChronons("decreasing_op");
    }

    // Test pattern analysis optimization for each pattern
    double repetitiveFactor =
        resourceOptimizer->optimizeChrononsWithPatternAnalysis("repetitive_op", 10);
    double spikeFactor = resourceOptimizer->optimizeChrononsWithPatternAnalysis("spike_op", 10);
    double cyclicFactor = resourceOptimizer->optimizeChrononsWithPatternAnalysis("cyclic_op", 10);
    double increasingFactor =
        resourceOptimizer->optimizeChrononsWithPatternAnalysis("increasing_op", 10);
    double decreasingFactor =
        resourceOptimizer->optimizeChrononsWithPatternAnalysis("decreasing_op", 10);

    // Verify the optimization factors are reasonable
    EXPECT_GT(repetitiveFactor, 0.0);
    EXPECT_LT(repetitiveFactor, 1.0);
    EXPECT_GT(spikeFactor, 0.0);
    EXPECT_LT(spikeFactor, 1.0);
    EXPECT_GT(cyclicFactor, 0.0);
    EXPECT_LT(cyclicFactor, 1.0);
    EXPECT_GT(increasingFactor, 0.0);
    EXPECT_LT(increasingFactor, 1.0);
    EXPECT_GT(decreasingFactor, 0.0);
    EXPECT_LT(decreasingFactor, 1.0);

    // The pattern-specific optimization should be different from standard optimization
    double standardFactor = resourceOptimizer->optimizeChronons("new_op");

    // At least one of the pattern-optimized factors should be different from standard
    bool hasDifferentFactor = std::abs(repetitiveFactor - standardFactor) > 0.01 ||
                              std::abs(spikeFactor - standardFactor) > 0.01 ||
                              std::abs(cyclicFactor - standardFactor) > 0.01 ||
                              std::abs(increasingFactor - standardFactor) > 0.01 ||
                              std::abs(decreasingFactor - standardFactor) > 0.01;

    EXPECT_TRUE(hasDifferentFactor)
        << "Pattern analysis should provide different optimization than standard approach";
}

TEST_F(CoreResourceOptimizerTest, ProvidesDetailedAethelEfficiencyMetrics) {
    // First run a few operations to generate history
    for (int i = 0; i < 3; i++) {
        resourceOptimizer->optimizeAethel("timeline_1");
    }

    // Test spike pattern
    runtime->replenishChronons(500);
    resourceOptimizer->optimizeChrononsAdvanced("timeline_2", "spike");

    // Add more Aethel operations
    for (int i = 0; i < 2; i++) {
        resourceOptimizer->optimizeAethelAdvanced("timeline_3", "repetitive");
    }

    // Get detailed metrics
    auto metrics = resourceOptimizer->getDetailedAethelEfficiencyMetrics();

    // Check that all expected metrics are present
    EXPECT_TRUE(metrics.find("aethel_baseline") != metrics.end());
    EXPECT_TRUE(metrics.find("aethel_generation_rate") != metrics.end());
    EXPECT_TRUE(metrics.find("aethel_to_chronon_ratio") != metrics.end());
    EXPECT_TRUE(metrics.find("aethel_stability") != metrics.end());
    EXPECT_TRUE(metrics.find("aethel_coherence") != metrics.end());
    EXPECT_TRUE(metrics.find("aethel_resonance") != metrics.end());
    EXPECT_TRUE(metrics.find("aethel_flux_alignment") != metrics.end());

    // Verify that metrics are in valid ranges (except for raw values which can exceed 1.0)
    for (const auto& pair : metrics) {
        // Skip checking the raw value which is not normalized
        if (pair.first == "aethel_generation_rate_raw") {
            continue;
        }

        EXPECT_GE(pair.second, 0.0) << "Metric " << pair.first << " should be >= 0.0";
        EXPECT_LE(pair.second, 1.0) << "Metric " << pair.first << " should be <= 1.0";
    }

    // Force a recalculation of metrics
    resourceOptimizer->analyzeResourceUsage();

    // Now get aethel efficiency using getEfficiencyMetric
    double efficiency = resourceOptimizer->getEfficiencyMetric("aethel_efficiency");

    // Verify that it's calculated correctly
    EXPECT_GE(efficiency, 0.0);
    EXPECT_LE(efficiency, 1.0);

    // Test individual metric calculations
    double generationRate = resourceOptimizer->calculateAethelGenerationRate(100);
    EXPECT_GT(generationRate, 0.0);

    double ratio = resourceOptimizer->calculateAethelToChrononRatio(100);
    EXPECT_GE(ratio, 0.0);

    double stability = resourceOptimizer->calculateAethelStability();
    EXPECT_GE(stability, 0.0);
    EXPECT_LE(stability, 1.0);
}

TEST_F(CoreResourceOptimizerTest, AdvancedOptimization) {
    // Configure advanced optimization test
    resourceTracker->setResourceLevel("chronons", 100.0);
    resourceTracker->setResourceLevel("aethel", 100.0);

    // Test advanced optimization for Chronons
    double factor1 = resourceOptimizer->optimizeChrononsAdvanced("timeline_1", "pattern_1");
    EXPECT_GT(factor1, 0.0);

    // Test pattern-based optimization for Chronons
    double factor2 = resourceOptimizer->optimizeChrononsWithPatternAnalysis("operation_2", 5);
    EXPECT_GT(factor2, 0.0);

    // Test basic Aethel optimization
    double factor3 = resourceOptimizer->optimizeAethel("timeline_1");
    EXPECT_GT(factor3, 0.0);

    // Test advanced Aethel optimization
    double factor4 = resourceOptimizer->optimizeAethelAdvanced("timeline_1", "pattern_2");
    EXPECT_GT(factor4, 0.0);
}

// Main function for running all tests
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}