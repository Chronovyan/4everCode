#ifndef CHRONOVYAN_RESOURCE_VISUALIZATION_H
#define CHRONOVYAN_RESOURCE_VISUALIZATION_H

#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <variant>
#include <vector>

#include "../src/resource_management/resource_tracker.h"
#include "rebel_operation.h"
#include "resource_optimizer.h"
#include "temporal_debt_tracker.h"
#include "temporal_runtime.h"
// Forward declare GraphicalResourceVisualizer if its full definition isn't needed here
// Or ensure its actual header is included if methods are called on it within this header.
// For now, assuming forward declaration is what was intended based on cpp usage.
namespace chronovyan {
class GraphicalResourceVisualizer;  // Forward declaration
}

namespace chronovyan {

// Forward declaration for ResourceType
enum class ResourceType { CHRONO_ENERGY, AETHEL_FLUX, TEMPORAL_STABILITY };

/**
 * @enum VisualizationFormat
 * @brief Defines the output format for resource visualizations
 */
enum class VisualizationFormat {
    TEXT,     // Simple text output
    JSON,     // JSON formatted output for web interfaces
    CSV,      // CSV formatted output for spreadsheets
    COMPACT,  // Compact single-line representation
    HTML      // HTML formatted output
};

/**
 * @enum ThemeType
 * @brief Predefined visualization theme types
 */
enum class ThemeType {
    DEFAULT,    // Standard theme with neutral colors and symbols
    DARK,       // Dark theme with bright text on dark background
    LIGHT,      // Light theme with dark text on light background
    MINIMAL,    // Minimal theme with simple characters and no decorations
    ELABORATE,  // Elaborate theme with detailed decorations and symbols
    CUSTOM      // Custom user-defined theme
};

/**
 * @struct VisualizationTheme
 * @brief Defines visual appearance settings for resource visualizations
 */
struct VisualizationTheme {
    // Text color settings for TEXT and HTML formats
    std::string title_color = "blue";      // Title text color
    std::string header_color = "cyan";     // Header text color
    std::string text_color = "default";    // Normal text color
    std::string positive_color = "green";  // Color for positive values/increases
    std::string negative_color = "red";    // Color for negative values/decreases
    std::string warning_color = "yellow";  // Color for warning conditions
    std::string critical_color = "red";    // Color for critical conditions
    std::string text_muted = "gray";       // Muted/dimmed text color

    /**
     * @struct ASCIIChars
     * @brief Nested structure for ASCII visualization characters
     */
    struct ASCIIChars {
        char bar_character = '#';            // Character for bars in histograms
        char border_horizontal = '-';        // Horizontal border character
        char border_vertical = '|';          // Vertical border character
        char border_corner = '+';            // Corner border character
        char axis_marker = '+';              // Axis marker character
        char data_point = '*';               // Data point character for charts
        char y_axis = '|';                   // Y-axis character for charts
        char x_axis = '-';                   // X-axis character for charts
        std::string uptrend_marker = "^";    // Symbol for upward trends
        std::string downtrend_marker = "v";  // Symbol for downward trends
        std::string stable_marker = ">";     // Symbol for stable trends
        char filled_gauge = '#';             // Character to fill gauges
        char empty_gauge = '-';              // Character for empty gauge portions
    };

    // ASCII visualization characters
    ASCIIChars ascii_chars;

    // For backward compatibility - these duplicate the nested structure fields
    // NOT using references to avoid assignment operator issues
    char bar_character() const { return ascii_chars.bar_character; }
    void bar_character(char c) { ascii_chars.bar_character = c; }

    char border_horizontal() const { return ascii_chars.border_horizontal; }
    void border_horizontal(char c) { ascii_chars.border_horizontal = c; }

    char border_vertical() const { return ascii_chars.border_vertical; }
    void border_vertical(char c) { ascii_chars.border_vertical = c; }

    char border_corner() const { return ascii_chars.border_corner; }
    void border_corner(char c) { ascii_chars.border_corner = c; }

    char axis_marker() const { return ascii_chars.axis_marker; }
    void axis_marker(char c) { ascii_chars.axis_marker = c; }

    std::string uptrend_marker() const { return ascii_chars.uptrend_marker; }
    void uptrend_marker(const std::string& s) { ascii_chars.uptrend_marker = s; }

    std::string downtrend_marker() const { return ascii_chars.downtrend_marker; }
    void downtrend_marker(const std::string& s) { ascii_chars.downtrend_marker = s; }

    std::string stable_marker() const { return ascii_chars.stable_marker; }
    void stable_marker(const std::string& s) { ascii_chars.stable_marker = s; }

    char gauge_fill() const { return ascii_chars.filled_gauge; }
    void gauge_fill(char c) { ascii_chars.filled_gauge = c; }

    char gauge_empty() const { return ascii_chars.empty_gauge; }
    void gauge_empty(char c) { ascii_chars.empty_gauge = c; }

    // Text formatting
    bool use_unicode = true;  // Use Unicode characters if available
    bool use_colors = true;   // Use colors in output if available
    bool use_bold = true;     // Use bold text for emphasis
    int indentation = 2;      // Default indentation level

    // HTML theme settings
    std::string html_background_color = "#ffffff";   // Background color for HTML
    std::string html_text_color = "#000000";         // Text color for HTML
    std::string html_header_background = "#f0f0f0";  // Header background for HTML
    std::string html_accent_color = "#4285f4";       // Accent color for HTML

    // Creates a theme of the specified type
    static VisualizationTheme createTheme(ThemeType type);
};

/**
 * @enum DashboardMode
 * @brief Defines the performance mode for the real-time dashboard
 */
enum class DashboardMode {
    HIGH_FIDELITY,  // Full detail, all animations, real-time updates
    BALANCED,       // Moderate detail, selective animations, adaptive updates
    LEAN,           // Minimal detail, static or slow-updating visuals
    AUTO,           // Dynamically switch modes based on system metrics
    MINIMAL,        // Show only essential information
    PERFORMANCE     // Focus on performance metrics
};

/**
 * @enum ReportType
 * @brief Defines the type of resource usage report to generate
 */
enum class ReportType {
    SUMMARY,            // Basic summary of current resource usage
    DETAILED,           // Detailed report with all metrics and historical data
    EFFICIENCY,         // Focus on efficiency metrics and optimization opportunities
    DEBT_ANALYSIS,      // Detailed analysis of temporal debt
    DEBT_IMPACT,        // Debt impact over time with repayment strategies
    REBEL_IMPACT,       // Analysis of Rebel operation impact on debt and stability
    HISTORICAL_TRENDS,  // Analysis of resource usage trends over time
    COMPLETE            // Complete report with all available information
};

/**
 * @struct ResourceSnapshot
 * @brief Contains a snapshot of resource metrics at a point in time
 */
struct ResourceSnapshot {
    // Members expected by the .cpp implementation
    std::string m_context;
    std::chrono::time_point<std::chrono::system_clock> m_timestamp;
    std::map<std::string, std::string> m_resource_stats;

    // Static takeSnapshot signature to match the .cpp implementation
    static ResourceSnapshot takeSnapshot(
        std::shared_ptr<TemporalRuntime> runtime, std::shared_ptr<TemporalDebtTracker> debtTracker,
        const std::string& context, std::shared_ptr<ResourceOptimizer> optimizer,
        std::shared_ptr<GraphicalResourceVisualizer> graphVisualizer);
};

/**
 * @struct DashboardConfiguration
 * @brief Configuration options for the real-time dashboard
 */
struct DashboardConfiguration {
    // Update frequency in milliseconds
    int update_frequency_ms = 1000;

    // Which components to display
    bool show_resource_levels = true;
    bool show_debt_status = true;
    bool show_efficiency_metrics = true;
    bool show_trends = true;
    bool show_temporal_flow = false;

    // Dashboard display mode
    DashboardMode mode = DashboardMode::BALANCED;

    // Number of historical data points to display
    int history_length = 20;

    // Dashboard dimensions
    int width = 80;
    int height = 20;
};

// Forward declarations
class TemporalRuntime;
class ResourceOptimizer;
class TemporalDebtTracker;
enum class RebelOperationType;

/**
 * @class ResourceVisualization
 * @brief Provides visualization capabilities for resource usage and temporal metrics
 */
class ResourceVisualization {
public:
    /**
     * @brief Construct a new Resource Visualization object
     * @param resourceTracker The resource tracker to visualize
     * @param runtime The temporal runtime to visualize
     * @param debtTracker The temporal debt tracker to visualize
     */
    ResourceVisualization(std::shared_ptr<ResourceTracker> resourceTracker,
                          std::shared_ptr<TemporalRuntime> runtime,
                          std::shared_ptr<TemporalDebtTracker> debtTracker);

    /**
     * @brief Create a resource visualization component with runtime, optimizer and debt tracker
     * @param runtime The temporal runtime to visualize
     * @param optimizer The resource optimizer to use
     * @param debtTracker The temporal debt tracker to visualize
     */
    ResourceVisualization(std::shared_ptr<TemporalRuntime> runtime,
                          std::shared_ptr<ResourceOptimizer> optimizer,
                          std::shared_ptr<TemporalDebtTracker> debtTracker);

    /**
     * @brief Create a resource visualization component with runtime, optimizer, debt tracker, and
     * graph visualizer
     * @param runtime The temporal runtime to visualize
     * @param optimizer The resource optimizer to use
     * @param debtTracker The temporal debt tracker to visualize
     * @param graph_visualizer The graphical resource visualizer to use
     */
    ResourceVisualization(std::shared_ptr<TemporalRuntime> runtime,
                          std::shared_ptr<TemporalDebtTracker> debt_tracker,
                          std::shared_ptr<ResourceOptimizer> optimizer,
                          std::shared_ptr<GraphicalResourceVisualizer> graph_visualizer);

    /**
     * @brief Generate a text-based resource usage report
     * @return A formatted string with resource usage information
     */
    std::string generateTextReport() const;

    /**
     * @brief Generate a visual graph of resource usage (if graphical libraries available)
     * @param outputFile The file to save the graph to
     * @return True if successful, false otherwise
     */
    bool generateResourceGraph(const std::string& outputFile) const;

    /**
     * @brief Generate a timeline visualization of operations
     * @param outputFile The file to save the timeline to
     * @return True if successful, false otherwise
     */
    bool generateTimelineVisualization(const std::string& outputFile) const;

    /**
     * @brief Create a new resource visualization component with a resource tracker
     * @param tracker The resource tracker to visualize
     */
    ResourceVisualization(ResourceTracker* tracker);

    /**
     * @brief Destructor to ensure monitoring thread is stopped
     */
    ~ResourceVisualization();

    /**
     * @brief Visualize the current resource state
     * @param format The output format
     * @return Formatted string representation
     */
    std::string visualizeCurrentState(VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Visualize the current debt status
     * @param format The output format
     * @return Formatted string representation
     */
    std::string visualizeDebtStatus(VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Visualize the impact of temporal debt on system stability
     * @param projection_cycles Number of cycles to project into the future
     * @param format The output format
     * @return Formatted string representation
     */
    std::string visualizeDebtImpact(int projection_cycles = 10,
                                    VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Create an enhanced visualization of debt impact with additional charts
     * @param projection_cycles Number of cycles to project into the future
     * @param format The output format
     * @return Formatted string representation with charts and projections
     */
    std::string createDebtImpactVisualization(
        int projection_cycles = 10, VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Export the debt impact visualization to a file
     * @param format The output format
     * @param projection_cycles Number of cycles to project into the future
     * @param filename The filename to export to (empty for auto-generated)
     * @return True if export was successful
     */
    bool exportDebtImpactVisualization(VisualizationFormat format = VisualizationFormat::TEXT,
                                       int projection_cycles = 10,
                                       const std::string& filename = "");

    /**
     * @brief Visualize predictive resource usage
     * @param projection_cycles Number of cycles to project into the future
     * @param confidence_level Confidence level for predictions (0.0-1.0)
     * @param format The output format
     * @return Formatted string representation
     */
    std::string visualizePredictiveUsage(int projection_cycles = 10, double confidence_level = 0.8,
                                         VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Visualize the impact of rebel operations
     * @param operation_type The type of rebel operation to analyze
     * @param projection_cycles Number of cycles to project into the future
     * @param operations_per_cycle Number of operations per cycle
     * @param format The output format
     * @return Formatted string representation
     */
    std::string visualizeRebelOperationImpact(
        RebelOperationType operation_type, int projection_cycles = 10, int operations_per_cycle = 1,
        VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Export rebel operation impact visualization to a file
     * @param operation_type The type of rebel operation to analyze
     * @param format The output format
     * @param projection_cycles Number of cycles to project into the future
     * @param operations_per_cycle Number of operations per cycle
     * @param filename The filename to export to (empty for auto-generated)
     * @return True if export was successful
     */
    bool exportRebelOperationImpact(RebelOperationType operation_type,
                                    VisualizationFormat format = VisualizationFormat::TEXT,
                                    int projection_cycles = 10, int operations_per_cycle = 1,
                                    const std::string& filename = "");

    /**
     * @brief Export rebel operation impact report to a file
     * @param operation_type The type of rebel operation to analyze
     * @param format The output format
     * @param projection_cycles Number of cycles to project into the future
     * @param operations_per_cycle Number of operations per cycle
     * @param filename The filename to export to (empty for auto-generated)
     * @return True if export was successful
     */
    bool exportRebelOperationImpactReport(RebelOperationType operation_type,
                                          VisualizationFormat format = VisualizationFormat::TEXT,
                                          int projection_cycles = 10, int operations_per_cycle = 1,
                                          const std::string& filename = "");

    /**
     * @brief Generate a comprehensive dashboard with all metrics
     * @param format The output format
     * @return Formatted string representation
     */
    std::string generateDashboard(VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Visualize aethel efficiency metrics
     * @param format The output format
     * @return Formatted string representation
     */
    std::string visualizeAethelEfficiencyMetrics(
        VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Visualize temporal resource flow
     * @param steps Number of time steps to visualize
     * @param format The output format
     * @return Formatted string representation
     */
    std::string visualizeTemporalResourceFlow(
        int steps = 10, VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Start real-time monitoring of resources
     * @param update_callback Callback function to handle updates
     * @param config Dashboard configuration
     * @return True if monitoring was started
     */
    bool startRealTimeMonitoring(std::function<void(const std::string&)> update_callback,
                                 const DashboardConfiguration& config = DashboardConfiguration());

    /**
     * @brief Stop real-time monitoring
     */
    bool stopRealTimeMonitoring();

    /**
     * @brief Update and return the current dashboard
     * @param format The output format
     * @return Formatted dashboard
     */
    std::string updateDashboard(VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Set the dashboard configuration
     * @param config The configuration to set
     */
    void setDashboardConfiguration(const DashboardConfiguration& config);

    /**
     * @brief Get the current dashboard configuration
     * @return The current dashboard configuration
     */
    DashboardConfiguration getDashboardConfiguration() const;

    /**
     * @brief Clear the history of resource snapshots
     */
    void clearHistory();

    /**
     * @brief Visualize historical trends
     * @param steps Number of historical steps to visualize
     * @param format The output format
     * @return Formatted string representation
     */
    std::string visualizeTrends(int steps = 10,
                                VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Visualize optimization opportunities
     * @param format The output format
     * @return Formatted string representation
     */
    std::string visualizeOptimizationOpportunities(
        VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Generate a report of a specific type
     * @param report_type The type of report to generate
     * @param format The output format
     * @param history_length Number of historical data points to include
     * @return Formatted report
     */
    std::string generateResourceReport(ReportType report_type,
                                       VisualizationFormat format = VisualizationFormat::TEXT,
                                       int history_length = 20);

    /**
     * @brief Export a report to a file
     * @param report_type The type of report
     * @param filename The filename to export to
     * @param format The output format
     * @param history_length Number of historical data points to include
     * @return True if export was successful
     */
    bool exportResourceReport(ReportType report_type, const std::string& filename,
                              VisualizationFormat format = VisualizationFormat::TEXT,
                              int history_length = 20);

    /**
     * @brief Export all resource data to various files
     * @param directory The directory to export to
     * @param format The output format
     * @return True if export was successful
     */
    bool exportAllResourceData(const std::string& directory = "resource_export",
                               VisualizationFormat format = VisualizationFormat::TEXT);

    /**
     * @brief Export a specific visualization to a file
     * @param visualization_type The type of visualization to export
     * @param filename The filename to export to
     * @param format The output format
     * @param params Additional parameters for the visualization
     * @return True if export was successful
     */
    bool exportVisualization(int visualization_type, const std::string& filename,
                             VisualizationFormat format = VisualizationFormat::TEXT,
                             const std::map<std::string, std::string>& params = {});

    /**
     * @brief Generate a filename for a report
     * @param report_type The type of report
     * @param format The output format
     * @return Generated filename
     */
    std::string generateReportFilename(ReportType report_type, VisualizationFormat format) const;

    /**
     * @brief Export a report to a file
     * @param report_type The type of report
     * @param format The output format
     * @param filename The filename to export to
     * @return True if export was successful
     */
    bool exportReport(ReportType report_type [[maybe_unused]],
                      VisualizationFormat format [[maybe_unused]], const std::string& filename);

    /**
     * @brief Export a debt impact report to a file
     * @param format The output format
     * @param projection_cycles Number of cycles to project into the future
     * @param filename The filename to export to (empty for auto-generated)
     * @return True if export was successful
     */
    bool exportDebtImpactReport(VisualizationFormat format = VisualizationFormat::TEXT,
                                int projection_cycles = 10, const std::string& filename = "");

    /**
     * @brief Set the visualization theme
     * @param theme The theme to set
     */
    void setVisualizationTheme(const VisualizationTheme& theme);

    /**
     * @brief Get the current visualization theme
     * @return The current theme
     */
    VisualizationTheme getVisualizationTheme() const;

    /**
     * @brief Set the theme type
     * @param type The theme type to set
     */
    void setThemeType(ThemeType type);

    /**
     * @brief Apply theme color to text
     * @param text The text to colorize
     * @param color_name The color name from theme
     * @param use_bold Whether to use bold formatting
     * @return The colorized text
     */
    std::string applyThemeColor(const std::string& text, const std::string& color_name,
                                bool use_bold = false) const;

    /**
     * @brief Generate HTML style definition based on current theme
     * @return CSS style definition string
     */
    std::string generateThemeStyleDefinition() const;

    /**
     * @brief Get the file extension for a visualization format
     * @param format The format to get the extension for
     * @return The file extension including the dot (e.g., ".txt")
     */
    std::string getFileExtension(VisualizationFormat format) const;

    /**
     * @brief Generate ASCII chart from data points
     * @param data The data points to visualize
     * @param width The chart width
     * @param height The chart height
     * @param title The chart title
     * @param x_label The label for the x-axis
     * @param y_label The label for the y-axis
     * @return ASCII chart string
     */
    std::string generateASCIIChart(const std::vector<double>& data, int width, int height,
                                   const std::string& title, const std::string& x_label = "",
                                   const std::string& y_label = "") const;

    /**
     * @brief Generate ASCII gauge for a single metric
     * @param value The metric value (0.0 to 1.0)
     * @param width The gauge width
     * @param title The gauge title
     * @return ASCII gauge string
     */
    std::string generateASCIIGauge(double value, int width, const std::string& title) const;

    /**
     * @brief Generate an ASCII bar with theme settings
     * @param value The value to represent (0.0-1.0)
     * @param width The width of the bar
     * @return Themed ASCII bar string
     */
    std::string generateThemedBar(double value, int width) const;

    /**
     * @brief Generate a report of a specific type
     * @param report_type The type of report to generate
     * @param format The output format
     * @return The formatted report
     */
    std::string generateReport(ReportType report_type, VisualizationFormat format [[maybe_unused]]);

    double calculateAethelImpact(RebelOperationType operation_type, int operation_count) const;

    std::shared_ptr<ResourceSnapshot> takeSnapshot(const std::string& context = "Default") const;

    /**
     * @brief Generate a summary report of resource usage
     * @param format The output format
     * @return The formatted summary report
     */
    std::string generateSummaryReport(VisualizationFormat format) const;

    /**
     * @brief Generate a detailed report of resource usage
     * @param format The output format
     * @param history_length Number of historical data points to include
     * @return The formatted detailed report
     */
    std::string generateDetailedReport(VisualizationFormat format, int history_length) const;

private:
    // Resource tracking and optimization components
    std::shared_ptr<TemporalRuntime> m_runtime;
    std::shared_ptr<ResourceOptimizer> m_optimizer;
    std::shared_ptr<TemporalDebtTracker> m_debt_tracker;
    std::shared_ptr<GraphicalResourceVisualizer> m_graph_visualizer;
    ResourceTracker* m_resource_tracker;
    // Added to manage ownership of ResourceTracker when created internally
    std::shared_ptr<ResourceTracker> m_owned_resource_tracker;

    // MODIFIED: Changed to store shared_ptr
    mutable std::deque<std::shared_ptr<ResourceSnapshot>> m_history;  // History of snapshots

    mutable std::mutex m_history_mutex;  // Mutex for thread-safe history access
    std::chrono::steady_clock::time_point m_last_update_time;
    int m_current_cycle;

    // Dashboard generation
    std::string generateDashboardHeader(VisualizationFormat format);
    std::string generateDashboardFooter(VisualizationFormat format);
    std::string generateResourceSection(VisualizationFormat format);
    std::string generateDebtSection(VisualizationFormat format);
    std::string generateEfficiencySection(VisualizationFormat format);
    std::string generateTrendsSection(VisualizationFormat format);

    // Real-time monitoring
    DashboardConfiguration m_dashboard_config;
    std::mutex m_monitor_mutex;
    std::thread m_monitor_thread;
    std::atomic<bool> m_monitoring_active;
    std::function<void(const std::string&)> m_update_callback;
    void monitorResources();

    // File operations
    bool writeToFile(const std::string& content, const std::string& filename) const;

    // Helper methods for predictive analytics
    double calculateResourceTrend(ResourceType type);
    double calculateDebtTrend();
    double calculateStabilityTrend();
    double applyRandomVariation(double uncertainty);
    double calculateProjectedEfficiency(double projected, double current);

    // Visualization theme settings
    VisualizationTheme m_theme;

    // Resource snapshot formatting
    std::string formatSnapshotAsText(const ResourceSnapshot& snapshot) const;
    std::string formatSnapshotAsJSON(const ResourceSnapshot& snapshot) const;
    std::string formatSnapshotAsCSV(const ResourceSnapshot& snapshot) const;
    std::string formatSnapshotCompact(const ResourceSnapshot& snapshot) const;

    // Helper for generating timestamp strings
    std::string getCurrentTimestampString() const;

    // Snapshot visualization
    std::string generateASCIIVisualization(const std::vector<ResourceSnapshot>& snapshots,
                                           int width, int height) const;

    /**
     * @brief Check if enough time has passed for a dashboard update
     * @return True if an update should occur, false otherwise
     */
    bool shouldUpdateDashboard() const;

    /**
     * @brief Generate a dashboard based on current configuration
     * @param format The output format
     * @return The formatted dashboard
     */
    std::string generateConfiguredDashboard(VisualizationFormat format) const;

    /**
     * @brief Adjust the dashboard based on the current mode
     * @param mode The current dashboard mode
     * @return Adjusted dashboard configuration
     */
    DashboardConfiguration adjustConfigForMode(DashboardMode mode) const;

    /**
     * @brief Generate an efficiency report focused on optimization metrics
     * @param format The output format
     * @return The formatted report
     */
    std::string generateEfficiencyReport(VisualizationFormat format) const;

    /**
     * @brief Generate a debt analysis report
     * @param format The output format
     * @return The formatted report
     */
    std::string generateDebtAnalysisReport(VisualizationFormat format) const;

    /**
     * @brief Generate a historical trends report
     * @param format The output format
     * @param history_length Number of historical data points to include
     * @return The formatted report
     */
    std::string generateHistoricalTrendsReport(VisualizationFormat format,
                                               int history_length) const;

    /**
     * @brief Generate a complete report with all available information
     * @param format The output format
     * @param history_length Number of historical data points to include
     * @return The formatted report
     */
    std::string generateCompleteReport(VisualizationFormat format, int history_length) const;
};

}  // namespace chronovyan

#endif  // CHRONOVYAN_RESOURCE_VISUALIZATION_H