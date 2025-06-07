#include <gtest/gtest.h>

#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

#include "../src/resource_management/resource_tracker.h"
#include "resource_optimizer.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

using namespace chronovyan;

class ResourceVisualizationTest : public ::testing::Test {
protected:
    void SetUp() override {
        runtime = std::make_shared<TemporalRuntime>();
        debt_tracker = std::make_shared<TemporalDebtTracker>(runtime);
        optimizer = std::make_shared<ResourceOptimizer>(runtime);
        visualization = std::make_shared<ResourceVisualization>(runtime, optimizer, debt_tracker);

        // Initialize with some resources
        runtime->replenishChronons(100.0);
        runtime->replenishAethel(100.0);
    }

    // Add some debt for testing
    void createTestDebts() {
        debt_tracker->borrowChronons(50.0, "operation1", true);  // Critical debt
        debt_tracker->borrowAethel(30.0, "operation2", false);   // Regular debt
    }

    // Make some debt past due
    void makeSomeDebtPastDue() {
        createTestDebts();
        for (int i = 0; i < 10; i++) {
            debt_tracker->advanceCycle();
        }
    }

    std::shared_ptr<TemporalRuntime> runtime;
    std::shared_ptr<ResourceOptimizer> optimizer;
    std::shared_ptr<TemporalDebtTracker> debt_tracker;
    std::shared_ptr<ResourceVisualization> visualization;
};

// Test taking a snapshot with proper context
TEST_F(ResourceVisualizationTest, TakeSnapshotWithContext) {
    auto snapshot = visualization->takeSnapshot("Test Context");

    EXPECT_EQ(snapshot->m_context, "Test Context");
    EXPECT_FALSE(snapshot->m_resource_stats.empty());
    EXPECT_EQ(snapshot->m_resource_stats["chronons"], "200.000000");
    EXPECT_EQ(snapshot->m_resource_stats["aethel"], "200.000000");
}

// Test visualization of current state
TEST_F(ResourceVisualizationTest, VisualizeCurrentState) {
    // Test TEXT format
    std::string text_viz = visualization->visualizeCurrentState(VisualizationFormat::TEXT);
    EXPECT_FALSE(text_viz.empty());

    // Test JSON format
    std::string json_viz = visualization->visualizeCurrentState(VisualizationFormat::JSON);
    EXPECT_FALSE(json_viz.empty());

    // Test CSV format
    std::string csv_viz = visualization->visualizeCurrentState(VisualizationFormat::CSV);
    EXPECT_FALSE(csv_viz.empty());

    // Test COMPACT format
    std::string compact_viz = visualization->visualizeCurrentState(VisualizationFormat::COMPACT);
    EXPECT_FALSE(compact_viz.empty());
}

// Test generating summary report
TEST_F(ResourceVisualizationTest, GenerateSummaryReport) {
    std::string report = visualization->generateSummaryReport(VisualizationFormat::TEXT);
    EXPECT_FALSE(report.empty());
    EXPECT_NE(report.find("Resource Visualization Summary Report"), std::string::npos);
    EXPECT_NE(report.find("Current Resources"), std::string::npos);
    EXPECT_NE(report.find("Chronons: 200"), std::string::npos);
    EXPECT_NE(report.find("Aethel: 200"), std::string::npos);
}

// Test generating detailed report
TEST_F(ResourceVisualizationTest, GenerateDetailedReport) {
    std::string report = visualization->generateDetailedReport(VisualizationFormat::TEXT, 5);
    EXPECT_FALSE(report.empty());
    EXPECT_NE(report.find("Resource Visualization Detailed Report"), std::string::npos);
    EXPECT_NE(report.find("Current Resources"), std::string::npos);
    EXPECT_NE(report.find("Chronons: 200"), std::string::npos);
    EXPECT_NE(report.find("Aethel: 200"), std::string::npos);

    // Add some debt and check again
    createTestDebts();
    report = visualization->generateDetailedReport(VisualizationFormat::TEXT, 5);
    EXPECT_NE(report.find("Temporal Debt Information"), std::string::npos);
    EXPECT_NE(report.find("Total Chronon Debt: 50"), std::string::npos);
    EXPECT_NE(report.find("Total Aethel Debt: 30"), std::string::npos);
}

// Test real-time monitoring functionality
TEST_F(ResourceVisualizationTest, StartStopMonitoring) {
    bool callback_called = false;
    auto callback = [&callback_called](const std::string&) { callback_called = true; };

    DashboardConfiguration config;
    config.update_frequency_ms = 100;  // Fast updates for testing

    EXPECT_TRUE(visualization->startRealTimeMonitoring(callback, config));

    // Sleep to allow the monitoring thread to update
    std::this_thread::sleep_for(std::chrono::milliseconds(150));

    EXPECT_TRUE(visualization->stopRealTimeMonitoring());
}

// Test dashboard configuration
TEST_F(ResourceVisualizationTest, ConfiguresDashboard) {
    DashboardConfiguration config;
    config.update_frequency_ms = 500;
    config.show_resource_levels = true;
    config.show_debt_status = false;
    config.show_efficiency_metrics = true;
    config.mode = DashboardMode::PERFORMANCE;
    config.history_length = 15;

    visualization->setDashboardConfiguration(config);

    DashboardConfiguration retrieved = visualization->getDashboardConfiguration();
    EXPECT_EQ(retrieved.update_frequency_ms, 500);
    EXPECT_EQ(retrieved.show_resource_levels, true);
    EXPECT_EQ(retrieved.show_debt_status, false);
    EXPECT_EQ(retrieved.show_efficiency_metrics, true);
    EXPECT_EQ(retrieved.mode, DashboardMode::PERFORMANCE);
    EXPECT_EQ(retrieved.history_length, 15);
}

// Test exporting reports
TEST_F(ResourceVisualizationTest, ExportReport) {
    createTestDebts();

    // Create a test file path
    std::string test_file = "test_report.txt";

    // Test TEXT format export
    bool result =
        visualization->exportReport(ReportType::DEBT_IMPACT, VisualizationFormat::TEXT, test_file);
    EXPECT_TRUE(result);

    // Auto-generated filename
    result = visualization->exportReport(ReportType::DEBT_IMPACT, VisualizationFormat::TEXT, "");
    EXPECT_TRUE(result);

    // Clean up
    std::remove(test_file.c_str());
}

// Test snapshot with long context string
TEST_F(ResourceVisualizationTest, LongContextString) {
    std::string longContext(1000, 'A');  // Create a string with 1000 'A's
    auto snapshot = visualization->takeSnapshot(longContext);
    EXPECT_EQ(snapshot->m_context, longContext);
}

// Test null component handling
TEST_F(ResourceVisualizationTest, NullComponentHandling) {
    // Create visualizer with null components
    ResourceVisualization nullVisualizer(static_cast<ResourceTracker*>(nullptr));

    // These should not crash but return empty or default values
    std::string report = nullVisualizer.generateSummaryReport(VisualizationFormat::TEXT);
    EXPECT_NE(report.find("No resource data available"), std::string::npos);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}