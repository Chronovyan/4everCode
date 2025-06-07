#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

// Simplified ResourceDataPoint structure
struct ResourceDataPoint {
    std::chrono::time_point<std::chrono::system_clock> timestamp;
    double chronon_usage;
    double aethel_usage;
    double temporal_debt;
    double paradox_risk;
};

// Simplified ResourceTracker class
class ResourceTracker {
public:
    ResourceTracker(size_t max_history = 1000) : max_history_size_(max_history) {}

    void recordCurrentUsage(double chronon_usage, double aethel_usage, double temporal_debt = 0.0,
                            double paradox_risk = 0.0) {
        ResourceDataPoint point;
        point.timestamp = std::chrono::system_clock::now();
        point.chronon_usage = chronon_usage;
        point.aethel_usage = aethel_usage;
        point.temporal_debt = temporal_debt;
        point.paradox_risk = paradox_risk;

        history_.push_back(point);

        // Limit history size
        if (history_.size() > max_history_size_) {
            history_.erase(history_.begin());
        }

        updateStatistics();
    }

    const std::vector<ResourceDataPoint>& getHistoricalData() const { return history_; }

    std::map<std::string, double> getResourceStatistics() const { return statistics_; }

    void reset() {
        history_.clear();
        statistics_.clear();
    }

private:
    std::vector<ResourceDataPoint> history_;
    size_t max_history_size_;
    std::map<std::string, double> statistics_;

    void updateStatistics() {
        if (history_.empty()) {
            return;
        }

        // Calculate basic statistics
        double total_chronon = 0.0;
        double total_aethel = 0.0;
        double total_debt = 0.0;
        double total_risk = 0.0;

        double max_chronon = 0.0;
        double max_aethel = 0.0;
        double max_debt = 0.0;
        double max_risk = 0.0;

        for (const auto& point : history_) {
            total_chronon += point.chronon_usage;
            total_aethel += point.aethel_usage;
            total_debt += point.temporal_debt;
            total_risk += point.paradox_risk;

            max_chronon = std::max(max_chronon, point.chronon_usage);
            max_aethel = std::max(max_aethel, point.aethel_usage);
            max_debt = std::max(max_debt, point.temporal_debt);
            max_risk = std::max(max_risk, point.paradox_risk);
        }

        size_t count = history_.size();

        statistics_["current_chronon_usage"] = history_.back().chronon_usage;
        statistics_["current_aethel_usage"] = history_.back().aethel_usage;
        statistics_["current_temporal_debt"] = history_.back().temporal_debt;
        statistics_["current_paradox_risk"] = history_.back().paradox_risk;

        statistics_["avg_chronon_usage"] = total_chronon / count;
        statistics_["avg_aethel_usage"] = total_aethel / count;
        statistics_["avg_temporal_debt"] = total_debt / count;
        statistics_["avg_paradox_risk"] = total_risk / count;

        statistics_["max_chronon_usage"] = max_chronon;
        statistics_["max_aethel_usage"] = max_aethel;
        statistics_["max_temporal_debt"] = max_debt;
        statistics_["max_paradox_risk"] = max_risk;
    }
};

// Advanced Resource Visualization Classes

enum class VisualizationOutputFormat {
    ASCII,  // Enhanced ASCII art with colors for terminal
    SVG,    // Scalable Vector Graphics for web or documents
    HTML,   // HTML with embedded charts
    JSON,   // JSON data format for external visualization tools
    PNG     // PNG image format (requires external library)
};

enum class ChartType {
    LINE,     // Line chart showing trends over time
    BAR,      // Bar chart for comparing values
    HEATMAP,  // Heatmap showing intensity of resource usage
    SCATTER,  // Scatter plot for correlation analysis
    AREA,     // Area chart for cumulative trends
    COMBINED  // Multiple chart types in one visualization
};

struct ChartConfiguration {
    size_t width = 100;                    // Width of the chart in characters or pixels
    size_t height = 30;                    // Height of the chart in characters or pixels
    std::string title = "";                // Chart title
    std::vector<std::string> labels = {};  // Labels for data points
    bool show_grid = true;                 // Whether to show grid lines
    bool show_legend = true;               // Whether to show the legend
    std::array<const char*, 6> colors = {
        // Default color scheme (ANSI colors for terminal)
        "\033[31m",  // Red
        "\033[32m",  // Green
        "\033[33m",  // Yellow
        "\033[34m",  // Blue
        "\033[35m",  // Magenta
        "\033[36m"   // Cyan
    };
    const char* reset_color = "\033[0m";  // Reset color code
};

class AdvancedResourceVisualizer {
public:
    AdvancedResourceVisualizer(const ResourceTracker& tracker) : tracker_(tracker) {}

    std::string generateResourceVisualization(
        VisualizationOutputFormat format = VisualizationOutputFormat::ASCII,
        ChartType chart_type = ChartType::LINE,
        const ChartConfiguration& config = ChartConfiguration()) const {
        const auto& data = tracker_.getHistoricalData();
        if (data.empty()) {
            return "No data available for visualization.";
        }

        // Extract resource usage values
        std::vector<double> chronon_values;
        std::vector<double> aethel_values;

        for (const auto& point : data) {
            chronon_values.push_back(point.chronon_usage);
            aethel_values.push_back(point.aethel_usage);
        }

        // Create a combined visualization
        std::stringstream ss;

        // Add title
        std::string title =
            config.title.empty() ? "Chronovyan Resource Visualization" : config.title;

        switch (format) {
            case VisualizationOutputFormat::ASCII: {
                // Create an ASCII visualization
                ss << "═══════════════════════════════════════════════════════════\n";
                ss << "║ " << title << "\n";
                ss << "═══════════════════════════════════════════════════════════\n\n";

                // Generate appropriate chart based on type
                switch (chart_type) {
                    case ChartType::LINE:
                        ss << "Chronon Usage (Line Chart):\n";
                        ss << generateAsciiLineChart(chronon_values, config) << "\n\n";
                        ss << "Aethel Usage (Line Chart):\n";
                        ss << generateAsciiLineChart(aethel_values, config) << "\n\n";
                        break;

                    case ChartType::BAR:
                        ss << "Chronon Usage (Bar Chart):\n";
                        ss << generateAsciiBarChart(chronon_values, config) << "\n\n";
                        ss << "Aethel Usage (Bar Chart):\n";
                        ss << generateAsciiBarChart(aethel_values, config) << "\n\n";
                        break;

                    case ChartType::COMBINED:
                        ss << "Combined Resource Visualization:\n";
                        ss << generateAsciiCombinedChart(chronon_values, aethel_values, config);
                        break;

                    default:
                        ss << "Unsupported chart type for ASCII output.";
                        break;
                }
                break;
            }

            case VisualizationOutputFormat::JSON: {
                ss << "{\n";
                ss << "  \"title\": \"" << title << "\",\n";
                ss << "  \"chronon_data\": " << generateJsonData(chronon_values, config) << ",\n";
                ss << "  \"aethel_data\": " << generateJsonData(aethel_values, config) << "\n";
                ss << "}\n";
                break;
            }

            default:
                ss << "Unsupported visualization format.";
                break;
        }

        return ss.str();
    }

    // Helper methods for chart generation

    std::string generateAsciiLineChart(const std::vector<double>& values,
                                       const ChartConfiguration& config) const {
        if (values.empty()) {
            return "[No data]";
        }

        const size_t width = std::min(config.width, values.size());
        const size_t height = config.height;

        // Normalize values to fit within the height
        auto normalized = normalizeValues(values, height);

        // Build the chart
        std::vector<std::string> grid(height);
        for (size_t y = 0; y < height; ++y) {
            grid[y] = std::string(width, ' ');
        }

        // Plot the line chart
        for (size_t x = 0; x < width; ++x) {
            if (x < normalized.size()) {
                size_t y_pos = height - normalized[x] - 1;
                if (y_pos < height) {
                    grid[y_pos][x] = '*';

                    // Connect points with lines
                    if (x > 0 && x < normalized.size()) {
                        size_t prev_y = height - normalized[x - 1] - 1;
                        if (prev_y < height) {
                            // Draw vertical line to connect points
                            size_t start_y = std::min(y_pos, prev_y);
                            size_t end_y = std::max(y_pos, prev_y);

                            for (size_t line_y = start_y + 1; line_y < end_y; ++line_y) {
                                grid[line_y][x] = '|';
                            }
                        }
                    }
                }
            }
        }

        // Build the chart string with frame
        std::stringstream ss;

        // Add y-axis labels and chart frame
        for (size_t y = 0; y < height; ++y) {
            ss << "│ ";

            for (size_t x = 0; x < width; ++x) {
                char point = grid[y][x];
                if (point == '*') {
                    ss << config.colors[0] << point << config.reset_color;
                } else if (point == '|') {
                    ss << config.colors[1] << point << config.reset_color;
                } else {
                    ss << point;
                }
            }

            ss << " │\n";
        }

        // Add x-axis
        ss << "└─" << std::string(width, '─') << "─┘\n";

        return ss.str();
    }

    std::string generateAsciiBarChart(const std::vector<double>& values,
                                      const ChartConfiguration& config) const {
        if (values.empty()) {
            return "[No data]";
        }

        const size_t width = std::min(config.width, values.size() * 2);
        const size_t height = config.height;

        // Normalize values to fit within the height
        auto normalized = normalizeValues(values, height);

        // Generate the bar chart
        std::stringstream ss;

        // Draw each row of the chart from top to bottom
        for (size_t y = 0; y < height; ++y) {
            ss << "│ ";

            size_t row = height - y - 1;  // Invert y-coordinate (0 = bottom)

            // Draw bars for this row
            for (size_t i = 0; i < std::min(normalized.size(), width / 2); ++i) {
                if (normalized[i] >= row) {
                    // Part of a bar
                    ss << config.colors[0] << "█" << config.reset_color;
                } else {
                    // Empty space
                    ss << " ";
                }

                // Add space between bars
                ss << " ";
            }

            ss << "│\n";
        }

        // Add x-axis
        ss << "└─" << std::string(width, '─') << "─┘\n";

        return ss.str();
    }

    std::string generateAsciiCombinedChart(const std::vector<double>& chronon_values,
                                           const std::vector<double>& aethel_values,
                                           const ChartConfiguration& config) const {
        if (chronon_values.empty() || aethel_values.empty()) {
            return "[No data available for combined chart]";
        }

        const size_t width = config.width;
        const size_t height = config.height;

        // Find max values for scaling
        double max_chronon = *std::max_element(chronon_values.begin(), chronon_values.end());
        double max_aethel = *std::max_element(aethel_values.begin(), aethel_values.end());

        // Create normalized values for display
        std::vector<size_t> norm_chronon = normalizeValues(chronon_values, height);
        std::vector<size_t> norm_aethel = normalizeValues(aethel_values, height);

        // Create a combined visualization
        std::stringstream ss;

        // Chart title and header
        ss << "╔" << std::string(width - 2, '═') << "╗\n";
        ss << "║ " << config.title << std::string(width - config.title.length() - 4, ' ') << "║\n";
        ss << "╠" << std::string(width - 2, '═') << "╣\n";

        // Draw the chart grid
        std::vector<std::string> grid(height);
        for (size_t row = 0; row < height; ++row) {
            std::string line = "║ ";

            // Add data points for current row
            size_t y_pos = height - row - 1;

            for (size_t x = 0; x < std::min(width - 4, norm_chronon.size()); ++x) {
                char cell = ' ';
                std::string color = "";
                std::string reset = config.reset_color;

                // Determine which points to plot for this cell
                bool has_chronon = (x < norm_chronon.size() && norm_chronon[x] == y_pos);
                bool has_aethel = (x < norm_aethel.size() && norm_aethel[x] == y_pos);

                // Color code the different metrics
                if (has_chronon) {
                    cell = 'C';
                    color = config.colors[0];  // Red for Chronon
                }
                if (has_aethel) {
                    cell = 'A';
                    color = config.colors[1];  // Green for Aethel
                }

                // If both metrics in same position, use a special character
                if (has_chronon && has_aethel) {
                    cell = '*';
                    color = config.colors[4];  // Magenta for overlap
                }

                line += color + cell + reset;
            }

            // Pad to full width
            line += std::string(width - line.length() + reset.length() - 1, ' ') + "║";
            grid[row] = line;
        }

        // Add the grid to the output
        for (const auto& line : grid) {
            ss << line << "\n";
        }

        // Add x-axis
        ss << "╚" << std::string(width - 2, '═') << "╝\n";

        // Add legend
        ss << "\nLegend:\n";
        ss << config.colors[0] << "C" << config.reset_color << " - Chronon Usage\n";
        ss << config.colors[1] << "A" << config.reset_color << " - Aethel Usage\n";
        ss << config.colors[4] << "*" << config.reset_color << " - Both Metrics\n";

        return ss.str();
    }

    std::string generateJsonData(const std::vector<double>& values,
                                 const ChartConfiguration& config) const {
        std::stringstream ss;
        ss << "[";

        for (size_t i = 0; i < values.size(); ++i) {
            ss << values[i];
            if (i < values.size() - 1) {
                ss << ", ";
            }
        }

        ss << "]";

        return ss.str();
    }

    std::vector<size_t> normalizeValues(const std::vector<double>& values, size_t height) const {
        if (values.empty()) {
            return {};
        }

        // Find the maximum value for scaling
        double max_value = *std::max_element(values.begin(), values.end());

        // If all values are 0, return a flat line
        if (max_value == 0) {
            return std::vector<size_t>(values.size(), 0);
        }

        // Scale values to fit within the height range
        std::vector<size_t> normalized(values.size());
        for (size_t i = 0; i < values.size(); ++i) {
            normalized[i] = static_cast<size_t>((values[i] / max_value) * (height - 1));
        }

        return normalized;
    }

private:
    const ResourceTracker& tracker_;
};

// Helper function to generate sample resource data
void generateSampleData(ResourceTracker& tracker, int count) {
    // Generate sine wave pattern for chronon usage
    for (int i = 0; i < count; ++i) {
        double chronon_usage = 50.0 + 45.0 * std::sin(i * 0.2);
        double aethel_usage = 30.0 + 25.0 * std::cos(i * 0.3);
        double temporal_debt = 10.0 + 8.0 * std::sin(i * 0.1);
        double paradox_risk = 0.05 + 0.3 * std::abs(std::sin(i * 0.4));

        tracker.recordCurrentUsage(chronon_usage, aethel_usage, temporal_debt, paradox_risk);

        // Add small delay to simulate real-time data collection
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main() {
    std::cout << "Advanced Resource Visualization Demo" << std::endl;
    std::cout << "===================================" << std::endl << std::endl;

    // Create resource tracker
    ResourceTracker tracker;

    // Generate sample data
    std::cout << "Generating sample resource data..." << std::endl;
    generateSampleData(tracker, 50);
    std::cout << "Sample data generated." << std::endl << std::endl;

    // Create visualizer
    AdvancedResourceVisualizer visualizer(tracker);

    // Test different visualization types
    std::cout << "LINE CHART VISUALIZATION:" << std::endl;
    std::cout << visualizer.generateResourceVisualization(VisualizationOutputFormat::ASCII,
                                                          ChartType::LINE)
              << std::endl;

    std::cout << "BAR CHART VISUALIZATION:" << std::endl;
    std::cout << visualizer.generateResourceVisualization(VisualizationOutputFormat::ASCII,
                                                          ChartType::BAR)
              << std::endl;

    std::cout << "COMBINED CHART VISUALIZATION:" << std::endl;
    std::cout << visualizer.generateResourceVisualization(VisualizationOutputFormat::ASCII,
                                                          ChartType::COMBINED)
              << std::endl;

    // Test JSON output
    std::cout << "JSON DATA VISUALIZATION:" << std::endl;
    std::cout << visualizer.generateResourceVisualization(VisualizationOutputFormat::JSON,
                                                          ChartType::LINE)
              << std::endl;

    std::cout << "Demo completed successfully." << std::endl;

    return 0;
}