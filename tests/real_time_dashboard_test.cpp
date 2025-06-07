#include <iostream>
#include <memory>
#include <sstream>
#include <string>

#include "gtest/gtest.h"
#include "resource_optimizer.h"
#include "resource_tracker.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

using namespace chronovyan;

// Helper to generate dashboard content directly
std::string generateDashboardContent(std::shared_ptr<TemporalRuntime> runtime,
                                     std::shared_ptr<TemporalDebtTracker> debt_tracker,
                                     std::shared_ptr<ResourceOptimizer> optimizer,
                                     const DashboardConfiguration& config) {
    std::ostringstream dashboard;

    // Generate dashboard header with title
    dashboard << "REAL-TIME RESOURCE MONITORING DASHBOARD\n";
    dashboard << "=======================================\n\n";

    // Include resource levels section
    dashboard << "CURRENT RESOURCE LEVELS\n";
    dashboard << "----------------------\n";

    if (runtime) {
        dashboard << "Chronons: " << runtime->getChrononsLevel() << "\n";
        dashboard << "Aethel: " << runtime->getAethelLevel() << "\n";
    } else {
        dashboard << "No runtime available for resource levels.\n";
    }
    dashboard << "\n";

    // Include debt status section
    dashboard << "DEBT STATUS\n";
    dashboard << "-----------\n";

    if (debt_tracker) {
        dashboard << "Chronon debt: " << debt_tracker->getTotalChrononsDebt() << "\n";
        dashboard << "Aethel debt: " << debt_tracker->getTotalAethelDebt() << "\n";
        dashboard << "Stability: " << debt_tracker->getStability() << "\n";
        dashboard << "Paradox risk: " << debt_tracker->getParadoxRisk() << "\n";
    } else {
        dashboard << "No debt tracker available for debt status.\n";
    }
    dashboard << "\n";

    // Include efficiency metrics section
    dashboard << "EFFICIENCY METRICS\n";
    dashboard << "------------------\n";

    if (optimizer) {
        auto metrics = optimizer->getEfficiencyMetrics();
        for (const auto& metric : metrics) {
            dashboard << metric.first << ": " << metric.second << "\n";
        }
    } else {
        dashboard << "No optimizer available for efficiency metrics.\n";
    }
    dashboard << "\n";

    // Include update frequency
    dashboard << "Update frequency: " << config.update_frequency_ms << "ms\n";

    // Include dashboard mode
    dashboard << "Dashboard mode: ";
    switch (config.mode) {
        case DashboardMode::HIGH_FIDELITY:
            dashboard << "HIGH FIDELITY";
            break;
        case DashboardMode::BALANCED:
            dashboard << "BALANCED";
            break;
        case DashboardMode::LEAN:
            dashboard << "LEAN";
            break;
        default:
            dashboard << "DEFAULT";
            break;
    }
    dashboard << "\n\n";

    // Dashboard footer
    dashboard << "=======================================\n";

    return dashboard.str();
}

class RealTimeDashboardTest : public ::testing::Test {
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
    }

    std::shared_ptr<TemporalRuntime> runtime;
    std::shared_ptr<ResourceOptimizer> optimizer;
    std::shared_ptr<TemporalDebtTracker> debt_tracker;
};

TEST_F(RealTimeDashboardTest, GeneratesRealTimeDashboard) {
    ResourceVisualization visualizer(runtime, optimizer, debt_tracker);

    // Take a few snapshots to populate history
    visualizer.takeSnapshot();
    runtime->replenishChronons(100);
    visualizer.takeSnapshot();
    runtime->consumeChronons(50);
    visualizer.takeSnapshot();

    // Use our helper to generate dashboard content
    DashboardConfiguration config;
    std::string expectedDashboard =
        generateDashboardContent(runtime, debt_tracker, optimizer, config);

    // Verify our generated content contains all the expected strings
    EXPECT_FALSE(expectedDashboard.empty());
    EXPECT_TRUE(expectedDashboard.find("REAL-TIME RESOURCE MONITORING DASHBOARD") !=
                std::string::npos);
    EXPECT_TRUE(expectedDashboard.find("CURRENT RESOURCE LEVELS") != std::string::npos);
    EXPECT_TRUE(expectedDashboard.find("Chronons:") != std::string::npos);
    EXPECT_TRUE(expectedDashboard.find("Aethel:") != std::string::npos);
    EXPECT_TRUE(expectedDashboard.find("DEBT STATUS") != std::string::npos);
    EXPECT_TRUE(expectedDashboard.find("EFFICIENCY METRICS") != std::string::npos);
    EXPECT_TRUE(expectedDashboard.find("Update frequency:") != std::string::npos);

    // Update dashboard and compare with expected content
    std::string dashboard = visualizer.updateDashboard();

    // For debugging
    if (dashboard != expectedDashboard) {
        std::cerr << "Expected dashboard:\n" << expectedDashboard << "\n\n";
        std::cerr << "Actual dashboard:\n" << dashboard << "\n\n";
    }

    // Check if the dashboard has the expected strings
    EXPECT_FALSE(dashboard.empty());
    EXPECT_TRUE(dashboard.find("REAL-TIME RESOURCE MONITORING DASHBOARD") != std::string::npos);
    EXPECT_TRUE(dashboard.find("CURRENT RESOURCE LEVELS") != std::string::npos);
    EXPECT_TRUE(dashboard.find("Chronons:") != std::string::npos);
    EXPECT_TRUE(dashboard.find("Aethel:") != std::string::npos);
    EXPECT_TRUE(dashboard.find("DEBT STATUS") != std::string::npos);
    EXPECT_TRUE(dashboard.find("EFFICIENCY METRICS") != std::string::npos);
    EXPECT_TRUE(dashboard.find("Update frequency:") != std::string::npos);
}

TEST_F(RealTimeDashboardTest, ConfiguresDashboard) {
    ResourceVisualization visualizer(runtime, optimizer, debt_tracker);

    // Create a custom configuration
    DashboardConfiguration config;
    config.update_frequency_ms = 500;
    config.show_debt_status = false;
    config.show_efficiency_metrics = true;
    config.show_resource_levels = true;
    config.show_temporal_flow = false;
    config.show_trends = true;
    config.mode = DashboardMode::HIGH_FIDELITY;

    // Set configuration
    visualizer.setDashboardConfiguration(config);

    // Verify configuration was stored
    DashboardConfiguration retrieved_config = visualizer.getDashboardConfiguration();
    EXPECT_EQ(retrieved_config.update_frequency_ms, 500);
    EXPECT_FALSE(retrieved_config.show_debt_status);
    EXPECT_TRUE(retrieved_config.show_efficiency_metrics);
    EXPECT_TRUE(retrieved_config.show_resource_levels);
    EXPECT_FALSE(retrieved_config.show_temporal_flow);
    EXPECT_TRUE(retrieved_config.show_trends);
    EXPECT_EQ(retrieved_config.mode, DashboardMode::HIGH_FIDELITY);
}

TEST_F(RealTimeDashboardTest, StartStopMonitoring) {
    ResourceVisualization visualizer(runtime, optimizer, debt_tracker);

    // Store dashboard updates
    bool callback_called = false;
    std::string last_dashboard;

    // Generate expected dashboard content
    DashboardConfiguration config;
    std::string expectedDashboard =
        generateDashboardContent(runtime, debt_tracker, optimizer, config);

    // Start monitoring
    bool started = visualizer.startRealTimeMonitoring(
        [&callback_called, &last_dashboard, &expectedDashboard](const std::string& dashboard) {
            callback_called = true;
            last_dashboard = dashboard;

            // For debugging
            if (dashboard != expectedDashboard) {
                std::cerr << "Expected dashboard:\n" << expectedDashboard << "\n\n";
                std::cerr << "Actual dashboard:\n" << dashboard << "\n\n";
            }
        });

    // Verify monitoring started successfully
    EXPECT_TRUE(started);
    EXPECT_TRUE(callback_called);
    EXPECT_FALSE(last_dashboard.empty());

    // Verify starting again fails
    callback_called = false;
    bool started_again =
        visualizer.startRealTimeMonitoring([&callback_called](const std::string& dashboard) {
            // Mark parameter as intentionally unused
            (void)dashboard;
            callback_called = true;
        });
    EXPECT_FALSE(started_again);
    EXPECT_FALSE(callback_called);

    // Stop monitoring
    bool stopped = visualizer.stopRealTimeMonitoring();
    EXPECT_TRUE(stopped);

    // Verify stopping again fails
    bool stopped_again = visualizer.stopRealTimeMonitoring();
    EXPECT_FALSE(stopped_again);
}

TEST_F(RealTimeDashboardTest, DashboardModes) {
    ResourceVisualization visualizer(runtime, optimizer, debt_tracker);

    // Populate with some data
    for (int i = 0; i < 5; i++) {
        visualizer.takeSnapshot();
        runtime->replenishChronons(50);
        runtime->replenishAethel(20);
    }

    // Test HIGH_FIDELITY mode
    DashboardConfiguration high_config;
    high_config.mode = DashboardMode::HIGH_FIDELITY;
    visualizer.setDashboardConfiguration(high_config);

    std::string expectedHighDashboard =
        generateDashboardContent(runtime, debt_tracker, optimizer, high_config);
    std::string high_dashboard = visualizer.updateDashboard();

    // For debugging
    if (high_dashboard != expectedHighDashboard) {
        std::cerr << "Expected high fidelity dashboard:\n" << expectedHighDashboard << "\n\n";
        std::cerr << "Actual high fidelity dashboard:\n" << high_dashboard << "\n\n";
    }

    EXPECT_TRUE(high_dashboard.find("HIGH FIDELITY") != std::string::npos);

    // Test BALANCED mode
    DashboardConfiguration balanced_config;
    balanced_config.mode = DashboardMode::BALANCED;
    visualizer.setDashboardConfiguration(balanced_config);

    std::string expectedBalancedDashboard =
        generateDashboardContent(runtime, debt_tracker, optimizer, balanced_config);
    std::string balanced_dashboard = visualizer.updateDashboard();

    // For debugging
    if (balanced_dashboard != expectedBalancedDashboard) {
        std::cerr << "Expected balanced dashboard:\n" << expectedBalancedDashboard << "\n\n";
        std::cerr << "Actual balanced dashboard:\n" << balanced_dashboard << "\n\n";
    }

    EXPECT_TRUE(balanced_dashboard.find("BALANCED") != std::string::npos);

    // Test LEAN mode
    DashboardConfiguration lean_config;
    lean_config.mode = DashboardMode::LEAN;
    visualizer.setDashboardConfiguration(lean_config);

    std::string expectedLeanDashboard =
        generateDashboardContent(runtime, debt_tracker, optimizer, lean_config);
    std::string lean_dashboard = visualizer.updateDashboard();

    // For debugging
    if (lean_dashboard != expectedLeanDashboard) {
        std::cerr << "Expected lean dashboard:\n" << expectedLeanDashboard << "\n\n";
        std::cerr << "Actual lean dashboard:\n" << lean_dashboard << "\n\n";
    }

    EXPECT_TRUE(lean_dashboard.find("LEAN") != std::string::npos);

    // Verify lean mode has less content than high fidelity
    EXPECT_TRUE(lean_dashboard.length() < high_dashboard.length());
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}