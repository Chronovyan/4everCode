#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "gtest/gtest.h"
#include "resource_optimizer.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

using namespace chronovyan;

class DebtVisualizationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create a runtime with some initial resources
        runtime = std::make_shared<TemporalRuntime>();
        runtime->replenishChronons(1000.0);
        runtime->replenishAethel(1000.0);

        // Create a debt tracker
        debt_tracker = std::make_shared<TemporalDebtTracker>(runtime);

        // Create a resource optimizer
        optimizer = std::make_shared<ResourceOptimizer>(runtime, debt_tracker);

        // Create the visualization object
        visualization = std::make_shared<ResourceVisualization>(runtime, optimizer, debt_tracker);

        // Create some test debts
        createTestDebts();
    }

    void createTestDebts() {
        // Create different types of debts to test visualization

        // 1. Small, non-critical chronon debt
        ASSERT_TRUE(debt_tracker->borrowChronons(100.0, "small_chronon_debt", false));

        // 2. Large, critical chronon debt
        ASSERT_TRUE(debt_tracker->borrowChronons(500.0, "large_chronon_debt", true));

        // 3. Medium, non-critical aethel debt
        ASSERT_TRUE(debt_tracker->borrowAethel(200.0, "medium_aethel_debt", false));

        // 4. Small, critical aethel debt
        ASSERT_TRUE(debt_tracker->borrowAethel(50.0, "small_aethel_debt", true));

        // 5. Accrue some rebel operation debts
        ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::REWIND_FLOW, "rewind_debt",
                                                  -1.0, false));
        ASSERT_TRUE(debt_tracker->accrueRebelDebt(RebelOperationType::TEMPORAL_ECHO_LOOP,
                                                  "echo_debt", -1.0, true));
    }

    std::shared_ptr<TemporalRuntime> runtime;
    std::shared_ptr<TemporalDebtTracker> debt_tracker;
    std::shared_ptr<ResourceOptimizer> optimizer;
    std::shared_ptr<ResourceVisualization> visualization;
};

TEST_F(DebtVisualizationTest, VisualizeDebtImpact) {
    // Get the debt impact visualization
    std::string impact = visualization->visualizeDebtImpact();

    // Check that essential sections are present
    ASSERT_NE(impact.find("TEMPORAL DEBT IMPACT ANALYSIS"), std::string::npos);
    ASSERT_NE(impact.find("CURRENT STATUS"), std::string::npos);
    ASSERT_NE(impact.find("DEBT PROJECTION"), std::string::npos);

    // Test with different projection cycles
    std::string impact_5 = visualization->visualizeDebtImpact(5);
    ASSERT_NE(impact_5.find("DEBT PROJECTION OVER 5 CYCLES"), std::string::npos);

    // Test with different formats
    std::string impact_json = visualization->visualizeDebtImpact(10, VisualizationFormat::JSON);
    ASSERT_NE(impact_json.find("debt_impact_analysis"), std::string::npos);

    std::string impact_csv = visualization->visualizeDebtImpact(10, VisualizationFormat::CSV);
    ASSERT_NE(impact_csv.find("Section,Item,Value"), std::string::npos);
}

TEST_F(DebtVisualizationTest, CreateDebtImpactVisualization) {
    // Get the enhanced debt impact visualization
    std::string enhanced_viz = visualization->createDebtImpactVisualization();

    // Check that all standard sections are present
    ASSERT_NE(enhanced_viz.find("ENHANCED DEBT IMPACT VISUALIZATION"), std::string::npos);
    ASSERT_NE(enhanced_viz.find("TEMPORAL DEBT IMPACT ANALYSIS"), std::string::npos);

    // Test with different formats - non-TEXT formats should return the same as visualizeDebtImpact
    std::string enhanced_json =
        visualization->createDebtImpactVisualization(10, VisualizationFormat::JSON);
    std::string json_impact = visualization->visualizeDebtImpact(10, VisualizationFormat::JSON);
    ASSERT_EQ(enhanced_json, json_impact);
}

TEST_F(DebtVisualizationTest, ExportDebtImpactVisualization) {
    // Create a temporary file path for testing
    std::string test_filename = "test_debt_impact_viz.txt";

    // Export the visualization
    bool export_result =
        visualization->exportDebtImpactVisualization(VisualizationFormat::TEXT, 10, test_filename);
    ASSERT_TRUE(export_result);

    // Verify the file exists and has content
    std::ifstream file(test_filename);
    ASSERT_TRUE(file.is_open());

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Check that the file has content
    ASSERT_FALSE(content.empty());

    // Check for key sections in the content
    ASSERT_NE(content.find("ENHANCED DEBT IMPACT VISUALIZATION"), std::string::npos);

    // Clean up
    std::remove(test_filename.c_str());

    // Test export with default filename (should generate a timestamped name)
    export_result = visualization->exportDebtImpactVisualization();
    ASSERT_TRUE(export_result);

    // Note: We don't verify the actual file because its name is dynamically generated with a
    // timestamp
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}