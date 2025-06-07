#include <iostream>
#include <memory>
#include <string>

#include "../src/resource_management/resource_tracker.h"
#include "../src/resource_management/resource_visualizer.h"
#include "gtest/gtest.h"
#include "resource_optimizer.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

using namespace chronovyan;

// Helper function to populate a tracker with test data
void populateTrackerWithTestData(ResourceTracker& tracker) {
    // Add some test data with increasing, then decreasing pattern
    tracker.recordCurrentUsage(10.0, 5.0, 0.0, 0.1);
    tracker.recordCurrentUsage(20.0, 10.0, 1.0, 0.2);
    tracker.recordCurrentUsage(30.0, 15.0, 2.0, 0.3);
    tracker.recordCurrentUsage(25.0, 12.5, 1.5, 0.25);
    tracker.recordCurrentUsage(15.0, 7.5, 1.0, 0.2);
}

TEST(ResourceVisualizerTest, GeneratesTextSummary) {
    ResourceTracker tracker;
    populateTrackerWithTestData(tracker);

    ResourceVisualizer visualizer(tracker);
    std::string summary = visualizer.generateTextSummary();

    // Verify summary contains expected sections
    EXPECT_TRUE(summary.find("Resource Usage Summary") != std::string::npos);
    EXPECT_TRUE(summary.find("Current Chronon Usage:") != std::string::npos);
    EXPECT_TRUE(summary.find("Current Aethel Usage:") != std::string::npos);
    EXPECT_TRUE(summary.find("Current Temporal Debt:") != std::string::npos);

    // Verify summary contains the latest values (should be the last ones added)
    EXPECT_TRUE(summary.find("15.00") != std::string::npos);
    EXPECT_TRUE(summary.find("7.50") != std::string::npos);
    EXPECT_TRUE(summary.find("1.00") != std::string::npos);
}

TEST(ResourceVisualizerTest, GeneratesDetailedReport) {
    ResourceTracker tracker;
    populateTrackerWithTestData(tracker);

    ResourceVisualizer visualizer(tracker);
    std::string report = visualizer.generateDetailedReport();

    // Verify report contains expected sections
    EXPECT_TRUE(report.find("Chronovyan Resource Detailed Report") != std::string::npos);
    EXPECT_TRUE(report.find("CURRENT VALUES:") != std::string::npos);
    EXPECT_TRUE(report.find("HISTORICAL AVERAGES:") != std::string::npos);
    EXPECT_TRUE(report.find("MAXIMUM VALUES:") != std::string::npos);
    EXPECT_TRUE(report.find("EFFICIENCY INDICATORS:") != std::string::npos);

    // Print the report for debugging
    std::cout << "Report content: " << report << std::endl;

    // Verify report contains correct statistics (check for general patterns, not exact formatting)
    EXPECT_TRUE(report.find("Max Chronon") != std::string::npos);
    EXPECT_TRUE(report.find("30.00") != std::string::npos);
    EXPECT_TRUE(report.find("Max Aethel") != std::string::npos);
    EXPECT_TRUE(report.find("15.00") != std::string::npos);
    EXPECT_TRUE(report.find("Max Temporal") != std::string::npos);
    EXPECT_TRUE(report.find("2.00") != std::string::npos);

    // Verify report shows correct data point count
    EXPECT_TRUE(report.find("Data points in history: 5") != std::string::npos);
}

TEST(ResourceVisualizerTest, GeneratesAsciiGraph) {
    ResourceTracker tracker;
    populateTrackerWithTestData(tracker);

    ResourceVisualizer visualizer(tracker);
    std::string graph = visualizer.generateAsciiGraph(40, 10);

    // Verify graph contains expected sections
    EXPECT_TRUE(graph.find("Chronovyan Resource Visualization") != std::string::npos);
    EXPECT_TRUE(graph.find("Chronon Usage:") != std::string::npos);
    EXPECT_TRUE(graph.find("Aethel Usage:") != std::string::npos);
    EXPECT_TRUE(graph.find("Resource Timeline:") != std::string::npos);

    // Basic verification that graph contains visual elements
    EXPECT_TRUE(graph.find("|") != std::string::npos);
    EXPECT_TRUE(graph.find("+") != std::string::npos);
    EXPECT_TRUE(graph.find("-") != std::string::npos);
}

TEST(ResourceVisualizerTest, HandlesEmptyTracker) {
    ResourceTracker tracker;
    ResourceVisualizer visualizer(tracker);

    // Verify all visualization methods handle empty data gracefully
    std::string summary = visualizer.generateTextSummary();
    std::string report = visualizer.generateDetailedReport();
    std::string graph = visualizer.generateAsciiGraph();

    EXPECT_TRUE(summary.find("No data available") != std::string::npos);
    EXPECT_TRUE(report.find("No data available") != std::string::npos);
    EXPECT_TRUE(graph.find("No data available") != std::string::npos);
}

TEST(ResourceVisualizerTest, GeneratesAsciiTimeline) {
    ResourceTracker tracker;
    populateTrackerWithTestData(tracker);

    ResourceVisualizer visualizer(tracker);
    std::string graph = visualizer.generateAsciiGraph();

    // Check for timeline directional indicators
    EXPECT_TRUE(graph.find("Chronon:") != std::string::npos);
    EXPECT_TRUE(graph.find("Aethel:") != std::string::npos);

    // We should see at least one of these direction symbols in the output
    bool found_direction_symbol = graph.find("/") != std::string::npos ||
                                  graph.find("\\") != std::string::npos ||
                                  graph.find("-") != std::string::npos;

    EXPECT_TRUE(found_direction_symbol);
}

class ResourceVisualizationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create runtime and related objects
        runtime = std::make_shared<TemporalRuntime>();
        debt_tracker = std::make_shared<TemporalDebtTracker>(runtime);

        // Create resource tracker
        auto resource_tracker = std::make_shared<ResourceTracker>();

        // Create optimizer with both required parameters
        optimizer = std::make_shared<ResourceOptimizer>(runtime, debt_tracker);

        // Set up initial state
        runtime->replenishChronons(1000);
        runtime->replenishAethel(500);

        // Set up resource tracker
        tracker.recordCurrentUsage(1000, 500);
    }

    std::shared_ptr<TemporalRuntime> runtime;
    std::shared_ptr<ResourceOptimizer> optimizer;
    std::shared_ptr<TemporalDebtTracker> debt_tracker;
    ResourceTracker tracker;
};

TEST_F(ResourceVisualizationTest, TakesSnapshot) {
    ResourceVisualization visualizer(runtime, optimizer, debt_tracker);
    auto snapshot_ptr = visualizer.takeSnapshot();

    ASSERT_TRUE(snapshot_ptr);

    const auto& stats = snapshot_ptr->m_resource_stats;

    auto chronons_it = stats.find("CHRONONS_LEVEL");
    ASSERT_NE(chronons_it, stats.end()) << "CHRONONS_LEVEL not found in snapshot stats";
    try {
        EXPECT_DOUBLE_EQ(std::stod(chronons_it->second), 1000.0);
    } catch (const std::exception& e) {
        ADD_FAILURE() << "Exception converting CHRONONS_LEVEL: " << e.what() << " (value: '"
                      << chronons_it->second << "')";
    }

    auto aethel_it = stats.find("AETHEL_LEVEL");
    ASSERT_NE(aethel_it, stats.end()) << "AETHEL_LEVEL not found in snapshot stats";
    try {
        EXPECT_DOUBLE_EQ(std::stod(aethel_it->second), 500.0);
    } catch (const std::exception& e) {
        ADD_FAILURE() << "Exception converting AETHEL_LEVEL: " << e.what() << " (value: '"
                      << aethel_it->second << "')";
    }

    auto cycle_it = stats.find("CURRENT_CYCLE");
    ASSERT_NE(cycle_it, stats.end()) << "CURRENT_CYCLE not found in snapshot stats";
    try {
        EXPECT_EQ(std::stoll(cycle_it->second), 0LL);
    } catch (const std::exception& e) {
        ADD_FAILURE() << "Exception converting CURRENT_CYCLE: " << e.what() << " (value: '"
                      << cycle_it->second << "')";
    }
}

TEST_F(ResourceVisualizationTest, VisualizesCurrentState) {
    ResourceVisualization visualizer(runtime, optimizer, debt_tracker);
    std::string state = visualizer.visualizeCurrentState();

    // Only print during debugging
    // std::cout << "Actual state output:\n" << state << std::endl;

    EXPECT_FALSE(state.empty());
    EXPECT_TRUE(state.find("Resource Levels") != std::string::npos);
    EXPECT_TRUE(state.find("Chronons:") != std::string::npos);
    EXPECT_TRUE(state.find("Aethel:") != std::string::npos);
    EXPECT_TRUE(state.find("Debt Status") != std::string::npos);
    EXPECT_TRUE(state.find("Efficiency Metrics") != std::string::npos);
    EXPECT_TRUE(state.find("System Status") != std::string::npos);
    EXPECT_TRUE(state.find("1100") != std::string::npos);  // Chronon level
    EXPECT_TRUE(state.find("600") != std::string::npos);   // Aethel level
}

TEST_F(ResourceVisualizationTest, VisualizesResourceTrends) {
    ResourceVisualization visualizer(runtime, optimizer, debt_tracker);

    // Take multiple snapshots with changing values
    visualizer.takeSnapshot();

    runtime->replenishChronons(100);
    visualizer.takeSnapshot();

    runtime->consumeChronons(200);
    visualizer.takeSnapshot();

    // Visualize trends
    std::string trends = visualizer.visualizeTrends(3);

    EXPECT_FALSE(trends.empty());
    EXPECT_TRUE(trends.find("RESOURCE TRENDS") != std::string::npos);
    EXPECT_TRUE(trends.find("Chronon Levels") != std::string::npos);
    EXPECT_TRUE(trends.find("Aethel Levels") != std::string::npos);
}

TEST_F(ResourceVisualizationTest, VisualizesAethelEfficiencyMetrics) {
    ResourceVisualization visualizer(runtime, optimizer, debt_tracker);

    // Simulate some optimization operations to generate metrics
    optimizer->optimizeAethel("timeline_1");
    optimizer->optimizeChronons("timeline_2");

    std::string metrics = visualizer.visualizeAethelEfficiencyMetrics();

    EXPECT_FALSE(metrics.empty());
    EXPECT_TRUE(metrics.find("AETHEL EFFICIENCY METRICS") != std::string::npos);
}

TEST_F(ResourceVisualizationTest, GeneratesCompleteDashboard) {
    ResourceVisualization visualizer(runtime, optimizer, debt_tracker);

    // Take some snapshots
    visualizer.takeSnapshot();

    runtime->replenishChronons(200);
    runtime->replenishAethel(100);
    visualizer.takeSnapshot();

    runtime->consumeChronons(50);
    visualizer.takeSnapshot();

    // Generate dashboard
    std::string dashboard = visualizer.generateDashboard();

    EXPECT_FALSE(dashboard.empty());
    EXPECT_TRUE(dashboard.find("RESOURCE DASHBOARD") != std::string::npos);
    EXPECT_TRUE(dashboard.find("CURRENT STATE") != std::string::npos);
    EXPECT_TRUE(dashboard.find("TRENDS") != std::string::npos);
    EXPECT_TRUE(dashboard.find("AETHEL EFFICIENCY METRICS") != std::string::npos);
}

TEST_F(ResourceVisualizationTest, VisualizesTemporalResourceFlow) {
    // Create visualization object with our tracker
    ResourceVisualization viz(&tracker);

    // Generate some history by performing operations
    tracker.recordCurrentUsage(1200, 500);
    viz.takeSnapshot();

    tracker.recordCurrentUsage(1050, 500);
    viz.takeSnapshot();

    tracker.recordCurrentUsage(1050, 600);
    viz.takeSnapshot();

    tracker.recordCurrentUsage(1000, 600);
    viz.takeSnapshot();

    tracker.recordCurrentUsage(1300, 600);
    viz.takeSnapshot();

    // Visualize the flow
    std::string flow = viz.visualizeTemporalResourceFlow(5);

    EXPECT_FALSE(flow.empty());
    EXPECT_TRUE(flow.find("TEMPORAL RESOURCE FLOW") != std::string::npos);
    EXPECT_TRUE(flow.find("Chronons") != std::string::npos);
    EXPECT_TRUE(flow.find("Aethel") != std::string::npos);
}

TEST_F(ResourceVisualizationTest, VisualizesPredictiveUsage) {
    // Create visualization object with our tracker
    ResourceVisualization viz(&tracker);

    // Generate data with a clear trend
    tracker.recordCurrentUsage(1000, 500, 0, 0.1);
    viz.takeSnapshot();

    tracker.recordCurrentUsage(950, 520, 10, 0.15);
    viz.takeSnapshot();

    tracker.recordCurrentUsage(900, 540, 20, 0.2);
    viz.takeSnapshot();

    tracker.recordCurrentUsage(850, 560, 30, 0.25);
    viz.takeSnapshot();

    tracker.recordCurrentUsage(800, 580, 40, 0.3);
    viz.takeSnapshot();

    // Generate predictive analytics
    std::string predictions = viz.visualizePredictiveUsage(5, 0.9);

    // Basic validation
    EXPECT_FALSE(predictions.empty());
    EXPECT_TRUE(predictions.find("PREDICTIVE RESOURCE ANALYTICS") != std::string::npos);
    EXPECT_TRUE(predictions.find("TREND ANALYSIS") != std::string::npos);
    EXPECT_TRUE(predictions.find("PROJECTED RESOURCE STATES") != std::string::npos);
    EXPECT_TRUE(predictions.find("PREDICTIVE INSIGHTS") != std::string::npos);
    EXPECT_TRUE(predictions.find("RECOMMENDED ACTIONS") != std::string::npos);

    // Verify trend detection
    EXPECT_TRUE(predictions.find("Chronon Trend") != std::string::npos);
    EXPECT_TRUE(predictions.find("Aethel Trend") != std::string::npos);
    EXPECT_TRUE(predictions.find("Debt Trend") != std::string::npos);
    EXPECT_TRUE(predictions.find("Stability Trend") != std::string::npos);

    // Verify that trend direction is recognized correctly
    EXPECT_TRUE(predictions.find("Chronon levels are decreasing") != std::string::npos);
    EXPECT_TRUE(predictions.find("Aethel production is exceeding") != std::string::npos);
    EXPECT_TRUE(predictions.find("Temporal debt is increasing") != std::string::npos);

    // Test different format
    std::string jsonPredictions = viz.visualizePredictiveUsage(3, 0.8, VisualizationFormat::JSON);
    EXPECT_TRUE(jsonPredictions.find("\"predictive_analysis\"") != std::string::npos);
    EXPECT_TRUE(jsonPredictions.find("\"projections\"") != std::string::npos);

    // Test insufficient data case
    ResourceTracker emptyTracker;
    ResourceVisualization emptyViz(&emptyTracker);
    std::string emptyPredictions = emptyViz.visualizePredictiveUsage();
    EXPECT_TRUE(emptyPredictions.find("Insufficient historical data") != std::string::npos);
}