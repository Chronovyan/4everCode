#include <filesystem>
#include <iostream>
#include <memory>
#include <string>

#include "graphical_resource_visualizer.h"
#include "resource_optimizer.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

using namespace chronovyan;

void printHeader(const std::string &title) {
  std::cout << "\n========================================================="
            << std::endl;
  std::cout << title << std::endl;
  std::cout << "========================================================="
            << std::endl;
}

int main(int argc, char *argv[]) {
  std::cout << "Chronovyan Graphical Resource Visualization Demo" << std::endl;
  std::cout << "================================================" << std::endl;

  // Check if graphical visualization is available
  if (!GraphicalResourceVisualizer::isAvailable()) {
    std::cout << "ERROR: Graphical visualization is not available."
              << std::endl;
    std::cout << "This feature requires the Matplot++ library, which was not "
                 "found during build."
              << std::endl;
    std::cout << "Please install Matplot++ and rebuild the project to enable "
                 "graphical visualization."
              << std::endl;
    return 1;
  }

  // Initialize components
  auto runtime = std::make_shared<TemporalRuntime>();
  auto debt_tracker = std::make_shared<TemporalDebtTracker>(runtime);
  auto optimizer = std::make_shared<ResourceOptimizer>(runtime);
  auto visualization =
      std::make_shared<ResourceVisualization>(runtime, optimizer, debt_tracker);

  // Create the graphical visualizer
  auto graphical_viz =
      std::make_shared<GraphicalResourceVisualizer>(visualization);

  // Generate some sample data
  runtime->replenishChronons(350.0);
  runtime->replenishAethel(200.0);

  // Add some sample debts
  debt_tracker->borrowChronons(50.0, "test_operation_1");
  debt_tracker->borrowAethel(25.0, "test_operation_2");
  debt_tracker->accrueRebelDebt(RebelOperationType::REWIND_FLOW,
                                "test_rebel_op");

  // Create output directory if it doesn't exist
  std::filesystem::create_directories("resource_viz_output");

  // 1. Generate resource usage charts
  printHeader("Generating Resource Usage Charts");

  std::string chronon_chart = graphical_viz->generateResourceChart(
      ResourceType::CHRONO_ENERGY, ChartType::LINE, 20,
      ChartOptions("Chronon Usage Over Time", "Time", "Chronons"),
      GraphOutputFormat::PNG, "resource_viz_output/chronon_usage.png");
  std::cout << "Chronon chart saved to: " << chronon_chart << std::endl;

  std::string aethel_chart = graphical_viz->generateResourceChart(
      ResourceType::AETHEL_FLUX, ChartType::AREA, 20,
      ChartOptions("Aethel Flux Over Time", "Time", "Aethel"),
      GraphOutputFormat::PNG, "resource_viz_output/aethel_usage.png");
  std::cout << "Aethel chart saved to: " << aethel_chart << std::endl;

  std::string stability_chart = graphical_viz->generateResourceChart(
      ResourceType::TEMPORAL_STABILITY, ChartType::LINE, 20,
      ChartOptions("Temporal Stability", "Time", "Stability"),
      GraphOutputFormat::PNG, "resource_viz_output/stability.png");
  std::cout << "Stability chart saved to: " << stability_chart << std::endl;

  // 2. Generate resource comparison chart
  printHeader("Generating Resource Comparison Chart");

  std::string comparison_chart = graphical_viz->generateResourceComparisonChart(
      ChartType::LINE, 20,
      ChartOptions("Resource Comparison", "Time", "Resource Level"),
      GraphOutputFormat::PNG, "resource_viz_output/resource_comparison.png");
  std::cout << "Resource comparison chart saved to: " << comparison_chart
            << std::endl;

  // 3. Generate debt impact chart
  printHeader("Generating Debt Impact Chart");

  std::string debt_chart = graphical_viz->generateDebtImpactChart(
      15, ChartType::AREA,
      ChartOptions("Temporal Debt Projection", "Future Cycles", "Debt Level"),
      GraphOutputFormat::PNG, "resource_viz_output/debt_impact.png");
  std::cout << "Debt impact chart saved to: " << debt_chart << std::endl;

  // 4. Generate rebel operation impact charts
  printHeader("Generating Rebel Operation Impact Charts");

  RebelOperationType operations[] = {RebelOperationType::REWIND_FLOW,
                                     RebelOperationType::QUANTUM_MANIPULATION,
                                     RebelOperationType::SUSPEND_TIMELINE,
                                     RebelOperationType::TEMPORAL_ECHO_LOOP,
                                     RebelOperationType::SHATTER_ITERATION};

  for (auto op : operations) {
    std::string op_name;
    switch (op) {
    case RebelOperationType::REWIND_FLOW:
      op_name = "rewind_flow";
      break;
    case RebelOperationType::QUANTUM_MANIPULATION:
      op_name = "quantum_manipulation";
      break;
    case RebelOperationType::SUSPEND_TIMELINE:
      op_name = "suspend_timeline";
      break;
    case RebelOperationType::TEMPORAL_ECHO_LOOP:
      op_name = "temporal_echo_loop";
      break;
    case RebelOperationType::SHATTER_ITERATION:
      op_name = "shatter_iteration";
      break;
    default:
      op_name = "unknown";
    }

    std::string rebel_chart = graphical_viz->generateRebelOperationImpactChart(
        op, 15, 2, ChartType::LINE, ChartOptions("", "", "Impact"),
        GraphOutputFormat::PNG,
        "resource_viz_output/rebel_" + op_name + ".png");
    std::cout << "Rebel operation impact chart for " << op_name
              << " saved to: " << rebel_chart << std::endl;
  }

  // 5. Generate complete dashboard (HTML)
  printHeader("Generating Complete Dashboard");

  std::string dashboard = graphical_viz->generateCompleteDashboard(
      ChartOptions("Chronovyan Resource Dashboard"), GraphOutputFormat::HTML,
      "resource_viz_output/dashboard.html");
  std::cout << "Complete dashboard saved to: " << dashboard << std::endl;

  std::cout << "\nAll visualizations have been generated in the "
               "'resource_viz_output' directory."
            << std::endl;
  std::cout << "You can open these files in any image viewer or web browser "
               "(for HTML files)."
            << std::endl;

  return 0;
}