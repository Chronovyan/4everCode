#ifndef CHRONOVYAN_GRAPHICAL_RESOURCE_VISUALIZER_H
#define CHRONOVYAN_GRAPHICAL_RESOURCE_VISUALIZER_H

#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "resource_optimizer.h"
#include "resource_visualization.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"

namespace chronovyan {

/**
 * @enum GraphOutputFormat
 * @brief Defines the output format for graphical visualizations
 */
enum class GraphOutputFormat {
    PNG,   // PNG image format
    SVG,   // SVG vector format
    PDF,   // PDF document format
    HTML,  // Interactive HTML format with embedded JavaScript
    JPEG   // JPEG image format
};

/**
 * @enum ChartType
 * @brief Defines the type of chart to generate
 */
enum class ChartType {
    LINE,       // Line chart for time series data
    BAR,        // Bar chart for comparing values
    PIE,        // Pie chart for proportions
    AREA,       // Area chart for cumulative data
    SCATTER,    // Scatter plot for correlations
    HISTOGRAM,  // Histogram for distribution
    HEATMAP,    // Heatmap for 2D density
    GAUGE,      // Gauge for single metric
    SANKEY,     // Sankey diagram for flow visualization
    RADAR       // Radar chart for multi-dimensional data
};

/**
 * @struct ChartOptions
 * @brief Configuration options for chart generation
 */
struct ChartOptions {
    std::string title = "";                     // Chart title
    std::string x_label = "";                   // X-axis label
    std::string y_label = "";                   // Y-axis label
    int width = 800;                            // Chart width in pixels
    int height = 600;                           // Chart height in pixels
    bool show_grid = true;                      // Whether to show grid lines
    bool show_legend = true;                    // Whether to show legend
    std::string background_color = "#ffffff";   // Background color
    std::string text_color = "#000000";         // Text color
    std::map<std::string, std::string> colors;  // Series colors
    double min_y = 0.0;                         // Minimum Y value (auto if not set)
    double max_y = 0.0;                         // Maximum Y value (auto if not set)
    bool y_auto_scale = true;                   // Whether to auto-scale Y axis
    std::string subtitle = "";                  // Chart subtitle
    std::string legend_position = "right";      // Legend position (right, bottom, top, left)
    bool interactive = false;  // Whether chart should be interactive (for HTML format)

    // Constructor with common parameters
    ChartOptions(const std::string& title_val = "", const std::string& x_label_val = "",
                 const std::string& y_label_val = "", int width_val = 800, int height_val = 600)
        : title(title_val)
        , x_label(x_label_val)
        , y_label(y_label_val)
        , width(width_val)
        , height(height_val) {}
};

/**
 * @class GraphicalResourceVisualizer
 * @brief Extends resource visualization with graphical output capabilities
 */
class GraphicalResourceVisualizer {
public:
    /**
     * @brief Create a new graphical resource visualizer
     * @param visualization The base resource visualization component
     */
    GraphicalResourceVisualizer(std::shared_ptr<ResourceVisualization> visualization);

    /**
     * @brief Create a new graphical resource visualizer with direct dependencies
     * @param runtime The temporal runtime to visualize
     * @param optimizer The resource optimizer to use for analysis
     * @param debt_tracker The debt tracker to monitor debt levels
     */
    GraphicalResourceVisualizer(std::shared_ptr<TemporalRuntime> runtime,
                                std::shared_ptr<ResourceOptimizer> optimizer,
                                std::shared_ptr<TemporalDebtTracker> debt_tracker);

    /**
     * @brief Destructor
     */
    ~GraphicalResourceVisualizer();

    /**
     * @brief Generate a resource usage chart
     * @param resource_type The type of resource to visualize
     * @param chart_type The type of chart to generate
     * @param history_length Number of historical data points to include
     * @param options Chart configuration options
     * @param output_format The output format
     * @param output_file The file to save the chart to (empty for in-memory)
     * @return Path to generated file or error message
     */
    std::string generateResourceChart(ResourceType resource_type,
                                      ChartType chart_type = ChartType::LINE,
                                      int history_length = 20,
                                      const ChartOptions& options = ChartOptions(),
                                      GraphOutputFormat output_format = GraphOutputFormat::PNG,
                                      const std::string& output_file = "");

    /**
     * @brief Generate a multi-resource comparison chart
     * @param chart_type The type of chart to generate
     * @param history_length Number of historical data points to include
     * @param options Chart configuration options
     * @param output_format The output format
     * @param output_file The file to save the chart to (empty for in-memory)
     * @return Path to generated file or error message
     */
    std::string generateResourceComparisonChart(
        ChartType chart_type = ChartType::LINE, int history_length = 20,
        const ChartOptions& options = ChartOptions(),
        GraphOutputFormat output_format = GraphOutputFormat::PNG,
        const std::string& output_file = "");

    /**
     * @brief Generate a debt impact chart
     * @param projection_cycles Number of cycles to project into the future
     * @param chart_type The type of chart to generate
     * @param options Chart configuration options
     * @param output_format The output format
     * @param output_file The file to save the chart to (empty for in-memory)
     * @return Path to generated file or error message
     */
    std::string generateDebtImpactChart(int projection_cycles = 10,
                                        ChartType chart_type = ChartType::AREA,
                                        const ChartOptions& options = ChartOptions(),
                                        GraphOutputFormat output_format = GraphOutputFormat::PNG,
                                        const std::string& output_file = "");

    /**
     * @brief Generate a rebel operation impact chart
     * @param operation_type The type of Rebel operation to analyze
     * @param projection_cycles Number of cycles to project into the future
     * @param operations_per_cycle Number of operations performed per cycle
     * @param chart_type The type of chart to generate
     * @param options Chart configuration options
     * @param output_format The output format
     * @param output_file The file to save the chart to (empty for in-memory)
     * @return Path to generated file or error message
     */
    std::string generateRebelOperationImpactChart(
        RebelOperationType operation_type, int projection_cycles = 10, int operations_per_cycle = 1,
        ChartType chart_type = ChartType::LINE, const ChartOptions& options = ChartOptions(),
        GraphOutputFormat output_format = GraphOutputFormat::PNG,
        const std::string& output_file = "");

    /**
     * @brief Generate a resource flow diagram (Sankey chart)
     * @param history_length Number of historical data points to include
     * @param options Chart configuration options
     * @param output_format The output format
     * @param output_file The file to save the chart to (empty for in-memory)
     * @return Path to generated file or error message
     */
    std::string generateResourceFlowDiagram(
        int history_length = 10, const ChartOptions& options = ChartOptions(),
        GraphOutputFormat output_format = GraphOutputFormat::PNG,
        const std::string& output_file = "");

    /**
     * @brief Generate an efficiency metrics dashboard
     * @param options Chart configuration options
     * @param output_format The output format
     * @param output_file The file to save the chart to (empty for in-memory)
     * @return Path to generated file or error message
     */
    std::string generateEfficiencyDashboard(
        const ChartOptions& options = ChartOptions(),
        GraphOutputFormat output_format = GraphOutputFormat::PNG,
        const std::string& output_file = "");

    /**
     * @brief Generate a complete resource dashboard with multiple charts
     * @param options Chart configuration options
     * @param output_format The output format
     * @param output_file The file to save the chart to (empty for in-memory)
     * @return Path to generated file or error message
     */
    std::string generateCompleteDashboard(const ChartOptions& options = ChartOptions(),
                                          GraphOutputFormat output_format = GraphOutputFormat::HTML,
                                          const std::string& output_file = "");

    /**
     * @brief Generate a timeline visualization of resource usage
     * @param history_length Number of historical data points to include
     * @param options Chart configuration options
     * @param output_format The output format
     * @param output_file The file to save the chart to (empty for in-memory)
     * @return Path to generated file or error message
     */
    std::string generateResourceTimeline(int history_length = 20,
                                         const ChartOptions& options = ChartOptions(),
                                         GraphOutputFormat output_format = GraphOutputFormat::PNG,
                                         const std::string& output_file = "");

    /**
     * @brief Generate a resource optimization opportunities chart
     * @param options Chart configuration options
     * @param output_format The output format
     * @param output_file The file to save the chart to (empty for in-memory)
     * @return Path to generated file or error message
     */
    std::string generateOptimizationOpportunitiesChart(
        const ChartOptions& options = ChartOptions(),
        GraphOutputFormat output_format = GraphOutputFormat::PNG,
        const std::string& output_file = "");

    /**
     * @brief Set the default chart options
     * @param options The default options to use
     */
    void setDefaultChartOptions(const ChartOptions& options);

    /**
     * @brief Get the default chart options
     * @return The current default options
     */
    ChartOptions getDefaultChartOptions() const;

    /**
     * @brief Get the file extension for a graph output format
     * @param format The format to get the extension for
     * @return The file extension including the dot (e.g., ".png")
     */
    static std::string getFileExtension(GraphOutputFormat format);

    /**
     * @brief Check if the graphical visualization system is available
     * @return True if the system is available, false otherwise
     */
    static bool isAvailable();

private:
    // The base resource visualization component
    std::shared_ptr<ResourceVisualization> m_visualization;

    // Direct access to core components (via visualization)
    std::shared_ptr<TemporalRuntime> m_runtime;
    std::shared_ptr<ResourceOptimizer> m_optimizer;
    std::shared_ptr<TemporalDebtTracker> m_debt_tracker;

    // Default chart options
    ChartOptions m_default_options;

    // Helper methods for chart generation
    std::string generateChart(const std::vector<std::vector<double>>& data,
                              const std::vector<std::string>& series_names,
                              const std::vector<std::string>& x_labels, ChartType chart_type,
                              const ChartOptions& options, GraphOutputFormat output_format,
                              const std::string& output_file);

    // Data gathering methods
    std::vector<double> getResourceHistory(ResourceType resource_type, int history_length);
    std::vector<double> getDebtProjection(int projection_cycles);
    std::vector<double> getRebelOperationImpact(RebelOperationType operation_type,
                                                int projection_cycles, int operations_per_cycle);
    std::map<std::string, std::vector<double>> getEfficiencyMetrics();

    // File and format handling
    std::string saveChartToFile(const std::string& chart_data, GraphOutputFormat format,
                                const std::string& filename);
    std::string generateFilename(const std::string& prefix, GraphOutputFormat format);

    // Chart generation for different formats
    std::string generatePNGChart(const std::string& chart_data, const std::string& filename);
    std::string generateSVGChart(const std::string& chart_data, const std::string& filename);
    std::string generatePDFChart(const std::string& chart_data, const std::string& filename);
    std::string generateHTMLChart(const std::string& chart_data, const std::string& filename);
    std::string generateJPEGChart(const std::string& chart_data, const std::string& filename);
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_GRAPHICAL_RESOURCE_VISUALIZER_H