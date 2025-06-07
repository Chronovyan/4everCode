#include <gtest/gtest.h>

#include <chrono>
#include <numeric>
#include <thread>

#include "chronovyan/temporal_synchronizer.hpp"

using namespace chronovyan::sync;

class MLOptimizationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Configure ML optimization using the existing MLConfig structure
        TemporalSynchronizer::MLConfig ml_config;
        ml_config.enable_ml_optimization = true;
        ml_config.learning_rate = 0.01;
        ml_config.training_window = 1000;
        ml_config.feature_columns = {"sync_efficiency", "stability", "coherence"};
        ml_config.model_type = "gradient_boost";
        ml_config.enable_auto_tuning = true;

        synchronizer.configure_ml(ml_config);

        // Configure real-time optimization
        TemporalSynchronizer::RealTimeConfig real_time_config;
        real_time_config.enable_real_time_optimization = true;
        real_time_config.optimization_interval = std::chrono::milliseconds(100);
        real_time_config.target_latency = 50.0;
        real_time_config.optimization_window = 100;
        real_time_config.enable_adaptive_thresholds = true;

        synchronizer.configure_real_time(real_time_config);
    }

    TemporalSynchronizer synchronizer;
};

TEST_F(MLOptimizationTest, MLModelInitialization) {
    // Test ML model initialization by checking overall health metrics
    auto health = synchronizer.get_health_metrics();
    EXPECT_GE(health.system_health, 0.0);
    EXPECT_LE(health.system_health, 1.0);
}

TEST_F(MLOptimizationTest, SyncPointManagement) {
    // Test sync point management
    synchronizer.synchronize_temporal_flows();
    auto sync_points = synchronizer.get_sync_points();
    EXPECT_FALSE(sync_points.empty());
}

TEST_F(MLOptimizationTest, SyncPatternManagement) {
    // Test sync pattern management
    synchronizer.synchronize_temporal_flows();
    auto sync_patterns = synchronizer.get_sync_patterns();
    EXPECT_FALSE(sync_patterns.empty());
}

TEST_F(MLOptimizationTest, StabilityCalculation) {
    // Test stability calculation
    synchronizer.synchronize_temporal_flows();
    auto stability = synchronizer.get_overall_stability();
    EXPECT_GE(stability, 0.0);
    EXPECT_LE(stability, 1.0);
}

TEST_F(MLOptimizationTest, CoherenceCalculation) {
    // Test coherence calculation
    synchronizer.synchronize_temporal_flows();
    auto coherence = synchronizer.get_overall_coherence();
    EXPECT_GE(coherence, 0.0);
    EXPECT_LE(coherence, 1.0);
}

TEST_F(MLOptimizationTest, ErrorPrediction) {
    // Test error prediction
    auto prediction = synchronizer.predict_next_error();
    EXPECT_GE(prediction.probability, 0.0);
    EXPECT_LE(prediction.probability, 1.0);
}

TEST_F(MLOptimizationTest, PatternRecognition) {
    // Test pattern recognition
    synchronizer.synchronize_temporal_flows();
    auto analysis = synchronizer.analyze_current_pattern();
    EXPECT_GE(analysis.confidence, 0.0);
    EXPECT_LE(analysis.confidence, 1.0);
}

TEST_F(MLOptimizationTest, StateAnalysis) {
    // Test state analysis
    auto analysis = synchronizer.analyze_current_state();
    EXPECT_GE(analysis.health_score, 0.0);
    EXPECT_LE(analysis.health_score, 1.0);
}

TEST_F(MLOptimizationTest, PerformanceTracking) {
    // Test performance tracking
    synchronizer.synchronize_temporal_flows();
    auto metrics = synchronizer.get_performance_metrics();
    EXPECT_GE(metrics.sync_success_rate, 0.0);
    EXPECT_LE(metrics.sync_success_rate, 1.0);
}