#pragma once
#include <string>
#include <vector>

#include "resource_tracker.h"

namespace chronovyan {

// Class to visualize resource usage in various formats
class ResourceVisualizer {
public:
  // Constructor requires a resource tracker to visualize
  ResourceVisualizer(const ResourceTracker &tracker);

  // Generate ASCII visualization of resource usage over time
  std::string generateAsciiGraph(size_t width = 80, size_t height = 15) const;

  // Generate simple textual summary of resource statistics
  std::string generateTextSummary() const;

  // Generate detailed report with all statistics
  std::string generateDetailedReport() const;

private:
  const ResourceTracker &tracker_;

  // Helper functions for visualization
  std::string generateAsciiHistogram(const std::vector<double> &values,
                                     size_t width, size_t height) const;
  std::string generateAsciiTimeline(const std::vector<ResourceDataPoint> &data,
                                    size_t width) const;

  // Map values to ASCII graph height
  std::vector<size_t> normalizeValues(const std::vector<double> &values,
                                      size_t height) const;

  // Generate ASCII bar for histogram
  std::string generateAsciiBar(size_t height, size_t max_height) const;

  // Generate legend for visualization symbols
  std::string generateLegend() const;
};

} // namespace chronovyan