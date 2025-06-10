#include <iostream>
#include <memory>

#include "resource_visualization.h"

using namespace chronovyan;

int main() {
  std::cout << "Resource Export Demo" << std::endl;

  // Create a resource tracker
  std::shared_ptr<ResourceTracker> tracker =
      std::make_shared<ResourceTracker>();

  // Add some test data
  tracker->recordCurrentUsage(1000.0, 500.0);
  tracker->recordCurrentUsage(1100.0, 600.0);
  tracker->recordCurrentUsage(900.0, 700.0);

  // Create a visualizer
  ResourceVisualization visualizer(tracker.get());

  // Take some snapshots
  for (int i = 0; i < 3; i++) {
    visualizer.takeSnapshot();
  }

  // Generate and display reports
  std::cout << "\n--- Summary Report ---\n";
  std::string summary = visualizer.generateResourceReport(ReportType::SUMMARY);
  std::cout << summary << std::endl;

  std::cout << "\n--- Detailed Report ---\n";
  std::string detailed =
      visualizer.generateResourceReport(ReportType::DETAILED);
  std::cout << detailed << std::endl;

  // Export a report to a file
  std::string filename = "summary_report.txt";
  bool result = visualizer.exportResourceReport(ReportType::SUMMARY, filename);
  std::cout << "\nExport result: " << (result ? "Success" : "Failure")
            << std::endl;

  // Generate a filename
  std::string auto_filename = visualizer.generateReportFilename(
      ReportType::SUMMARY, VisualizationFormat::TEXT);
  std::cout << "Generated filename: " << auto_filename << std::endl;

  return 0;
}