#pragma once
#ifndef CHRONOVYAN_RESOURCE_MANAGEMENT_ADVANCED_RESOURCE_VISUALIZER_H
#define CHRONOVYAN_RESOURCE_MANAGEMENT_ADVANCED_RESOURCE_VISUALIZER_H

#include <array>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "resource_tracker.h"

namespace chronovyan {

/**
 * @enum VisualizationOutputFormat
 * @brief Defines the output format for the advanced visualizations
 */
enum class VisualizationOutputFormat {
  ASCII, // Enhanced ASCII art with colors for terminal
  SVG,   // Scalable Vector Graphics for web or documents
  HTML,  // HTML with embedded charts
  JSON,  // JSON data format for external visualization tools
  PNG    // PNG image format (requires external library)
};

/**
 * @enum ChartType
 * @brief Types of charts that can be generated
 */
enum class ChartType {
  LINE,    // Line chart showing trends over time
  BAR,     // Bar chart for comparing values
  HEATMAP, // Heatmap showing intensity of resource usage
  SCATTER, // Scatter plot for correlation analysis
  AREA,    // Area chart for cumulative trends
  COMBINED // Multiple chart types in one visualization
};

/**
 * @struct ChartConfiguration
 * @brief Configuration options for chart generation
 */
struct ChartConfiguration {
  size_t width = 100;     // Width of the chart in characters or pixels
  size_t height = 30;     // Height of the chart in characters or pixels
  std::string title = ""; // Chart title
  std::vector<std::string> labels = {}; // Labels for data points
  bool show_grid = true;                // Whether to show grid lines
  bool show_legend = true;              // Whether to show the legend
  std::array<const char *, 6> colors = {
      // Default color scheme (ANSI colors for terminal)
      "\033[31m", // Red
      "\033[32m", // Green
      "\033[33m", // Yellow
      "\033[34m", // Blue
      "\033[35m", // Magenta
      "\033[36m"  // Cyan
  };
  const char *reset_color = "\033[0m"; // Reset color code
};

/**
 * @class AdvancedResourceVisualizer
 * @brief Provides advanced visualization capabilities for Chronovyan resources
 *
 * This class extends the basic ResourceVisualizer with more sophisticated
 * visualization techniques, including enhanced ASCII art, SVG generation,
 * and HTML output for interactive charts.
 */
class AdvancedResourceVisualizer {
public:
  /**
   * @brief Construct a new Advanced Resource Visualizer
   * @param tracker Reference to the resource tracker containing data to
   * visualize
   */
  AdvancedResourceVisualizer(const ResourceTracker &tracker);

  /**
   * @brief Generate a visualization of resource usage over time
   * @param format The desired output format
   * @param chart_type The type of chart to generate
   * @param config Configuration options for the chart
   * @return String containing the visualization in the requested format
   */
  std::string generateResourceVisualization(
      VisualizationOutputFormat format = VisualizationOutputFormat::ASCII,
      ChartType chart_type = ChartType::LINE,
      const ChartConfiguration &config = ChartConfiguration()) const;

  /**
   * @brief Generate a visualization specifically for Chronon usage
   * @param format The desired output format
   * @param chart_type The type of chart to generate
   * @param config Configuration options for the chart
   * @return String containing the visualization in the requested format
   */
  std::string generateChronoVisualization(
      VisualizationOutputFormat format = VisualizationOutputFormat::ASCII,
      ChartType chart_type = ChartType::LINE,
      const ChartConfiguration &config = ChartConfiguration()) const;

  /**
   * @brief Generate a visualization specifically for Aethel usage
   * @param format The desired output format
   * @param chart_type The type of chart to generate
   * @param config Configuration options for the chart
   * @return String containing the visualization in the requested format
   */
  std::string generateAethelVisualization(
      VisualizationOutputFormat format = VisualizationOutputFormat::ASCII,
      ChartType chart_type = ChartType::LINE,
      const ChartConfiguration &config = ChartConfiguration()) const;

  /**
   * @brief Generate a combined visualization showing relationship between
   * resources
   * @param format The desired output format
   * @param config Configuration options for the chart
   * @return String containing the visualization in the requested format
   */
  std::string generateCombinedResourceVisualization(
      VisualizationOutputFormat format = VisualizationOutputFormat::ASCII,
      const ChartConfiguration &config = ChartConfiguration()) const;

  /**
   * @brief Generate a heatmap visualization of paradox risk
   * @param format The desired output format
   * @param config Configuration options for the chart
   * @return String containing the visualization in the requested format
   */
  std::string generateParadoxRiskHeatmap(
      VisualizationOutputFormat format = VisualizationOutputFormat::ASCII,
      const ChartConfiguration &config = ChartConfiguration()) const;

  /**
   * @brief Export a visualization to a file
   * @param visualization The visualization string to export
   * @param filename The filename to save the visualization to
   * @param format The format of the visualization
   * @return True if the export was successful
   */
  bool exportVisualization(const std::string &visualization,
                           const std::string &filename,
                           VisualizationOutputFormat format =
                               VisualizationOutputFormat::ASCII) const;

private:
  const ResourceTracker &tracker_;

  // Helper methods for generating different output formats
  std::string generateAsciiChart(const std::vector<double> &values,
                                 ChartType chart_type,
                                 const ChartConfiguration &config) const;

  std::string generateSvgChart(const std::vector<double> &values,
                               ChartType chart_type,
                               const ChartConfiguration &config) const;

  std::string generateHtmlChart(const std::vector<double> &values,
                                ChartType chart_type,
                                const ChartConfiguration &config) const;

  std::string generateJsonData(const std::vector<double> &values,
                               const ChartConfiguration &config) const;

  // Advanced ASCII chart generation methods
  std::string generateAsciiLineChart(const std::vector<double> &values,
                                     const ChartConfiguration &config) const;

  std::string generateAsciiBarChart(const std::vector<double> &values,
                                    const ChartConfiguration &config) const;

  std::string generateAsciiHeatmap(const std::vector<double> &values,
                                   const ChartConfiguration &config) const;

  std::string generateAsciiScatterPlot(const std::vector<double> &x_values,
                                       const std::vector<double> &y_values,
                                       const ChartConfiguration &config) const;

  // Utility methods
  std::vector<size_t> normalizeValues(const std::vector<double> &values,
                                      size_t height) const;

  std::string getColorForValue(double value, double max_value) const;

  std::string
  generateAsciiChartFrame(const std::vector<std::string> &chart_rows,
                          const ChartConfiguration &config) const;

  std::string generateAsciiAxisLabels(const std::vector<double> &values,
                                      const ChartConfiguration &config) const;
};

} // namespace chronovyan

#endif // CHRONOVYAN_RESOURCE_MANAGEMENT_ADVANCED_RESOURCE_VISUALIZER_H